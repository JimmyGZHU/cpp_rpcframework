/*************************************************************************
    > File Name: mprpcchannel.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2021年05月18日 星期二 14时37分14秒
 ************************************************************************/

#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


class MprpcChannel : public google::protobuf::RpcChannel {
public:
    // 所有通过stub代理对象调用的rpc方法, 都会走到这里
    // 统一做rpc方法的数据序列化和网络传送
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done); 
};
