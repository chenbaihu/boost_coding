#include "string_helper.h"

namespace util {

std::string StringHelper::ReplaceAll(const std::string &strBig, const std::string & strsrc, const std::string &strdst)
{
    std::string result = strBig;
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();
    while( (pos = result.find(strsrc, pos)) != std::string::npos){
        result.replace(pos, srclen, strdst);
        pos += dstlen;
    }
    return result;
}

void StringHelper::Split(std::vector<std::string>& strvct, const std::string & str, const std::string & splitor)
{
    std::string::size_type pos = 0;
    strvct.clear();
    while( true) {
        std::string::size_type tmp = str.find(splitor, pos);
        if( tmp == std::string::npos) {
            if( !str.substr(pos).empty() ) {
                strvct.push_back(str.substr(pos));
            }
            break;
        }
        else {
            strvct.push_back(str.substr(pos, tmp-pos));
            pos = tmp + splitor.length();
        }
    }
}
std::string StringHelper::TrimLeft(const std::string &str)
{
    std::string::size_type pos = 0;

    while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t'
                || str[pos] == '\r' || str[pos] == '\n')) {
        ++pos;
    }
    return str.substr(pos);
}

std::string StringHelper::TrimRight(const std::string & str)
{
    std::string::size_type pos = str.length() - 1;

    while (pos != std::string::npos && (str[pos] == ' ' || str[pos] == '\t'
                || str[pos] == '\r' || str[pos] == '\n')) {
        --pos;
    }

    if (pos == std::string::npos) {
        return "";
    } else {
        return str.substr(0, std::min(pos + 1, str.length()));
    }
}

std::string StringHelper::Trim(const std::string & str)
{
    return TrimLeft(TrimRight(str));
}

std::vector<std::string> StringHelper::Trim(const std::vector<std::string> &strvct)
{
    std::vector<std::string> result;
    for(std::vector<std::string>::const_iterator itr = strvct.begin(); 
                itr != strvct.end(); ++itr ) {
        std::string value = Trim(*itr);
        if (!value.empty()) {
            result.push_back(value);
        }
    }
    return result;
} 

int StringHelper::ParseParameter(const char *sParaStr/*a=b&c=d&e=f&g=h*/, 
            int iStrLen, 
            std::map<std::string, std::string>& params, 
            char ch1/*'='*/, 
            char ch2/*'&'*/)
{
    params.clear();
    int i;
    char sBuffer[iStrLen];
    char ch, chFlag;
    int iCounter;
    int iParaNum = 0;      //参数对的数目
    int iHavedGetName = 0;

    if (iStrLen==0 || sParaStr==NULL) {
        return 0;
    }

    iCounter = 0;  //sBuffer当前写入位置
    chFlag = 0;     //0 表示正在匹配ch1, 1表示已经匹配ch1
    std::string strName;
    std::string strValue;

    for (i=0; i<iStrLen; i++) {
        ch = sParaStr[i];

        if (ch1 == ch && iHavedGetName == 0) {
            if (iCounter >= (int)sizeof(sBuffer)) {
              sBuffer[sizeof(sBuffer) -1] = '\0';
            } else {
              sBuffer[iCounter] = '\0';
            }

            strName = sBuffer;
            iCounter = 0;
            chFlag = 1;
            iHavedGetName = 1;
        } else if (ch2==ch || 0==ch) {
            if (chFlag != 1) {
              return -2;
            }

            if (iCounter >= (int)sizeof(sBuffer)) {
              sBuffer[sizeof(sBuffer) -1] = '\0';
            } else {
              sBuffer[iCounter] = '\0';
            }

            strValue = sBuffer;
            params[strName] = strValue;
            iCounter = 0;

            iParaNum++;
            chFlag = 0;
            iHavedGetName = 0;
        } else {
            sBuffer[iCounter++] = ch;
        }
    }

    if (iHavedGetName==1 && chFlag==1) {
        if (iCounter>=(int)sizeof(sBuffer)) {
          sBuffer[sizeof(sBuffer) -1] = '\0';
        } else {
          sBuffer[iCounter] = '\0';
        }

        strValue        = sBuffer;
        params[strName] = strValue;
        ++iParaNum;
    }
    return iParaNum;
}

} // namespace util

