#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "logger.h"

int main(int argc, char** argv) {
    // 整个程序启动之前, 要先初始化框架
    MprpcApplication::Init(argc, argv);
  
    /* 演示调用远程发布的rpc方法Login, 底层都是
       RpcChannel->CallMethod集中来做rpc方法调用
       的参数序列化和网络发送
    */
    RPC::UserServiceRpc_Stub stub(new MprpcChannel());
    RPC::LoginRequest request;
    request.set_name("Jimmy");
    request.set_pwd("123456");

    RPC::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成, 读取响应
    if(response.res().errcode() == 0) {
        LOG_INFO("rpc login response success: %d", response.success());
    } else {
        LOG_ERR("rpc login response error: %s", response.res().errmsg());
    }


    // 演示调用远程发布的rpc方法Register
    RPC::RegisterRequest req;
    req.set_id(24);
    req.set_name("Bryant");
    req.set_pwd("666");

    RPC::RegisterResponse rsp;
    stub.Register(nullptr, &req, &rsp, nullptr);
    
    if(rsp.res().errcode() == 0) {
        LOG_INFO("rpc register response success: %d", rsp.success());
    } else {
        LOG_ERR("rpc register response error: %s", rsp.res().errmsg());
    }

    return 0; 
}
