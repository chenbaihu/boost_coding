#ifndef _ASYNC_MAP_SERVICE_H_
#define _ASYNC_MAP_SERVICE_H_

#include "MapService.h"

#include <string>
#include <map>
using std::string;
using std::map;

#ifdef DAsyncMapCli

class AsyncMapServiceHandler : virtual public MapServiceCobSvIf 
{
 public:
     AsyncMapServiceHandler();   
     virtual void compute(tcxx::function<void(ComputeResp const& _return)> cob, tcxx::function<void(::apache::thrift::TDelayedException* _throw)> /* exn_cob */, const ComputeReq& req);
};

#endif //#ifdef DAsyncMapCli

#endif //_MAP_SERVICE_H_
