#include "time_helper.h"

#include <cstdlib>
#include <math.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <net/if.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/ioctl.h>

namespace util {

uint64_t TimeHelper::time()
{
    return ::time(0);
}

uint64_t TimeHelper::timeus() {
    timeval time;
    ::gettimeofday(&time, 0);
    return time.tv_sec*1000LL*1000LL + time.tv_usec;
}

uint64_t TimeHelper::timems() {
    timeval time;
    ::gettimeofday(&time, 0);
    return time.tv_sec*1000 + time.tv_usec/1000;
}

double TimeHelper::timef()
{
    timeval time;
    ::gettimeofday(&time, 0);
    return time.tv_sec + time.tv_usec/1000.0/1000.0;
}

} //namespace util

//int main_test(){
//	string ip ;
//	ip = TimeHelper::local_ip();
//	//ip = GetLocalIp();
//	printf("ip=%s",ip.c_str());
//    return 0;
//}
