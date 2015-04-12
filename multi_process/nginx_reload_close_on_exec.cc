/*
   Nginx fork + exec的reload方式会导致没有设置为close on exec的fd泄露.

基础知识：
   句柄设置close on exec:
   int flags = fcntl(fd, F_GETFD);  
   flags |= FD_CLOEXEC;  
   fcntl(fd, F_SETFD, flags);

   没有设置close on exec的fd在nginx的fork exec(平滑升级)中会出现句柄泄露:
   具体：
        在linux操作系统中，当进程退出时，文件会被关闭；而使用exec执行新的bin时，新的bin进程空间替换老的进程空间时；
        但是操作系统默认保留了老进程的文件句柄和pid(使用exec执行新程序时文件锁也是被继承的)；
 
   nginx的平滑升级方法：
        kill -USER2  curpid
        kill -QUIT    oldpid（由curpid在USER2信号时生成）
   过程中：kill -USER2 curpid时，master进程会fork新进程；调用exec执行nginx 的新bin。

   我看了一下nginx这部分实现：
   1.Nginx在fork+exec之前，将自身的所有环境变量传递给了新的master进程，并且使用了一个特殊的环境变量名(NGINX)传递监听的fd列表，
     格式为(NGINX=%ud;)，如果只监听一个fd为5，则NGINX=5; （有分号）。
    （见core/nginx.c: nginx_exec_new_binary() ）

   2.master进程启动时，通过判断是否存在 NGINX 环境变量来确定当前自身是全新进程，还是exec产生的新master，并从字符串取出fd值列表。
    （见core/nginx.c: ngx_add_inherited_sockets() ）

   3.最终这个环境变量会留在新的进程中。

   因此，可以在ngx_module_gas的初始化代码中
   1.通过判断NGINX环境变量是否存在，确定是否是一个exec出来的master进程，并拿到listen fd列表
   2.关闭除了listen fd之外的所有fd。有两种方法：
   a)遍历/proc/self/fd，关闭每个fd（除了listen fd）
   b)遍历0至getrlimit(RLIMIT_NOFILE)，关闭每个fd（除listen fds）

   fork + exec 如何通过环境变量将参数传递给子进程呢？
   #include  <unistd.h>
   int execl(const char* path, const char* arg, ...); 
   int execv(const char* path, char* const argv[]);
   int execle(const char* path,  const char* arg, ..., char* const envp[]);
   int execve(const char* path, char* const argv[], char* const envp[]);
   int execlp(const char* file, const char* arg, ... );
   int execvp(const char* file, char* const argv[]);

   error: return -1

   这6个函数在函数名和使用语法的规则上都有细微的区别，下面就从可执行件查找方式、参数传递方式和环境变量这几个方面进行比较。
   查找方式：
       前4个函数的查找方式都是完整的件目录路径，而最后两个函数（也就是以 p 结尾的两个函数）可以只给出件名，系统就会自动按照环境变量“$PATH” 所指定的路径进行查找。

   参数传递方式：
       exec函数族的参数传递有两种：
       一种是逐个列举的方式，
       而另一种则是将所有参数整体构造指针数组传递。
       在这里是以函数名的第5位字母来区分的，
       字母为 "l"(list)的表示逐个列举参数的方式，其语法为const char *arg; 
       字母为“v”(vector)的表示将所有参数整体构造指针数组传递，其语法为 char *const argv[]。
       这里的参数实际上就是用户在使用这个可执行件时所需的全部命令选项字符串（包括该可执行程序命令本身）。
       要注意的是，这些参数必须以NULL结束。

   环境变量： 
       exec函数族可以默认系统的环境变量，也可以传入指定的环境变量。这里以 “e”(environment)结尾的两个函数 execle()和 execve()就可以在 envp[]中指定当前进程所使用的环境变量。

   在子进程中如何获取和设置环境变量？
       getenv和setenv就可以使用了。(其实不用设置，默认使用的就是execve传递过来的envp数组环境变量)



   关于记录锁的自动继承和释放有三条规则::
   1、锁与进程和文件两方面有关::
        a、当一个进程终止时，它所建立的锁全部释放；(即  进程退出，文件锁自动释放)
        b、任何时候关闭一个描述符时，则该进程通过这一描述符可以引用的文件上的任何一把锁都释放。(即  关闭文件,文件锁自动释放)
        注意::
              情况一::
              fd1 = open(pathname,....);
              read_lock(fd1,....);
              fd2 = dup(fd1);
              close(fd2);                  //此时,在close(fd2)后，在fd1上加的锁被释放。
              情况二::
              fd1 = open(pathname,....);
              read_lock(fd1,....);
              fd2 = open(pathname,...);
              close(fd2);                  //此时，在close(fd2)后，在fd1上加的锁被释放。
   
   2、由fork产生的子进程不继承父进程所设置的锁。（文件锁不能被继承）
        这意味着，若一个进程得到一把锁，然后调用fork,那么对于父进程获得的锁而言，子进程被视为另外一个进程，对于从父进程处继承过来的任一描述符，子进程需要调用fcntl才能获得它自己的锁。
        (
             这其实也是锁的本意，文件锁的作用是阻止多个进程同时写一个文件或区域，如果子进程继承父进程的锁，则父、子就可以同时写同一个文件。这显然是不对的。
        )
   
   3、在执行exec后，新程序可以继承原执行程序的锁。（EXEC文件锁被继承）
        执行exec后，其实是用当前进程的进程实体替换原进程的进程实体。
        原进程被杀掉(但保留了 进程ID和文件句柄(如果文件句柄没有设置close on exec)；)
*/

bool closeonexec()
{
    char* listening_fds = getenv(NGINX_VAR);   // nginx 传递的fd参数信息
    if (!listening_fds) {
        return true;
    }

    pid_t ppid = getppid();
    if (ppid == 1) {
        return true;
    }

    assert(ppid == ngx_master_pid); 
    
    std::set<int> except_fds;   
    convert_except_fds(listening_fds, except_fds);      
    append_errorlog_fds(except_fds);
    daemon_close_allv(except_fds);

    return true;
}

