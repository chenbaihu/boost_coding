/*
   Nginx fork + exec��reload��ʽ�ᵼ��û������Ϊclose on exec��fdй¶.

����֪ʶ��
   �������close on exec:
   int flags = fcntl(fd, F_GETFD);  
   flags |= FD_CLOEXEC;  
   fcntl(fd, F_SETFD, flags);

   û������close on exec��fd��nginx��fork exec(ƽ������)�л���־��й¶:
   ���壺
        ��linux����ϵͳ�У��������˳�ʱ���ļ��ᱻ�رգ���ʹ��execִ���µ�binʱ���µ�bin���̿ռ��滻�ϵĽ��̿ռ�ʱ��
        ���ǲ���ϵͳĬ�ϱ������Ͻ��̵��ļ������pid(ʹ��execִ���³���ʱ�ļ���Ҳ�Ǳ��̳е�)��
 
   nginx��ƽ������������
        kill -USER2  curpid
        kill -QUIT    oldpid����curpid��USER2�ź�ʱ���ɣ�
   �����У�kill -USER2 curpidʱ��master���̻�fork�½��̣�����execִ��nginx ����bin��

   �ҿ���һ��nginx�ⲿ��ʵ�֣�
   1.Nginx��fork+exec֮ǰ������������л����������ݸ����µ�master���̣�����ʹ����һ������Ļ���������(NGINX)���ݼ�����fd�б�
     ��ʽΪ(NGINX=%ud;)�����ֻ����һ��fdΪ5����NGINX=5; ���зֺţ���
    ����core/nginx.c: nginx_exec_new_binary() ��

   2.master��������ʱ��ͨ���ж��Ƿ���� NGINX ����������ȷ����ǰ������ȫ�½��̣�����exec��������master�������ַ���ȡ��fdֵ�б�
    ����core/nginx.c: ngx_add_inherited_sockets() ��

   3.����������������������µĽ����С�

   ��ˣ�������ngx_module_gas�ĳ�ʼ��������
   1.ͨ���ж�NGINX���������Ƿ���ڣ�ȷ���Ƿ���һ��exec������master���̣����õ�listen fd�б�
   2.�رճ���listen fd֮�������fd�������ַ�����
   a)����/proc/self/fd���ر�ÿ��fd������listen fd��
   b)����0��getrlimit(RLIMIT_NOFILE)���ر�ÿ��fd����listen fds��

   fork + exec ���ͨ�������������������ݸ��ӽ����أ�
   #include  <unistd.h>
   int execl(const char* path, const char* arg, ...); 
   int execv(const char* path, char* const argv[]);
   int execle(const char* path,  const char* arg, ..., char* const envp[]);
   int execve(const char* path, char* const argv[], char* const envp[]);
   int execlp(const char* file, const char* arg, ... );
   int execvp(const char* file, char* const argv[]);

   error: return -1

   ��6�������ں�������ʹ���﷨�Ĺ����϶���ϸ΢����������ʹӿ�ִ�м����ҷ�ʽ���������ݷ�ʽ�ͻ��������⼸��������бȽϡ�
   ���ҷ�ʽ��
       ǰ4�������Ĳ��ҷ�ʽ���������ļ�Ŀ¼·�������������������Ҳ������ p ��β����������������ֻ����������ϵͳ�ͻ��Զ����ջ���������$PATH�� ��ָ����·�����в��ҡ�

   �������ݷ�ʽ��
       exec������Ĳ������������֣�
       һ��������оٵķ�ʽ��
       ����һ�����ǽ����в������幹��ָ�����鴫�ݡ�
       ���������Ժ������ĵ�5λ��ĸ�����ֵģ�
       ��ĸΪ "l"(list)�ı�ʾ����оٲ����ķ�ʽ�����﷨Ϊconst char *arg; 
       ��ĸΪ��v��(vector)�ı�ʾ�����в������幹��ָ�����鴫�ݣ����﷨Ϊ char *const argv[]��
       ����Ĳ���ʵ���Ͼ����û���ʹ�������ִ�м�ʱ�����ȫ������ѡ���ַ����������ÿ�ִ�г����������
       Ҫע����ǣ���Щ����������NULL������

   ���������� 
       exec���������Ĭ��ϵͳ�Ļ���������Ҳ���Դ���ָ���Ļ��������������� ��e��(environment)��β���������� execle()�� execve()�Ϳ����� envp[]��ָ����ǰ������ʹ�õĻ���������

   ���ӽ�������λ�ȡ�����û���������
       getenv��setenv�Ϳ���ʹ���ˡ�(��ʵ�������ã�Ĭ��ʹ�õľ���execve���ݹ�����envp���黷������)



   ���ڼ�¼�����Զ��̳к��ͷ�����������::
   1��������̺��ļ��������й�::
        a����һ��������ֹʱ��������������ȫ���ͷţ�(��  �����˳����ļ����Զ��ͷ�)
        b���κ�ʱ��ر�һ��������ʱ����ý���ͨ����һ�������������õ��ļ��ϵ��κ�һ�������ͷš�(��  �ر��ļ�,�ļ����Զ��ͷ�)
        ע��::
              ���һ::
              fd1 = open(pathname,....);
              read_lock(fd1,....);
              fd2 = dup(fd1);
              close(fd2);                  //��ʱ,��close(fd2)����fd1�ϼӵ������ͷš�
              �����::
              fd1 = open(pathname,....);
              read_lock(fd1,....);
              fd2 = open(pathname,...);
              close(fd2);                  //��ʱ����close(fd2)����fd1�ϼӵ������ͷš�
   
   2����fork�������ӽ��̲��̳и����������õ��������ļ������ܱ��̳У�
        ����ζ�ţ���һ�����̵õ�һ������Ȼ�����fork,��ô���ڸ����̻�õ������ԣ��ӽ��̱���Ϊ����һ�����̣����ڴӸ����̴��̳й�������һ���������ӽ�����Ҫ����fcntl���ܻ�����Լ�������
        (
             ����ʵҲ�����ı��⣬�ļ�������������ֹ�������ͬʱдһ���ļ�����������ӽ��̼̳и����̵������򸸡��ӾͿ���ͬʱдͬһ���ļ�������Ȼ�ǲ��Եġ�
        )
   
   3����ִ��exec���³�����Լ̳�ԭִ�г����������EXEC�ļ������̳У�
        ִ��exec����ʵ���õ�ǰ���̵Ľ���ʵ���滻ԭ���̵Ľ���ʵ�塣
        ԭ���̱�ɱ��(�������� ����ID���ļ����(����ļ����û������close on exec)��)
*/

bool closeonexec()
{
    char* listening_fds = getenv(NGINX_VAR);   // nginx ���ݵ�fd������Ϣ
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

