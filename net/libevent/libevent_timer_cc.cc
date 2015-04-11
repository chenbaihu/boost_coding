#include <stdio.h>
#include <iostream>
using namespace std;

// libevent头文件
#include <event.h>

typedef void(*time_fun_type)();

class Timer {
    public:
        Timer(struct event_base* base):base_(base) {};
        ~Timer() {};

    public:
        bool Run(struct timeval tv, time_fun_type time_fun) {
            if (base_==NULL) {
                return false;
            }
            tv_       = tv;
            time_fun_ = time_fun;

            // 设置事件回调函数
            evtimer_set(&ev_timer_, Timer::OnTimer, this);

            // 将当前事件加入主事件循环管理   
            event_base_set(base_, &ev_timer_);

            // 添加定时事件
            event_add(&ev_timer_, &tv_);
        }

        struct event*   ev_timer() { return &ev_timer_; }
        time_fun_type   time_fun() { return time_fun_;  }
        struct timeval* tv() { return &tv_; }
    private:
        static void OnTimer(int sock, short event, void *arg) {
            Timer* timer = static_cast<Timer*>(arg);
            timer->time_fun()();

            // 重新添加定时事件（定时事件触发后默认自动删除）
            event_add(timer->ev_timer(), timer->tv());
        }

    private:
        struct event_base* base_;
        struct event       ev_timer_;
        struct timeval     tv_;
        time_fun_type      time_fun_;
};

// 定时事件回调函数
void time_fun()
{ 
    cout << "Game Over!" << endl;
}

int main()
{
    struct event_base* base;
    base = event_base_new();

    struct timeval tv;
    tv.tv_sec  = 1;
    tv.tv_usec = 0;
    Timer timer(base);
    timer.Run(tv, time_fun);

    // 事件循环
    event_base_dispatch(base);
    return 0;
}


