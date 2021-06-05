/*************************************************************************
    > File Name: mprpcconfig.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2021年05月13日 星期四 09时29分12秒
 ************************************************************************/
#pragma once

#include <unordered_map>
#include <string>

class MprpcConfig {
public:
    // 负责解析加载配置文件
    void LoadConfigFile(const char* config_file);
    // 查询配置项信息
    std::string Load(const std::string key);
private:
    /* rpcserverip rpcserverport zookeeperip zookeeperprot*/
    std::unordered_map<std::string, std::string> mp;

    // 去掉字符串前后的空格
    void Trim(std::string& src_buf);
};
