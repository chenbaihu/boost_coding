#ifndef _ASYNC_MAP_CLI_H_
#define _ASYNC_MAP_CLI_H_ 

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <tr1/functional>
#include <tr1/memory>
#include <sstream>
#include <string>
#include <vector>
using std::string;

#include <stdint.h>

#ifdef DAsyncMapCli
#include "EventThread.h"

struct event_base;
class AsyncMapCli {
public:
public:
    AsyncMapCli(clib::EventThreadPtr _eventThreadPtr, 
            char* _ip, uint16_t _port, 
            uint16_t _conn_timeout=100/*ms*/, 
            uint16_t _send_timeout=100/*ms*/, 
            uint16_t _recv_timeout=100/*ms*/); 
    ~AsyncMapCli();

public:
    bool Init();
    bool AddTask(clib::EventThread::TaskPtr task);

public:
    std::string get_cliInfo() { return cliInfo.str(); }
    ::event_base* get_event_base() { return eventThreadPtr->event_base(); } 

public:
    clib::EventThreadPtr      eventThreadPtr;
    
    std::string         ip;
    uint16_t            port;
    uint16_t            conn_timeout;
    uint16_t            send_timeout;
    uint16_t            recv_timeout;
    
    std::stringstream   cliInfo;    
}; 
typedef std::tr1::shared_ptr<AsyncMapCli> AsyncMapCliPtr; 
typedef std::vector<AsyncMapCliPtr>       AsyncMapCliPtrVec;

#endif //#ifdef DAsyncMapCli

#endif 


