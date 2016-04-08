#ifndef _UTIL_STRING_HELPER_
#define _UTIL_STRING_HELPER_

#include <vector>
#include <string>
#include <map>

#define GETSTR(str) ((str==NULL)?"NULL":(str))

namespace util {

class StringHelper {
public:
    static std::string ReplaceAll(const std::string &strBig, const std::string & strsrc, const std::string &strdst);
    static void Split(std::vector<std::string>& strvct, const std::string & str, const std::string & splitor = ";");
	static std::string TrimLeft(const std::string &str);
	static std::string TrimRight(const std::string & str);
	static std::string Trim(const std::string & str);
    static std::vector<std::string> Trim(const std::vector<std::string> &strvct);

    static int ParseParameter(const char *sParaStr, 
            int iStrLen, 
            std::map<std::string, std::string>& params, 
            char ch1/*'='*/, 
            char ch2/*'&'*/);
};

} // namespace util 

#endif // _UTIL_STRING_HELPER_

