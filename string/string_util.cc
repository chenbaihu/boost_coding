#ifndef __STRING_UTIL_H
#define __STRING_UTIL_H

#include <string>

class StringUtil {
public:
    static bool StringReplace(std::string& src, const char* sc, const char* dc, size_t slice_count=0);
    static bool StringSplit(const std::string& src, const char* delims, std::vector<std::string> result_vec, size_t slice_count=0);
    //static bool StringInIParse(const std::string& src, const char* delims, std::vector<std::string> result_vec, size_t slice_count=0);
}

#endif


bool StringUtil::StringReplace(std::string& src, const char* sc, const char* dc, size_t slice_count/*=0*/) 
{
}

bool StringUtil::StringSplit(const std::string& src, const char* delims, std::vector<std::string> result_vec, size_t slice_count/*=0*/) 
{
}

bool StringUtil::StringSplit


#ifdef _TEST_


#endif
