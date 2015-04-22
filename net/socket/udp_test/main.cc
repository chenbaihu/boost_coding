#include "udp_cli.h"
#include "udp_req.h"

#include <glog/logging.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <map>
#include <vector>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/ThreadPool.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/Atomic.h>

DEFINE_int32(thread_num,  1, "thread_num in thread_pool");
DEFINE_int32(udp_req_num, 1, "udp req num");
DEFINE_int32(data_len,    1, "udp req len");
DEFINE_string(server_ip,  "127.0.0.1", "server ip");
DEFINE_int32(server_port, 53, "server port");
DEFINE_bool(debug, false, "debug");
DEFINE_int32(total_count, 0, "0 mean not limit");

typedef boost::shared_ptr<UDP> UDPPtr;
typedef std::vector<UDPPtr>    UDPPtrVec;

bool g_quit = false;
void quit(int sig)
{
   g_quit = true; 
}

muduo::AtomicInt64 g_total_req;
void statis(muduo::net::EventLoop* loop)
{
    if (loop==NULL) {
        return;
    }   
    static int64_t last_total_req = 0;
    LOG(INFO) << "request speed=======================[" << g_total_req.get()-last_total_req << "]/s";
    last_total_req = g_total_req.get();
    if (g_total_req.get()>FLAGS_total_count && FLAGS_total_count!=0) {
        LOG(INFO) << "total_cout=" << FLAGS_total_count << " g_total_req=" << g_total_req.get();
        g_quit = true;
        return;
    }
    loop->runAfter(1.0, boost::bind(statis, loop));
}

void InitUdpPtrVec(UDPPtrVec& udp_ptr_vec, int udp_req_num) 
{
    int i = 0;
    for (i=0; i<udp_req_num; i++) {
        //UDPPtr udp_ptr(new UDP("106.39.202.170"/*w-hkill35.safe.bjdt.qihoo.net*/, 53));
        UDPPtr udp_ptr(new UDP(FLAGS_server_ip.c_str()/*w-hkill35.safe.bjdt.qihoo.net*/, FLAGS_server_port));
        if(!udp_ptr->CreateSocket()) {
            //exit(1);
            LOG(ERROR) << "InitUdpPtrVec create udp failed i=" << i << std::endl;
            continue;
        }
        udp_ptr_vec.push_back(udp_ptr); 
    }
}


void SendAndRecv(UDPPtr udp_ptr) 
{
    while(!g_quit) {
        if (udp_ptr==NULL) {
            LOG(ERROR) << "SendAndRecv udp_ptr empty" << std::endl;
            return;
        }

        std::string req;
        std::string exp;
        get_udp_req_and_exp(req, exp);

        std::string rsp;
        muduo::Timestamp beg = muduo::Timestamp::now();
        if (!udp_ptr->SendAndRecvData(req, rsp)) {
            //if (!udp_ptr->SendAndRecvData(req, rsp)) {
            LOG(ERROR) << "SendAndRecvData failed------------------";
            continue;
        } 

        muduo::Timestamp end = muduo::Timestamp::now(); 
        std::string rsp_b64 = rsp;
        if (FLAGS_debug) {
            LOG(INFO) << "rsp_len=" << rsp.length() << " rsp_b64=" << rsp_b64;
        }

        //if (exp.compare(rsp_b64)==0) {
        if (exp.compare(rsp_b64)==0||rsp_b64.length()>300) {
            LOG(INFO) << "rsp [ ok ] rsp_len=" << rsp.length() << "\tuse_time=" << muduo::timeDifference(end, beg)*1000;
            g_total_req.increment();
        }
    }
}

int main(int argc, char* argv[])
{
    ::google::ParseCommandLineFlags(&argc, &argv, false);
    google::InitGoogleLogging(argv[0]);
    //FLAGS_stderrthreshold=3;
    //FLAGS_stderrthreshold=0;

    signal(SIGTERM, quit); 

    UDPPtrVec udp_ptr_vec;
    int udp_req_num = FLAGS_udp_req_num;
    if (udp_req_num<=0) {  
        udp_req_num = 1; 
    }
    InitUdpPtrVec(udp_ptr_vec, udp_req_num);

    int thread_num = FLAGS_thread_num;
    if (thread_num<=0) { 
        thread_num = 1;
    }
    LOG(INFO) << "input:" << argv[0] << "\t" << thread_num << "\t" << udp_req_num << std::endl;

    muduo::net::EventLoopThread event_loop_thread;
    muduo::net::EventLoop* loop = event_loop_thread.startLoop();
    if (loop==NULL) { 
        LOG(INFO) << "event_loop_thread.startLoop return NULL" << std::endl;
        return 0;
    }
    loop->runAfter(1.0, boost::bind(statis, loop));

    muduo::ThreadPool thread_pool(std::string("soap_test_thread_pool"));
    thread_pool.start(thread_num); 
    for (size_t i=0; i < udp_ptr_vec.size(); i++) { 
        thread_pool.run(boost::bind(SendAndRecv, udp_ptr_vec[i]));
    }

    while(!g_quit) {
        sleep(1);
    }

    thread_pool.stop();
    udp_ptr_vec.clear(); 
    return 0;
}
