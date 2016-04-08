#ifndef _UTIL_TIME_HELPER_H_
#define _UTIL_TIME_HELPER_H_

#include <string>
#include <stdint.h>
#include <vector>

#define CurrTimeMs() \
    (util::TimeHelper::timems())

#define TimeConsumeS(begs)    \
    (int)(util::TimeHelper::time()-begs)

#define TimeConsumeMs(begms)  \
    ((int)(util::TimeHelper::timems()-begms))

#define TimeConsumeUs(begus)  \
    (int)(util::TimeHelper::timeus()-begus) 

namespace util {

class TimeHelper 
{
public:
	//time
	static uint64_t time();   // in seconds, diff 1s
	static uint64_t timeus(); // in micro seconds, diff 1us
	static uint64_t timems();
	static double   timef();  // in seconds, diff 1us


private:
    TimeHelper();
    ~TimeHelper();
}; 

} // namespace util

#endif // #ifndef _UTIL_TIME_HELPER_H_
