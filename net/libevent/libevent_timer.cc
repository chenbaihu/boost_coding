#include <stdio.h>
#include <iostream>
using namespace std;

// libevent头文件
#include <event.h>

// 定时事件回调函数
void onTime(int sock, short event, void *arg)
{
    cout << "Game Over!"
        << " sock="  << sock
        << " event=" << event
        << endl;

    struct timeval tv;
    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    // 重新添加定时事件（定时事件触发后默认自动删除）
    event_add((struct event*)arg, &tv);
}

int main()
{

    struct event_base* base;
    base = event_base_new();

    struct event evTime; 
    // 设置事件
    evtimer_set(&evTime, onTime, &evTime);

    event_base_set(base, &evTime);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // 添加定时事件
    event_add(&evTime, &tv);


    // 事件循环
    event_base_dispatch(base);

    return 0;
}

