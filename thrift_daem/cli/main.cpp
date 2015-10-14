#ifndef DAsyncMapCli
#include "MapCli.h" 
#else 
#include "AsyncMapCli.h" 
#include "event.h"
#endif
#include "map_types.h"
#include "event.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void ShowRsp(const ComputeResp& crsp)
{
    fprintf(stdout, "Compute succ, crsp.data=%s\n", crsp.data.c_str());
}

int main(int argc, char* argv[]) { 
    char ip[] = "127.0.0.1";

    ComputeReq creq;
    creq.type   = OrderType::COMMONORDER;
    creq.jobId  = 0;
    creq.cityId = 100;
    creq.mapsplitId = 0;
    for (int64_t i=0; i<1000; i++) { 
        creq.oidList.push_back(i);
        creq.didList.push_back(i*2);
    }

    #ifndef DAsyncMapCli
    ComputeResp crsp;
    MapCli mapCli(ip, 23456, 1000, 1000, 1000);
    if (!mapCli.Compute(creq, crsp)) {
        fprintf(stderr, "Compute failed\n");
        return 1;
    }
    fprintf(stdout, "Compute succ, crsp.data=%s\n", crsp.data.c_str());
    #else
    event_base* evbase = event_base_new();

    AsyncMapCli asyncMapCli(evbase, ip, 23456, 1000, 1000, 1000);
    asyncMapCli.Compute(creq, std::tr1::bind(&ShowRsp, std::tr1::placeholders::_1));

    event_base_dispatch(evbase);
    event_base_free(evbase);
    #endif
    return 0;
}
