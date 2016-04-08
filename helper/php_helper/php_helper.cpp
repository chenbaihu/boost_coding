#include "php_helper.h"

#include <json/json.h>
#include <stdlib.h> 
#include <iomanip>
#include <limits>

#include "string_helper.h"

namespace util {

int PhpHelper::Serialize(string *target,const Json::Value &source)
{
    stringstream ss;
    string tmp;
    if ( source.isString() ) {
        ss << source.asString().length();
        ss >> tmp;
        tmp = "s:" + tmp + ":\"" + source.asString() + "\";";
    }
    else if ( source.isInt() ) {
        ss << source.asInt();
        ss >> tmp;
        tmp = "i:" + tmp + ";";
    }
    else if ( source.isDouble() ) {
        ss << std::fixed << std::setprecision(std::numeric_limits<double>::digits10) << source.asDouble();
        ss >> tmp;
        tmp = "d:" + tmp + ";";
    }
    else if ( source.isBool() ) {
        if ( source.asBool() ) {
            tmp = "b:1;";
        }
        else{
            tmp = "b:0;";
        }
    }
    else if ( source.isUInt() ) {
        ss << source.asUInt();
        ss >> tmp;
        tmp = "i:" + tmp + ";";
    }
    else if ( source.isArray() ) {
        ss << source.size();
        ss >> tmp;
        tmp = "a:" + tmp + ":{";
        for( Json::Value::iterator it = source.begin() ; it != source.end() ; it++) {
            if (Serialize(&tmp, *it) != 0 ) {
                return -2;
            }
        }
        tmp = tmp + "}";
    }
    else if ( source.isObject() ) {
        ss << source.size();
        string lstr ;
        ss >> lstr;
        tmp = tmp + "a:" + lstr + ":{";
        vector<string> keys = source.getMemberNames();
        for( size_t i = 0; i < keys.size() ; i++ ) {
            ss.clear();
            string key_index = keys[i];
            ss << key_index.length();
            ss >> lstr;
            tmp = tmp + "s:" + lstr + ":\"" + key_index + "\";";
            if ( Serialize( &tmp, source[key_index]) != 0 ) {
                return -3;
            }
        }
        tmp = tmp + "}";
    }
    else {
        return -1;
    }
    target->append(tmp);
    return 0;
}

int PhpHelper::GetKey(string *target,const string &source)
{
    if (source[0] != 's') {
        return -1;
    }
    if ( source[1] != ':') {
        return -1;
    }
    string::size_type pos;
    pos = source.find(":", 2);
    if ( pos == string::npos ) {
        return -1;
    }
    if ( ( pos + 1 ) >= source.length() ) {
        return -1;
    }
    if ( source[pos + 1] != '"') {
        return -1;
    }
    if ( source[source.length() - 1 ] != '"') {
        return -1;
    }

    int count_key = atoi( source.substr(2, pos - 2 ).c_str());
    string str = source.substr(pos+2, source.length() - pos - 3 );
    if ( (int)str.length() != count_key ) {
        return -1;
    }
    *target = str;
    return 0;
}

int PhpHelper::GetValue(Json::Value *target,const string &source)
{
    if ( source[1] != ':') {
        return -1;
    }
    if (source[0] == 's') {
        string::size_type pos;
        pos = source.find(":", 2);
        if ( pos == string::npos ) {
            return -1;
        }
        if ( ( pos + 1 ) >= source.length() ) {
            return -1;
        }
        if ( source[pos + 1] != '"') {
            return -1;
        }
        if ( source[source.length() - 1 ] != '"') {
            return -1;
        }

        int count_key = atoi( source.substr(2, pos - 2 ).c_str());
        string str = source.substr(pos+2, source.length() - pos - 3 );
        if ( (int)str.length() != count_key ) {
            return -1;
        }
        *target = str;
    }
    else if ( source[0] == 'i') {
        string str = source.substr(2, source.length() - 2 );
        *target = atoi(str.c_str());
    }
    else if ( source[0] == 'd') {
        string str = source.substr(2, source.length() - 2 );
        *target = atof(str.c_str());
    }
    else if ( source[0] == 'b') {
        if ( source.length() != 3 ) {
            return -1;
        }
        if ( source[2] == '1') {
            *target = true;
        }
        else if ( source[2] == '0') {
            *target = false;
        }
        else{
            return -1;
        }
    }
    else {
        return -1;
    }

    return 0;
}

int PhpHelper::UnSerialize(Json::Value *target,const string &source)
{
    if ( source[0] != 'a') {
        return -1;
    }
    if ( source[1] != ':') {
        return -1;
    }
    if ( source[source.length() -1] != '}') {
        return -1;
    }
    string::size_type pos;
    pos = source.find(":", 2);
    if ( pos == string::npos ) {
        return -1;
    }
    if ( ( pos + 1 ) >= source.length() ) {
        return -1;
    }
    if ( source[pos + 1] != '{') {
        return -1;
    }
    int count_key = atoi( source.substr(2, pos - 2 ).c_str());
    string str = source.substr(pos+2, source.length() - pos - 3 );
    vector<string> items;
    util::StringHelper::Split(items, str, ";");
    items = util::StringHelper::Trim(items);
    if ( (int)items.size() != count_key * 2) {
        return -1;
    }
    for( size_t i = 0 ; i < items.size() - 1 ; i++ ) {
        //key
        string tmp = items[i];
        string key;
        if (GetKey(&key, tmp) != 0 ) {
            return -1;
        }

        i++;
        //value
        tmp = items[i];
        if ( GetValue( &((*target)[key]), tmp) != 0 ) {
            return -1;
        }
    }
    return 0;
}

} // namespace util

