#include <iostream>
#include <string>
#include <vector>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"

class FriendService : public RPC::FriendServiceRpc{
public:
    std::vector<std::string> GetFriendsLists(uint32_t userid) {
        std::cout << "do GetFriendsLists service!" << std::endl;
        std::vector<std::string> vec{"John", "Kevin", "Mary"};
        return vec;
    }   
    
    void GetFriendsLists(::google::protobuf::RpcController* controller,
                         const ::RPC::GetFriendsListsRequest* request,
                         ::RPC::GetFriendsListsResponse* response,
                         ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();                            //取请求参数
        std::vector<std::string> friendsList = GetFriendsLists(userid); //做本地业务

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("GetFriendLists");
        for(std::string& name : friendsList) {
            // 注意friends是个列表
            auto* p = response->add_friends(); 
            *p = name;
        }
        done->Run();
    }
};


int main(int argc, char** argv) {
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new FriendService());

    provider.Run();

    return 0;
}
