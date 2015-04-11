#include <boost/thread/thread.hpp>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

void thread_proc()
{
    int i = 0;
    while(i<100) {
        LOG(INFO) << " thread_id=" << pthread_self() << " i=" << i;
        i++;
    }
}

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    pid_t pid = fork();
    if (pid < 0) {
        //error;
        return 0;
    }

    if (pid>0) { // in father
        int status = 0;
        pid_t p = waitpid(-1, &status, 0);
        assert(pid==p);
        printf("child exit status=%d\n", status);
    } 
    
    // in child  
    google::InitGoogleLogging("deam.log"); 
    //FLAGS_log_dir = "./";
    //FLAGS_stderrthreshold = 0;

    boost::thread_group threads;

    for (int i=0; i<10; ++i) {
        threads.create_thread(&thread_proc);
    }  

    threads.join_all();

    google::FlushLogFiles(0);    
    google::ShutdownGoogleLogging();  
}


