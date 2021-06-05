#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include "logger.h"


// 全局的watcher观察器(是一个异步线程), zkserver给zkclient发送通知
void global_watcher(zhandle_t* zh, int type, int state, const char* path,
                    void* watcherCtx) {
    if(type == ZOO_SESSION_EVENT) {
        // 回调的消息类型是和会话相关的消息类型
        if(state == ZOO_CONNECTED_STATE) {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}



ZkClient::ZkClient() : m_zhandle(nullptr) {

}

ZkClient::~ZkClient() {
    if(m_zhandle != nullptr) {
        zookeeper_close(m_zhandle);
    }
}

// zk客户端启动连接zk服务端
void ZkClient::Start() {
    
    //加载zk的IP和端口号，默认为2181
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;

    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(m_zhandle == nullptr) {
        LOG_ERR("zookeeper init error!");
        exit(EXIT_FAILURE);
    }
    
    sem_t sem;
    sem_init(&sem, 0, 0);   //初始化资源为0
    zoo_set_context(m_zhandle,&sem); //设置上下文，添加额外信息

    sem_wait(&sem);
    LOG_INFO("zookeeper_init success!");
}


// 在zkserver上根据指定的path创建znode结点
void ZkClient::Create(const char* path, const char* data,
                int datalen, int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);

    int flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if(flag == ZNONODE) {
        // 结点不存在, 则创建结点
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE,
                          state, path_buffer, bufferlen);
        if(flag == ZOK) {
            LOG_INFO("znode create successfully!");
        } else {
            LOG_ERR("znode create error!");
            std::cout << "flag: " << flag << std::endl;
            std::cout << "znode create error... path: " << path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

// 传入参数指定结点的路径, 获取znode结点的值
std::string ZkClient::GetData(const char* path) {
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);

    if(flag != ZOK) {
        LOG_ERR("get znode data error!");
        std::cout << "get znode error... path: " << path << std::endl; 
        return "";
    } else {
        // 获取成功
        return buffer;
    }
}

