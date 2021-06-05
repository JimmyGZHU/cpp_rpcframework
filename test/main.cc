#include "./test.pb.h"
#include <iostream>
#include <string>
using namespace mprpc;

void Seria_Request(std::string name, std::string pwd) {
    LoginRequest req;
    req.set_name(name);
    req.set_pwd(pwd);


    //对数据进行序列化
    std::string send_str;
    if(!req.SerializeToString(&send_str)) {
        std::cout << "数据序列化失败" << std::endl;
    }

    //对数据进行反序列化
    LoginRequest reqB;
    if(!reqB.ParseFromString(send_str)) {
        std::cout << "数据反序列化失败" << std::endl;
    }
   
    std::cout << "name: " << reqB.name() << ", password: "
              << reqB.pwd() << std::endl;
}

int main() {
    /*
    LoginResponse rsp;
    ResultCode* rc = rsp.mutable_res();
    rc->set_errcode(1);
    rc->set_errmsg("登录处理失败");
    */

    //如果message成员也是一个messgae, 通过mutable_xxx来调用
    GetFriendListResponse rsp;
    ResultCode* rc = rsp.mutable_res();
    rc->set_errcode(1);
    rc->set_errmsg("登录处理失败");

    // 下面就是repeated容器的使用示例
    User* user1 = rsp.add_friend_list();
    user1->set_age(6);
    user1->set_name("Lebron");
    user1->set_sex(User::MAN);
    
    User* user2 = rsp.add_friend_list();
    user2->set_age(23);
    user2->set_name("Jordan");
    user2->set_sex(User::MAN);

    return 0;
}
