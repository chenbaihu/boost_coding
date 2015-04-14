/*
 * 举例：
 * xxxx.proto
 *
 * package muduo;
 * message Query {
 *   required int64 id = 1;
 *   required string questioner = 2; 
 *   repeated string question = 3;
 * }
 *
 * typename的获取：
 *    typedef muduo::Query T;
 *    std::string type_name = T::descriptor()->full_name();               
 *    //序列化的一方取到full_name，传给反序列化一方的时候，将这个full_name传给对象。 
 *
 * 
 * protobuf反射功能：
 *    https://github.com/chenshuo/recipes/tree/master/protobuf
 * */

#ifndef __PROTOFACTORY_H
#define __PROTOFACTORY_H


#include <google/protobuf/stubs/common.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <string>

namespace util {

class MessageFactory
{
public:
    static ::google::protobuf::Message* CreateMessage(const std::string& type_name);
};

} // end of namespace util

#endif

