/*

��������ͨ�õĲ���ģ��:

   ��1��accept + read/write 
        �ܶԸ�һ��򵥵�������Ӧ����

   ��2��accept + fork          
        һ�����Ӵ���һ�����̴���Qcmd�ͻ��˾�������ģ�ͣ�

   ��3��accept + thread          (mongodb) 
        һ�����Ӵ���һ���̴߳���Mongodb������ģ�ͣ�

   ��4��pre fork 
        ����̸����apache��fpm�Ĳ���ģ�ͣ���������ӵķ�ʽ�ǣ�
        lock����
        accept����
        unlock������һ������ͬʱֻ�ܴ���һ�����ӣ������н��̶�æʱ�����ӽ����������ں˵�������Ӷ����С�

   ��5��pre thread 
        ����4�����ƣ�����ڽ�����˵�̵߳�ȫ�����ݿ��Թ���

   ��6��pool��reactor��
        ���߳�Reactor�������Ƚ��ʺ�IO�ܼ���Ӧ�ã����ʺ�CPU�ܼ���Ӧ�ã�

   ��7��reactor+thread-per-task  
        �루6�������һ�����ɷ������Ѽ���ŵ�����һ���߳������������и�ȱ�����һ������Ķ������������ŵ��˲�ͬ���߳��д������ܱ�֤˳���ԣ�

   ��8��reactor + work thread    (mysql����)
        Ϊ�˱�֤˳���ԣ�Ϊÿ�����Ӵ���һ�������̣߳����ǲ����������������߳�����mysql������ģ�ͣ�

   ��9��reactors + thread pool Ϊ�˱���
   ��7��Ϊÿ�����񴴽�һ���̵߳�ȱ�ݶ�������̶���С���̳߳أ�reactor����IO������thread poolֻ������㣬��������˴˶�����IOѹ�������Ӧ�ã�
   ��7����8����9������reactor����������Ӻ�IO��

   ��10�� reactors in threads    (muduo��, memcache��ʵ��)
   ÿ���̶߳���reactor�����߳���Ҫ����accept ���ӣ�Ȼ������ӷ������reactor���̣߳��������Գ������CPUͬʱ��IO���䵽ÿ���̷߳�ֹ����һ��reactor�Ĵ����������͵�����;
   ������
   atlas ������Ϊ������ģ�ͣ�������������atlas���߳�Ҳ�����ǹ����̣߳���ģ��ȱ�������߳�CPU���ܻ�ܸߣ�

   ��11�� reactors in processes         (nginx����)
   nginx�Ĳ���ģ�ͣ���������֮���໥��������������������Ҫע���������ģ�Ͳ����ڴ������д���������㣬��������������������������ܽ�������½���
   ������
   nginx�Ĳ���ģ�ͣ�

   ��12�� reactors + pool �ѣ�9���ͣ�10����ϣ��ö��IO thread������IO����ʹ���̳߳���������㣬���ʺ�ͻ��IO�����ʺ�ͻ�������Ӧ�á�

*/

#include <time.h>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/time.h>
#include <errno.h>

void Daemon()
{
    pid_t pid;

    if ((pid = fork()) != 0) { exit(0); }

    setsid();
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGPIPE, &sig, NULL);

    if ((pid = fork()) != 0)    { exit(0); }

    //chdir("/");

    umask(0);

    for (int i = 3; i < 64; i++)
        close(i);
}

int work_process_run() {
    Daemon();


     // ʵ��ҵ���߼�(�������ã�����socket�ȵ�)      

    exit(0);
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < thread_num; i++) {
        pid_t pid = fork();
        switch (pid) {
            case -1: // fork error
                pid = getpid();
                printf("(%d) forks children fail\n",pid);
                break;

            case 0: //child process
                return work_process_run();

            default: //parent process
                printf("main_process fork child(%d) success\n",pid);
                usleep(20000);
                continue;
        }
    }

    return 0;
}

