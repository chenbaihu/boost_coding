#include "ssMemcached.h"
#include "log.h"

namespace util {
#define nValueLen 200000 // 200k

ssMemcached::ssMemcached() {
	servers = NULL;
	memc= memcached_create(NULL);
}

//destory memcache
int ssMemcached::destory() {
	memcached_free(memc);
    memcached_server_free(servers);
    return 0;
}

//init Server
int ssMemcached::initServer(char *pServerInfo, 
            int nPort, 
            int connTimeOutMs/*=100*/, 
            int sendTimeOutMs/*=200*/, 
            int recvTimeOutMs/*=200*/) {
  memcached_return rc;
  servers	= memcached_server_list_append(servers, pServerInfo, nPort, &rc); 
  rc		= memcached_server_push(memc, servers);
  if (rc != MEMCACHED_SUCCESS) {
    log_error("Couldn't add server: %s",memcached_strerror(memc, rc));
	return -1;
  }
  //log_notice("pServerInfo=%s\tnPort=%d", pServerInfo, nPort);

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK,        1);   //参数MEMCACHED_BEHAVIOR_NO_BLOCK为1使超时配置生效，
                                                                         //不设置超时会不生效，关键时候会悲剧的，容易引起雪崩  
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, connTimeOutMs);  //connect timeout
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RCV_TIMEOUT,     sendTimeOutMs);  //read timeout
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SND_TIMEOUT,     recvTimeOutMs);  //write timeout
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT,    recvTimeOutMs);
  return 0;
}

int ssMemcached::multiget(MGetKeyType& keys, MGetResultType& kvs)
{
    //req 
    char*   sKeySet[MAX_KEY_NUM];
    size_t  nKeySetLen[MAX_KEY_NUM];
    int     nKeySetNum = keys.size();

    //rsp
    char     return_key[MEMCACHED_MAX_KEY];
    size_t   return_key_length   = 0;
    char*    return_value        = NULL;
    size_t   return_value_length = 0;	
    uint32_t flags               = 0; 

    //init
    for (size_t ki=0; ki<keys.size(); ki++) { 
        sKeySet[ki]    = &(keys[ki][0]); 
        nKeySetLen[ki] = (int)keys[ki].size();
    }
    return_key[0] = 0x0;

    memcached_return rc;
    rc= memcached_mget(memc, sKeySet, nKeySetLen, nKeySetNum);
    if (rc != MEMCACHED_SUCCESS) {
        log_error("Couldn't multi key: %s", memcached_strerror(memc, rc));
        return -1; 
    }

    int index = 0;
    while ((return_value = memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &flags, &rc)) != NULL) {
        if (rc != MEMCACHED_SUCCESS) {
            log_error("memcached_fetch failed %s",memcached_strerror(memc, rc));
            return -1;
        }
        return_key[return_key_length] = '\0'; 
        //log_notice("index=%d, return_key=%s, return_value=%s", index, return_key, return_value);

        int i = 0;
        for(i=0; i<nKeySetNum; i++) {
            index = index % nKeySetNum;
            if(strcmp(sKeySet[index], return_key) == 0 ) {
                break;
            } else {
                index = (index+1) % nKeySetNum;
            }
        }

        if (i<nKeySetNum) {
            if(return_value_length>=nValueLen) {
                kvs[keys[index]] = std::string(return_value, nValueLen-1);
            } else {
                kvs[keys[index]] = std::string(return_value, return_value_length);
            }
            return_key[0] = 0x0;
            free(return_value);
            return_value = NULL;
            index++;
        } else {
            log_error("i=%d, nKeySetNum=%d, return_key=%s, return_value=%s", i, nKeySetNum, return_key, return_value);
            return_key[0] = 0x0;
            free(return_value);
            return_value = NULL;
            return -1;
        }    
    } 
    return 0;
}

int ssMemcached::set(char *sKey, 
            size_t nKeyLen, 
            char *pValue, 
            int pValueLen, 
            int32_t nTime, 
            uint32_t flag/*=0*/) 
{
    memcached_return rc;
    rc = memcached_set(memc, sKey, nKeyLen, pValue, pValueLen, nTime, flag);
    if (rc!=MEMCACHED_SUCCESS) {
        log_error("Couldn't store key: %s",memcached_strerror(memc, rc));
		return -1;
    }
    return 0;
}

} // namesapce util

