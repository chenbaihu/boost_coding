#ifndef  __ssMemcached_H_
#define  __ssMemcached_H_

#include <libmemcached/memcached.h>
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

namespace util {
#define VALUELEN    150000
#define MAX_KEY_NUM 4000

//key
typedef std::string CKVKey;

//val
typedef std::string CKVVal;

//get
typedef std::vector<CKVKey>      MGetKeyType;
typedef std::map<CKVKey, CKVVal> MGetResultType; 

//set
struct CKVLine {
public:
    CKVKey   key;
    CKVVal   val;
    int32_t  ttl;
    uint32_t flag;
public:
    CKVLine(const CKVLine& s) {
        key  = s.key;
        val  = s.val;
        ttl  = s.ttl;
        flag = s.flag;
    }
    CKVLine& operator=(const CKVLine& s) {
        key  = s.key;
        val  = s.val;
        ttl  = s.ttl;
        flag = s.flag;
        return *this;
    }
};
typedef std::vector<CKVLine> MCKVLine;

class ssMemcached {
public:
		memcached_server_st *servers;
		memcached_st *memc;
		memcached_return rc;
		ssMemcached();
		int destory();

		int initServer(char *pServerInfo, 
                    int nPort, 
                    int connTimeOutMs=100, 
                    int sendTimeOutMs=200, 
                    int recvTimeOutMs=200);

        int multiget(MGetKeyType& keys, 
                    MGetResultType& kvs);

		int set(char *sKey, 
                    size_t nKeyLen, 
                    char *pValue, 
                    int pValueLen, 
                    int32_t nTime, 
                    uint32_t flag=0);
};

} // namespace util

#endif //#ifndef  __ssMemcached_H_
