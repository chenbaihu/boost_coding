#include "AsyncMapServiceHandler.h"
#include <stdio.h>

#ifdef DAsyncMapCli

AsyncMapServiceHandler::AsyncMapServiceHandler()
{
    // Your initialization goes here
}

void AsyncMapServiceHandler::compute(
            tcxx::function<void(ComputeResp const& _return)> cob, 
            tcxx::function<void(::apache::thrift::TDelayedException* _throw)> /* exn_cob */, 
            const ComputeReq& req)
{
    fprintf(stdout, 
            "req.cityId=%d\t"
            "creq.oidList.size=%ld\t"
            "creq.didList.size=%ld\n", 
            req.cityId, 
            req.oidList.size(), 
            req.didList.size()); 

    ComputeResp _return;
    _return.type = OrderType::COMMONORDER;
    _return.data = std::string("hello client");
    cob(_return);
    return;
}

#endif //#ifdef DAsyncMapCli

