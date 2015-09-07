#include "MapCli.h" 
#include "map_types.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) { 
    char ip[] = "127.0.0.1";
    MapCli mapCli(ip, 23456, 1000, 1000, 1000);

    ComputeReq creq;
    creq.type   = OrderType::COMMONORDER;
    creq.jobId  = 0;
    creq.cityId = 100;
    creq.mapsplitId = 0;
    for (int64_t i=0; i<1000; i++) { 
        creq.oidList.push_back(i);
        creq.didList.push_back(i*2);
    }

    ComputeResp crsp;
    if (!mapCli.Compute(creq, crsp)) {
        fprintf(stderr, "Compute failed\n");
        return 1;
    }
    fprintf(stdout, "Compute succ, crsp.data=%s\n", crsp.data.c_str());
    return 0;
}
