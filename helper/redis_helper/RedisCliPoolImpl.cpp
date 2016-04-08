#include "RedisCliPoolImpl.h" 

#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <strings.h>

#include "RedisConnPool.h"
#include "log.h"
#include "base64.h"

namespace util {

std::string DumpFieldValueStr(const FieldValueMap& field_value_map)
{
    std::stringstream dump_oss; 

    FieldValueMap::const_iterator cit = field_value_map.begin();
    for (; cit!=field_value_map.end(); cit++) { 
        dump_oss << "[" << cit->first << "|" << cit->second.length() << "|" << cit->second << "]\t";
    }
    return dump_oss.str();
}

std::string DumpSetStr(const std::set<std::string/*field*/> field_set) 
{
    std::stringstream dump_oss;

    std::set<std::string/*field*/>::const_iterator cit = field_set.begin();
    for (; cit!=field_set.end(); cit++) {
        dump_oss << *cit << "|";
    }
    return dump_oss.str(); 
}

RedisCliPoolImpl::RedisCliPoolImpl()
{
}

RedisCliPoolImpl::~RedisCliPoolImpl() 
{
}

bool RedisCliPoolImpl::Init(std::vector<std::string> ips, 
            std::vector<uint32_t> ports, 
            int pool_size/*=1024*/, 
            int conn_timeout_ms/*=100*/, 
            int send_timeout_ms/*=100*/, 
            int recv_timeout_ms/*=100*/)
{
    srandom(time(NULL)); 
    
    for (size_t port_i=0; port_i<ports.size(); port_i++) {  
        for (size_t ip_i=0; ip_i<ips.size(); ip_i++) { 
            RedisConnPoolPtr redis_conn_pool_ptr(new RedisConnPool(ips[ip_i], 
                            (int)ports[port_i], 
                            pool_size, 
                            conn_timeout_ms,
                            send_timeout_ms, 
                            recv_timeout_ms));
            redis_conn_pool_ptr_vec_.push_back(redis_conn_pool_ptr);
        }
    }

    if (redis_conn_pool_ptr_vec_.empty()) { 
        log_error("redis_conn_pool_ptr_vec_.empty"); 
        return false;
    }
    return true;
}

int RedisCliPoolImpl::HSetImpl(const std::string& key, 
        const std::string& field_name, 
        const std::string& field_value, 
        bool nx/*=false*/)
{
    RedisConnect redis_conn = BorrowRedisConn();
    if (redis_conn.redis_ctx==NULL) {
        log_error("key=%s value_b64=%s redis_conn.redis_ctx==NULL",
            key.c_str(),
            util::Base64::encode(field_value).c_str());
        return -1;
    }
    redisContext* redis_ctx = redis_conn.redis_ctx; 

    redisReply *reply = NULL;  
    std::stringstream str_cmd; 
    if (nx) { 
        reply = (redisReply*)redisCommand(redis_ctx, "HSETNX %s %s %b", 
                key.c_str(), 
                field_name.c_str(), 
                field_value.c_str(), 
                field_value.length());
    } else { 
        reply = (redisReply*)redisCommand(redis_ctx, "HSET %s %s %b", 
                key.c_str(), 
                field_name.c_str(), 
                field_value.c_str(), 
                field_value.length());
    } 

    if (redis_ctx->err==0 && reply!=NULL && reply->type==REDIS_REPLY_INTEGER) { 
        long long line = reply->integer; 
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn);
        return (int)line;   
    }

    if (redis_ctx->err!=0) {
        log_error("key=%s value_b64=%s redis_ctx->errstr=%s", 
                    key.c_str(),
                    util::Base64::encode(field_value).c_str(),
                    redis_ctx->errstr);
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn);
        return -1;
    }

    if (reply==NULL) { 
        ReturnRedisConn(redis_conn);
        log_error("kye=%s value_b64=%s reply==NULL", 
                key.c_str(), 
                util::Base64::encode(field_value).c_str());
        return -1;
    } 

    if (reply->type==REDIS_REPLY_ERROR) {  
        log_error("key=%s value_b64=%s err=%s", 
                key.c_str(),
                util::Base64::encode(field_value).c_str(), 
                (reply->str==NULL)?"NULL":reply->str);
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn); 
        return -1;
    }

    log_error("key=%s value_b64=%s Unknow Error reply->type=%d",
            key.c_str(), 
            util::Base64::encode(field_value).c_str(), 
            reply->type);
    freeReplyObject(reply);
    ReturnRedisConn(redis_conn); 
    return -1;
}

bool RedisCliPoolImpl::HMSetImpl(const std::string& key, const FieldValueMap& values)
{
    RedisConnect redis_conn = BorrowRedisConn();
    if (redis_conn.redis_ctx==NULL) {
        log_error("key=%s values=%s redis_conn.redis_ctx==NULL",
                SafeStr(key.c_str()), 
                SafeStr(DumpFieldValueStr(values).c_str()));
        return false;
    }
    redisContext* redis_ctx = redis_conn.redis_ctx; 

    int valid_cmd_num = 0;
    int succ_cmd_num  = 0;

    FieldValueMap::const_iterator cit = values.begin();
    for (; cit!=values.end(); cit++) { 
        if (cit->first.length()==0 || cit->second.length()==0) {
            #ifdef _DEBUG
            log_warn("HMSetImpl cit->first=%s cit->second=%s", 
                    SafeStr(cit->first.c_str()), 
                    SafeStr(cit->second.c_str()));
            #endif
            continue;
        }
        int ret = redisAppendCommand(redis_ctx, "HMSET %s %s %b", 
                key.c_str(), 
                cit->first.c_str(), 
                cit->second.c_str(), 
                cit->second.length());
        if (ret!=REDIS_OK) { 
            log_error("redisAppendCommand key=%s filed=%s value_b64=%s failed",
                    key.c_str(), 
                    cit->first.c_str(), 
                    SafeStr(util::Base64::encode(cit->second).c_str()));
            continue;
        }
        valid_cmd_num++;
    }

    #ifdef _DEBUG
    log_notice("valid_cmd_num=%d", valid_cmd_num);
    #endif

    redisReply* reply = NULL;
    for (int ci=0; ci<valid_cmd_num; ci++) { 
        int ret = redisGetReply(redis_ctx,(void**)&reply); 
        if (REDIS_OK == ret) {
            succ_cmd_num++;
            freeReplyObject(reply);
            continue;
        }
        log_error("Failed to execute ci=%d with Pipeline", ci);

        if (redis_ctx->err!=0) { 
            log_error("redis_ctx->errstr=%s", redis_ctx->errstr);
            freeReplyObject(reply);
            ReturnRedisConn(redis_conn); 
            return false;
        }
     
        if (reply==NULL) {
            log_error("reply==NULL");
            continue;
        } 

        if (reply->type==REDIS_REPLY_ERROR) {  
            log_error("err=%s", SafeStr(reply->str));
            freeReplyObject(reply);
            continue;
        }
        log_error("ci=%d Unknwo error", ci);
        freeReplyObject(reply);
    }
    #ifdef _DEBUG
    log_notice("valid_cmd_num=%d\tsucc_cmd_num=%d", 
            valid_cmd_num, 
            succ_cmd_num);
    #endif 
    ReturnRedisConn(redis_conn);  
    return true;
}

int RedisCliPoolImpl::HMDelImpl(const std::string& str_cmd)
{
    RedisConnect redis_conn = BorrowRedisConn();
    if (redis_conn.redis_ctx==NULL) {
        log_error("str_cmd=%s redis_conn.redis_ctx==NULL", str_cmd.c_str());
        return -1;
    }
    redisContext* redis_ctx = redis_conn.redis_ctx;
 
    redisReply *reply = (redisReply*)redisCommand(redis_ctx, str_cmd.c_str()); 
    if (redis_ctx->err==0 && reply!=NULL && reply->type==REDIS_REPLY_INTEGER) {
        long long line = reply->integer; 
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn);
        return (int)line;   
    }

    if (redis_ctx->err!=0) { 
        log_error("str_cmd=%s redis_ctx->errstr=%s", 
                    str_cmd.c_str(), 
                    redis_ctx->errstr);
        if (reply!=NULL) {
            freeReplyObject(reply);
        }
        ReturnRedisConn(redis_conn);
        return -1;
    }

    if (reply==NULL) { 
        log_error("str_cmd=%s reply==NULL", str_cmd.c_str());
        ReturnRedisConn(redis_conn);
        return -1;
    } 

    if (reply->type==REDIS_REPLY_ERROR) {  
        log_error("str_cmd=%s err=%s", str_cmd.c_str(), (reply->str==NULL)?"NULL":reply->str);
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn); 
        return false;
    }

    log_error("Unknow Error reply->type=%d", reply->type);
    freeReplyObject(reply);
    ReturnRedisConn(redis_conn); 
    return -1;
}

bool RedisCliPoolImpl::HGetImpl(const std::string& str_cmd, std::string& value)
{
    RedisConnect redis_conn = BorrowRedisConn();
    if (redis_conn.redis_ctx==NULL) {
        log_error("str_cmd=%s redis_conn.redis_ctx==NULL", str_cmd.c_str());
        return false;
    }
    redisContext* redis_ctx = redis_conn.redis_ctx;
 
    redisReply *reply = (redisReply*)redisCommand(redis_ctx, str_cmd.c_str()); 
    if (redis_ctx->err==0 && reply!=NULL && reply->type!=REDIS_REPLY_ERROR) {
        if (reply->type==REDIS_REPLY_STRING && reply->str!=NULL) { // 获取到内容
            value = std::string(reply->str, reply->len);
            freeReplyObject(reply);
            ReturnRedisConn(redis_conn);
            return true;
        }
        if (reply->type==REDIS_REPLY_NIL) { // 不存在
            value.clear();
            freeReplyObject(reply);
            ReturnRedisConn(redis_conn);
            return true;
        }

        log_error("str_cmd=%s reply->type=%d", 
                str_cmd.c_str(), 
                reply->type);
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn);
        return true;
    }

    if (redis_ctx->err!=0) { 
        log_error("str_cmd=%s redis_ctx->errstr=%s", 
                    str_cmd.c_str(), 
                    redis_ctx->errstr);
        if (reply!=NULL) { 
            freeReplyObject(reply);
        }
        ReturnRedisConn(redis_conn);
        return false;
    }

    if (reply==NULL) { 
        log_error("str_cmd=%s reply==NULL", str_cmd.c_str());
        ReturnRedisConn(redis_conn);
        return false;
    } 

    if (reply->type==REDIS_REPLY_ERROR) {  
        log_error("str_cmd=%s err=%s", str_cmd.c_str(), (reply->str==NULL)?"NULL":reply->str);
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn); 
        return false;
    }
    log_error("Unknow Error reply->type=%d", reply->type);
    freeReplyObject(reply);
    ReturnRedisConn(redis_conn); 
    return false;
}

bool RedisCliPoolImpl::HMGetImpl(const std::string& str_cmd, const FieldSet& fields, FieldValueMap& values)
{
    RedisConnect redis_conn = BorrowRedisConn();
    if (redis_conn.redis_ctx==NULL) {
        log_error("str_cmd=%s redis_conn.redis_ctx==NULL", str_cmd.c_str());
        return false;
    }
    redisContext* redis_ctx = redis_conn.redis_ctx;
 
    redisReply *reply = (redisReply*)redisCommand(redis_ctx, str_cmd.c_str()); 
    if (redis_ctx->err==0 && reply!=NULL && reply->type==REDIS_REPLY_ARRAY) { //获取到内容
        #ifdef _DEBUG
        log_notice("elements=%d", reply->elements);
        #endif
        GetReplyValues(reply, fields, values); 
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn);
        return true;
    } 

    if (redis_ctx->err==0 && reply!=NULL && reply->type==REDIS_REPLY_NIL) {  //key不存在
        values.clear();
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn);
        return true;
    }

    if (redis_ctx->err!=0) { 
        log_error("str_cmd=%s redis_ctx->errstr=%s", 
                    str_cmd.c_str(), 
                    redis_ctx->errstr); 
        if (reply!=NULL) {
            freeReplyObject(reply);
        }
        ReturnRedisConn(redis_conn);
        return false;
    }

    if (reply==NULL) { 
        ReturnRedisConn(redis_conn);
        log_error("str_cmd=%s reply==NULL", str_cmd.c_str());
        return false;
    } 

    if (reply->type==REDIS_REPLY_ERROR) {  
        log_error("str_cmd=%s err=%s", str_cmd.c_str(), (reply->str==NULL)?"NULL":reply->str);
        freeReplyObject(reply);
        ReturnRedisConn(redis_conn); 
        return false;
    }

    log_error("Unknow Error reply->type=%d", reply->type);
    freeReplyObject(reply);
    ReturnRedisConn(redis_conn); 
    return true;
}

void RedisCliPoolImpl::GetReplyValues(redisReply *reply, const FieldSet& fields, FieldValueMap& values) 
{
    if (reply==NULL) { 
        return;
    }

    #ifdef _DEBUG
    log_notice("fields.size=%ld\treply->elements=%d", 
            fields.size(), 
            reply->elements);
    #endif

    //HGETALL
    if (fields.empty()) { 
        for (size_t i=0; i<reply->elements; i++) { 
            redisReply* rpf = reply->element[i];
            redisReply* rpv = reply->element[i+1];
            i++; 
            if (rpf!=NULL && rpv!=NULL) { 
                values[std::string(rpf->str, rpf->len)] = std::string(rpv->str, rpv->len);
            } 
        }
        #ifdef _DEBUG
        log_notice("values.size=%ld", values.size());
        #endif
        return;
    } 

    //HMGET
    FieldSet::const_iterator cit = fields.begin(); 
    for (size_t i=0; i<reply->elements; i++) {
        if (cit==fields.end()) {
            break;
        }

        redisReply* rp = reply->element[i]; 
        if (rp==NULL || rp->type==REDIS_REPLY_NIL) { //不存在的field
            cit++;
            continue;
        }
        
        //log_notice("+++++++++i=%d rp->type=%d", i, rp->type);

        if (rp->type==REDIS_REPLY_STRING) {    
            values[*cit] = std::string(rp->str, rp->len);
            //log_notice("-------------i=%d *cit=%s rp->str=%s", i, cit->c_str(), rp->str);
            cit++;
        }
    }
    #ifdef _DEBUG
    log_notice("values.size=%ld", values.size());
    #endif 
    return;
}

bool RedisCliPoolImpl::HKeysImpl(const std::string& str_cmd, FieldSet& fields)
{
    //TODO...
    return true;
}

bool RedisCliPoolImpl::HValsImpl(const std::string& str_cmd, ValueSet& values)
{
    //TODO...
    return true;
}

RedisConnect RedisCliPoolImpl::BorrowRedisConn() 
{
    RedisConnect redis_conn;
    if (redis_conn_pool_ptr_vec_.empty()) { 
        log_error("redis_conn_pool_ptr_vec_.empty");
        return redis_conn;
    }

    int index =  (int)rand()%redis_conn_pool_ptr_vec_.size();
    RedisConnPoolPtr redis_conn_pool_ptr = redis_conn_pool_ptr_vec_[index];
    if (redis_conn_pool_ptr==NULL) {
        log_error("redis_conn_pool_ptr==NULL");
        return redis_conn;
    }

    redis_conn.redis_conn_pool_ptr = redis_conn_pool_ptr;
    redis_conn.redis_ctx           = redis_conn_pool_ptr->BorrowRedisConn();
    if(redis_conn.redis_ctx!=NULL) {
        return redis_conn;
    } 
    log_error("redis_conn.redis_ctx== NULL");
       
    // 重试获取连接
    redis_conn_pool_ptr->ReturnRedisConn(redis_conn.redis_ctx);     
    redis_conn.redis_ctx = redis_conn_pool_ptr->BorrowRedisConn();
    return redis_conn;

    //if(redis_conn.redis_ctx == NULL) {
    //    log_error("redis_conn.redis_ctx== NULL retry");
    //    return redis_conn;
    //}
}

void RedisCliPoolImpl::ReturnRedisConn(RedisConnect redis_conn) 
{
    if (redis_conn.redis_conn_pool_ptr==NULL) { 
        return;
    }
    redis_conn.redis_conn_pool_ptr->ReturnRedisConn(redis_conn.redis_ctx);
}

} // namespace util
