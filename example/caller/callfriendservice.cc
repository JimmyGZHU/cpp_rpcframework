#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"
#include "logger.h"

int main(int argc, char** argv) {
    // 整个程序启动之前, 要先初始化框架
    MprpcApplication::Init(argc, argv);
  
    /* 演示调用远程发布的rpc方法GetFriendsLists, 底层都是
       调用RpcChannel->CallMethod集中来做rpc方法调用的参
       数序列化和网络发送
    */
    RPC::FriendServiceRpc_Stub stub(new MprpcChannel());
    RPC::GetFriendsListRequest request;
    request.set_userid(1000);

    RPC::GetFriendsListResponse response;

    MprpcController controller;     // 记录一些错误信息
    
    stub.GetFriendsList(&controller, &request, &response, nullptr); //RpcChannel->CallMethod

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    } else {
    // 一次rpc调用完成, 读取响应
        if(response.res().errcode() == 0) {
            std::cout << "rpc GetFriendsList response success: " << response.success() << std::endl;
            int size = response.size();
            for(int i = 0; i < size; ++i) {
                std::cout << "第" << i + 1 << "个, name: " << response.friends(i) << std::endl;
            }
        } else {
            std::cout << "rpc GetFriendsList response error: " << response.res().errmsg() << std::endl;
            LOG_ERR("rpc GetFriendsListResponse error: %s", response().res().errmsg());        
        }
    }
    return 0;
}

