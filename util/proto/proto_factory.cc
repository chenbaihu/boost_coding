#include "proto_factory.h"

//include 已知的proto文件信息
//#include "comm.pb.h"
//#include "request_get.pb.h"
//#include "request_set.pb.h"
//#include "respnse_get.pb.h"
//#include "respnse_set.pb.h"

::google::protobuf::Message* MessageFactory::CreateMessage(const string& type_name)
{
    ::google::protobuf::Message* message = NULL;

    //if(type_name == "message1_type") { 
    //    return new message1_type; 
    //}

    //if (type_name =="message2_type") {
    //    return  new message2_type;
    //}
    
    //通用的获取方法 
    const ::google::protobuf::Descriptor* descriptor = ::google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if (descriptor) {
        const ::google::protobuf::Message* prototype = ::google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}

