#ifndef __UDP_REQ_H_
#define __UDP_REQ_H_

#include <string>
#include <glog/logging.h>

DECLARE_int32(data_len);

void echo(std::string& req, std::string& exp) 
{ /*{{{*/
    std::string req_str = "";
    for(size_t i=0; i<(size_t)FLAGS_data_len; i++) {
        req_str.append("a");
    }
    req = req_str;
    exp = req_str;
} /**/

void get_udp_req_and_exp(std::string& req, std::string& exp)
{ /*{{{*/
    echo(req, exp);
} /*}}}*/

#endif


