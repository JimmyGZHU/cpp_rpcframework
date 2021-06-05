
#include <iostream>
#include <string>

#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "../user.pb.h"

/*
 *  本地方法发布成RPC服务
*/

class UserService : public RPC::UserServiceRpc {        //使用在rpc服务发布端, 也就是提供rpc服务提供者
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "Name: " << name <<", Pwd: " << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id: " << id << ", Name: " << name <<", Pwd: " << pwd << std::endl;
        return true;
    }
    
    
    // 重写Login方法, 下面的方法是框架直接调用的, 通过caller请求的方法和参数
    // 自动匹配到这个方法
    void Login(::google::protobuf::RpcController* controller,
                         const ::RPC::LoginRequest* request,
                         ::RPC::LoginResponse* response,
                         ::google::protobuf::Closure* done)
    {
        //框架给业务上报请求参数LoginRequest, 业务获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool LoginRes = Login(name, pwd);   //把远程拿到的参数来做本地业务

        // 把响应写入response, 由框架交给caller
        RPC::ResultCode *code = response->mutable_res();
        code->set_errcode(0);
        code->set_errmsg("response error!");
        response->set_success(LoginRes);

        // 执行回调
        done->Run();
    }

    
    void Register(::google::protobuf::RpcController* controller,
                         const ::RPC::RegisterRequest* request,
                         ::RPC::RegisterResponse* response,
                         ::google::protobuf::Closure* done)
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id, name, pwd);

        response->mutable_res()->set_errcode(0);
        response->mutable_res()->set_errmsg("");
        response->set_success(ret);
    
        done->Run();
    }
};


int main(int argc, char** argv) {
    /*
     *  本地调用: 
     *  UserService us;
     *  us.Login("xxx", "xxx");
     */

    //调用框架的初始化操作, 格式为 provider -i test.conf
    MprpcApplication::Init(argc, argv); 

    //创建服务发布对象，将本地服务UserService发布到rpc结点
    RpcProvider provider;
    provider.NotifyService(new UserService());

    //启动一个rpc服务发布结点, 进行阻塞等待远程调用请求
    provider.Run();

    return 0;
}
