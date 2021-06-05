/*************************************************************************
    > File Name: zookeeperutil.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2021年05月25日 星期二 09时20分08秒
 ************************************************************************/

#pragma once

#include <string>
#include <iostream>
#include <semaphore.h>
#include <zookeeper/zookeeper.h>


// zookeeper客户端类
class ZkClient {
public:
    ZkClient();
    ~ZkClient();
    
    // zkclient启动连接zkserver
    void Start();

    // 在zkserver上根据参数path创建znode结点
    void Create(const char* path, const char* data,
                int datalen, int state = 0);

    // 根据参数path获取znode结点的值
    std::string GetData(const char* path);

private:
    // zk的客户端句柄
    zhandle_t* m_zhandle;
};
