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


     // 实现业务逻辑(加载配置，启动socket等等)      

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

