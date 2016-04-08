#ifndef _CLIB_LOG_H_
#define _CLIB_LOG_H_

#include <stdint.h>

namespace clib {

//TODO 实现这几个宏，替换掉使用printf

#define log_notice(fmt, ...)                  \
    do {                                      \
        fprintf(stdout, fmt, ##__VA_ARGS__);  \
    } while(0); 

#define log_warn(fmt, ...)                    \
    do {                                      \
        fprintf(stdout, fmt, ##__VA_ARGS__);  \
    } while(0); 

#define log_error(fmt, ...)                   \
    do {                                      \
        fprintf(stderr, fmt, ##__VA_ARGS__);  \
    } while(0); 

void set_pid(int pid);
void set_uuid(uint64_t log_id);
uint64_t get_uuid();                      

} // namespace clib 

#endif // _CLIB_LOG_H_

