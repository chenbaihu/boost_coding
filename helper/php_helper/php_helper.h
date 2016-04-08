#ifndef _UTIL_PHP_HELPER_H_
#define _UTIL_PHP_HELPER_H_

#include <sstream>

namespace Json { 
    class Value;
} // namespace Json

namespace util {

using namespace std;

class PhpHelper {
public:
    static int Serialize(string *target,const Json::Value &source);
    static int UnSerialize(Json::Value *target,const string &source);

private:
    static int GetKey(string *target,const string &source);
    static int GetValue(Json::Value *target,const string &source);
};

} // namespace util

#endif // _UTIL_PHP_HELPER_H_

