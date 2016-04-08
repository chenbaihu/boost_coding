#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <string>

#include "json/json.h"

#include "php_helper.h"

#define SAFE_STR(str) (str==NULL?"NULL":str)

int main(int argc, char* argv[])
{
    //序列化
    Json::Value json_coordinate;
    json_coordinate["lng"]  = (double)120.06;
    json_coordinate["lat"]  = (double)37.08;
    json_coordinate["time"] = static_cast<int32_t>(time(NULL)); 

    std::string str_coordinate;
    if (util::PhpHelper::Serialize(&str_coordinate, json_coordinate) != 0) {
        fprintf(stderr, "util::PhpHelper::Serialize json value:%s failed\n", 
                    Json::FastWriter().write(json_coordinate).c_str());
        exit(0);
    }

    fprintf(stdout, "str_coordinate=%s\n", str_coordinate.c_str());

    //反序列化
    Json::Value coordinate;
    if (util::PhpHelper::UnSerialize(&coordinate, str_coordinate) != 0) { 
        fprintf(stderr, "util::PhpHelper::UnSerialize str_coordinate=%s failed\n", 
                    SAFE_STR(str_coordinate.c_str()));
        return false;
    }

    double  lng  = 0.0;
    double  lat  = 0.0;
    int32_t time = 0;

    if (coordinate.isMember("lng") && coordinate["lng"].isDouble()) { 
        lng = coordinate["lng"].asDouble();
    }

    if (coordinate.isMember("lat") && coordinate["lat"].isDouble()) { 
        lat = coordinate["lat"].asDouble();
    }

    if (coordinate.isMember("time") && coordinate["time"].isInt()) { 
        time = coordinate["time"].asInt();
    }

    fprintf(stdout, "lng=%f lat=%f time=%d\n", lng, lat, time); 
    return 0;
}

