#include "RedisCliPool.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <hiredis.h>
#include <strings.h>
#include <assert.h>

#include "log.h"

void doTest()  
{  
    int timeout = 10000;  
    struct timeval tv;  
    tv.tv_sec = timeout / 1000;  
    tv.tv_usec = timeout * 1000;  

    //以带有超时的方式链接Redis服务器，同时获取与Redis连接的上下文对象。  
    //该对象将用于其后所有与Redis操作的函数。  
    redisContext* c = redisConnect((char*)"10.10.10.114", 6379);  
    if (c->err) {  
        redisFree(c);  
        return;  
    }  
    const char* command1 = "set stest1 value9";  
    redisReply* r = (redisReply*)redisCommand(c,command1);  
    //需要注意的是，如果返回的对象是NULL，则表示客户端和服务器之间出现严重错误，必须重新链接。  
    //这里只是举例说明，简便起见，后面的命令就不再做这样的判断了。  
    if (NULL == r) {  
        redisFree(c);  
        return;  
    }  

    //不同的Redis命令返回的数据类型不同，在获取之前需要先判断它的实际类型。  
    //至于各种命令的返回值信息，可以参考Redis的官方文档，或者查看该系列博客的前几篇  
    //有关Redis各种数据类型的博客。:)  
    //字符串类型的set命令的返回值的类型是REDIS_REPLY_STATUS，然后只有当返回信息是"OK"  
    //时，才表示该命令执行成功。后面的例子以此类推，就不再过多赘述了。  
    if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK") == 0)) {  
        printf("Failed to execute command[%s].\n",command1);  
        freeReplyObject(r);  
        redisFree(c);  
        return;  
    }  
    //由于后面重复使用该变量，所以需要提前释放，否则内存泄漏。  
    freeReplyObject(r);  
    printf("Succeed to execute command[%s].\n",command1);  

    const char* command2 = "strlen stest1";  
    r = (redisReply*)redisCommand(c,command2);  
    if (r->type != REDIS_REPLY_INTEGER) {  
        printf("Failed to execute command[%s].\n",command2);  
        freeReplyObject(r);  
        redisFree(c);  
        return;  
    }  
    int length = r->integer;  
    freeReplyObject(r);  
    printf("The length of 'stest1' is %d.\n",length);  
    printf("Succeed to execute command[%s].\n",command2);  

    const char* command3 = "get stest1";  
    r = (redisReply*)redisCommand(c,command3);  
    if (r->type != REDIS_REPLY_STRING) {  
        printf("Failed to execute command[%s].\n",command3);  
        freeReplyObject(r);  
        redisFree(c);  
        return;  
    }  
    printf("The value of 'stest1' is %s.\n",r->str);  
    freeReplyObject(r);  
    printf("Succeed to execute command[%s].\n",command3);  

    const char* command4 = "get stest2";  
    r = (redisReply*)redisCommand(c,command4);  
    //这里需要先说明一下，由于stest2键并不存在，因此Redis会返回空结果，这里只是为了演示。  
    if (r->type != REDIS_REPLY_NIL) {  
        printf("Failed to execute command[%s].\n",command4);  
        freeReplyObject(r);  
        redisFree(c);  
        return;  
    }  
    freeReplyObject(r);  
    printf("Succeed to execute command[%s].\n",command4);  

    const char* command5 = "mget stest1 stest2";  
    r = (redisReply*)redisCommand(c,command5);  
    //不论stest2存在与否，Redis都会给出结果，只是第二个值为nil。  
    //由于有多个值返回，因为返回应答的类型是数组类型。  
    if (r->type != REDIS_REPLY_ARRAY) {  
        printf("Failed to execute command[%s].\n",command5);  
        freeReplyObject(r);  
        redisFree(c);  
        //r->elements表示子元素的数量，不管请求的key是否存在，该值都等于请求是键的数量。  
        assert(2 == r->elements);  
        return;  
    }  
    int i;  
    for (i = 0; i < r->elements; ++i) {  
        redisReply* childReply = r->element[i];  
        //之前已经介绍过，get命令返回的数据类型是string。  
        //对于不存在key的返回值，其类型为REDIS_REPLY_NIL。  
        if (childReply->type == REDIS_REPLY_STRING)  
            printf("The value is %s.\n",childReply->str);  
    }  
    //对于每一个子应答，无需使用者单独释放，只需释放最外部的redisReply即可。  
    freeReplyObject(r);  
    printf("Succeed to execute command[%s].\n",command5);  

    printf("Begin to test pipeline.\n");  
    //该命令只是将待发送的命令写入到上下文对象的输出缓冲区中，直到调用后面的  
    //redisGetReply命令才会批量将缓冲区中的命令写出到Redis服务器。这样可以  
    //有效的减少客户端与服务器之间的同步等候时间，以及网络IO引起的延迟。  
    //至于管线的具体性能优势，可以考虑该系列博客中的管线主题。  
    /* if (REDIS_OK != redisAppendCommand(c,command1) 
       || REDIS_OK != redisAppendCommand(c,command2) 
       || REDIS_OK != redisAppendCommand(c,command3) 
       || REDIS_OK != redisAppendCommand(c,command4) 
       || REDIS_OK != redisAppendCommand(c,command5)) { 
       redisFree(c); 
       return; 
       } 
       */  

    redisAppendCommand(c,command1);  
    redisAppendCommand(c,command2);  
    redisAppendCommand(c,command3);  
    redisAppendCommand(c,command4);  
    redisAppendCommand(c,command5);  
    redisReply* reply = NULL;  
    //对pipeline返回结果的处理方式，和前面代码的处理方式完全一直，这里就不再重复给出了。  
    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {  
        printf("Failed to execute command[%s] with Pipeline.\n",command1);  
        freeReplyObject(reply);  
        redisFree(c);  
    }  
    freeReplyObject(reply);  
    printf("Succeed to execute command[%s] with Pipeline.\n",command1);  

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {  
        printf("Failed to execute command[%s] with Pipeline.\n",command2);  
        freeReplyObject(reply);  
        redisFree(c);  
    }  
    freeReplyObject(reply);  
    printf("Succeed to execute command[%s] with Pipeline.\n",command2);  

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {  
        printf("Failed to execute command[%s] with Pipeline.\n",command3);  
        freeReplyObject(reply);  
        redisFree(c);  
    }  
    freeReplyObject(reply);  
    printf("Succeed to execute command[%s] with Pipeline.\n",command3);  

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {  
        printf("Failed to execute command[%s] with Pipeline.\n",command4);  
        freeReplyObject(reply);  
        redisFree(c);  
    }  
    freeReplyObject(reply);  
    printf("Succeed to execute command[%s] with Pipeline.\n",command4);  

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {  
        printf("Failed to execute command[%s] with Pipeline.\n",command5);  
        freeReplyObject(reply);  
        redisFree(c);  
    }  
    freeReplyObject(reply);  
    printf("Succeed to execute command[%s] with Pipeline.\n",command5);  
    //由于所有通过pipeline提交的命令结果均已为返回，如果此时继续调用redisGetReply，  
    //将会导致该函数阻塞并挂起当前线程，直到有新的通过管线提交的命令结果返回。  
    //最后不要忘记在退出前释放当前连接的上下文对象。  
    redisFree(c);  
    return;  
}

int main(int argc, char* argv[]) 
{
    std::vector<std::string> ips;
    ips.push_back("10.10.10.114");
    //ips.push_back("10.10.10.114");

    std::vector<uint32_t> ports;
    ports.push_back(6379);
    //ports.push_back(6379);

    int pool_size=5; 

    int conn_timeout_ms = 100;
    int send_timeout_ms = 100;
    int recv_timeout_ms = 100;

    util::RedisCliPoolPtr redis_cli_pool_ptr(new util::RedisCliPool);
    redis_cli_pool_ptr->Init(ips, ports, pool_size, conn_timeout_ms, send_timeout_ms, recv_timeout_ms);

    printf("HSet=====================================================\n");
    if (redis_cli_pool_ptr->HSet(std::string("htk"), std::string("htf"), std::string("htvvvvvvvvvvvvv"))<0) {
        log_error("redis_cli_pool_ptr->HSet failed");
        exit(0);
    }

    printf("HMset=====================================================\n");
    util::FieldValueMap fv_map;
    fv_map[std::string("f1")] = "fv111111";
    fv_map[std::string("f2")] = "fv2222222222222222";
    fv_map[std::string("f3")] = "fv3333";
    fv_map[std::string("f4")] = "fv4444444444";
    fv_map[std::string("f5")] = "fv5555555555555555555555555"; 
    if (!redis_cli_pool_ptr->HMSet(std::string("htk"), fv_map)) {
        log_error("redis_cli_pool_ptr->HMSet failed");
        exit(0);
    }

    printf("HGet=====================================================\n");
    std::string value;
    if (!redis_cli_pool_ptr->HGet(std::string("htk"), std::string("f1"), value)) {
        log_error("redis_cli_pool_ptr->HGet failed");
        exit(0);
    }
    printf("%s\n", value.c_str()); 

    printf("HGetALL=====================================================\n");
    util::FieldValueMap get_fv_map;

    get_fv_map.clear();
    if (!redis_cli_pool_ptr->HGetAll(std::string("htk"), get_fv_map)) {
        log_error("redis_cli_pool_ptr->HGetAll failed");
        exit(0);
    }
    util::FieldValueMap::const_iterator cit = get_fv_map.begin();
    for (; cit!=get_fv_map.end(); cit++) {  
        printf("%s<->%s\n", cit->first.c_str(), cit->second.c_str()); 
    }

    printf("HMGet=====================================================\n");
    util::FieldSet fileds;
    fileds.insert("f1");
    fileds.insert("f2");
    fileds.insert("f3");
    fileds.insert("f4");
    fileds.insert("f5");
    fileds.insert("f100000");
    fileds.insert("f111111");

    get_fv_map.clear();
    if (!redis_cli_pool_ptr->HMGet(std::string("htk"), fileds, get_fv_map)) {
        log_error("redis_cli_pool_ptr->HMGet failed");
        exit(0);
    }
    cit = get_fv_map.begin();
    for (; cit!=get_fv_map.end(); cit++) {  
        printf("%s<->%s\n", cit->first.c_str(), cit->second.c_str()); 
    }

    printf("HMDel=====================================================\n");
    if (redis_cli_pool_ptr->HMDel(std::string("htk"), fileds)<=0) {
        log_error("redis_cli_pool_ptr->HMDel failed");
        exit(0);
    }

    printf("HGetAll=====================================================\n");
    get_fv_map.clear();
    if (!redis_cli_pool_ptr->HGetAll(std::string("htk"), get_fv_map)) {
        log_error("redis_cli_pool_ptr->HGetAll failed");
        exit(0);
    }
    cit = get_fv_map.begin();
    for (; cit!=get_fv_map.end(); cit++) {  
        printf("%s<->%s\n", cit->first.c_str(), cit->second.c_str()); 
    }

    doTest();
    return 0; 
}
