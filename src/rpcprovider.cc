#include <string>
#include <functional>
#include <google/protobuf/descriptor.h>

#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

// 将Service服务发布成RPC服务的接口, 提供给外部调用
// 因为是框架, 我们不能用自己定义的UserService类做参数, 而应该用Service基类服务对象
void RpcProvider::NotifyService(google::protobuf::Service* service) {
    ServiceInfo service_info;

    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();

    // 获取服务的名字
    std::string service_name = pserviceDesc->name();

    // 获取服务对象方法的数量
    int methodCnt = pserviceDesc->method_count();

    LOG_INFO("service_name: %s", service_name.c_str());

    for(int i = 0; i < methodCnt; ++i) {
        // 填充服务对象和其发布的服务方法之间的映射表
        const google::protobuf::MethodDescriptor* pMethodDesc= pserviceDesc->method(i);
        std::string method_name = pMethodDesc->name();
        service_info.m_methodMap[method_name] =  pMethodDesc;
        
        LOG_INFO("method_name: %s", method_name.c_str());
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});

    //
}

void RpcProvider::Run() {
    std::string ip = MprpcApplication::GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
   
    // 创建tcp server
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    // 绑定连接回调和消息读写回调
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));    
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, 
                                        std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);

    // 把当前rpc结点上要发布的服务注册到zk上, 让zk client可以发现服务
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性结点, method_name为临时性结点
    for(auto& sp : m_serviceMap) {
        // 找到service的路径:  /service_name
        std::string service_path = '/' + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        
        for(auto& mp : sp.second.m_methodMap) {
            // 找到method的路径: /service_name/method_name, 存储当前这个rpc结点的主机ip和port
            std::string method_path = service_path + '/' + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);

            // ZOO_EPHEERAL表示临时性结点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    std::cout << "RpcProvider statt! ip:" << ip.c_str() << "  port:" << port << std::endl;
    LOG_INFO("RpcProvider start! ip:%s  port:%d", ip.c_str(), port);
    
    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        // 连接已经断开
        conn->shutdown();
    }
}
    

/*
 *  在框架内部, RpcProvider和RpcConsumer协商好通信用的protobuf数据类型
 *  需要传入service_name method_name args(参数, 如name pwd)给到框架, 这些可以在.proto文件进行定义
 *  为了解决粘包问题, 我们需要对数据长度进行区分, 包括header_size + header_str + args_size
 */
// 已连接用户的读写事件回调
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, 
                            muduo::net::Buffer* buffer, 
                            muduo::Timestamp) 
{
    // 网络上接收的远程rpc调用请求的字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的数据
    // 从recv_buf中拷贝前4个字节放到header_size里
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 读取数据头的原始字符流
    std::string rpc_header_str = recv_buf.substr(4, header_size);   
    
    // 反序列化数据
    mprpc::RpcHeader rpcHeader;
    std::string service_name;   // 请求的服务对象名
    std::string method_name;    // 请求的方法名
    std::string args_str;       // 参数内容
    uint32_t args_size;

    if(rpcHeader.ParseFromString(rpc_header_str)) {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
        args_str = recv_buf.substr(4 + header_size, args_size);
    } else {
        LOG_ERR("rpcHeader parse failed!");
        return;
    }
        

    // 打印调试信息
    std::cout << "======================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "======================" << std::endl;


    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end()) {
        LOG_INFO("service: %s Not Found!", service_name.c_str());
    }
    
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end()) {
        LOG_INFO("service: %s's method: %s Not Found!", service_name.c_str(), method_name.c_str());
    }

    google::protobuf::Service* service = it->second.m_service;  //service对象(new UserService)
    const google::protobuf::MethodDescriptor* method = mit->second; //method方法 (Login)

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)) {
        LOG_ERR("request parse failed!");
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    //  绑定一个Closure的回调函数, 供下面的CallMethod调用
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&,
                                                                    google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, 
                                                                                                conn, response);
    
    // 在框架上根据远端rpc请求, 调用当前rpc结点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);  //相当于UserService.Login(...)
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if(response->SerializeToString(&response_str)) { //进行序列化
        // 接下来通过网络发送出去
        conn->send(response_str);
        conn->shutdown();       //短连接, RpcProvide主动断开连接 
    } else {
        LOG_ERR("RpcProvider response serialized failed!");
    }
}
