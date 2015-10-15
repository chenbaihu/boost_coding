#ifndef _LOG_H_
#define _LOG_H_

//TODO 实现这几个宏，替换掉使用printf

#define print_debug_info(fmt, ...)            \
    do {                                      \
        fprintf(stdout, fmt, ##__VA_ARGS__);  \
    } while(0); 

#define print_notice_info(fmt, ...)           \
    do {                                      \
        fprintf(stdout, fmt, ##__VA_ARGS__);  \
    } while(0); 

#define print_error_info(fmt, ...)            \
    do {                                      \
        fprintf(stderr, fmt, ##__VA_ARGS__);  \
    } while(0); 

#endif

