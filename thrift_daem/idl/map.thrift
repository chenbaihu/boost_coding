exception ServiceException {                                                                                                                                                
    1: required string errorMsg
}

enum OrderType{
    COMMONORDER,
    SHAREDORDER,
}

# compute接口输入参数
struct ComputeReq{
    1: required byte       type,
    2: required i64       jobId,
    3: required i32       cityId,
    4: required i32       mapsplitId,
    5: required list<i64> oidList,
    6: required list<i64> didList,
}

# compute接口输出参数
struct ComputeResp{
    1: required byte   version,
    2: required i32    type,
    3: required byte   status,
    4: required i32    mapsplitId,
    7: required string data,
}

service MapService{
    ComputeResp  compute(1:ComputeReq req)throws (1: ServiceException se),
}

