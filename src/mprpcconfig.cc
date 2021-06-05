#include <iostream>
#include <string>
#include <cstdlib>

#include "mprpcconfig.h"
#include "logger.h"

void MprpcConfig::LoadConfigFile(const char* config_file) {
    FILE* pf = fopen(config_file, "r");
    if(pf == nullptr) {
        LOG_ERR("%s is not exist!", config_file);
        exit(EXIT_FAILURE);
    }

    while(!feof(pf)) {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        // 去掉字符串前面多余的空格
        std::string src_buf(buf);
        Trim(src_buf);
        
        // 判断#的注释
        if(src_buf[0] == '#' || src_buf.empty()) {
            continue;
        } 

        // 解析配置项
        int idx = src_buf.find('=');
        if(idx == -1) {
            continue;
        }
        
        //存储配置项
        std::string key = src_buf.substr(0, idx);
        Trim(key);

        //127.0.0.1    \n
        int endidx = src_buf.find('\n', idx);
        std::string value = src_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);
        
        mp.insert({key, value});
    }
}

std::string MprpcConfig::Load(const std::string key) {
    if(mp.find(key) == mp.end()) {
        return "";
    }
    return mp[key];
}

void MprpcConfig::Trim(std::string& src_buf) {
    // 去掉字符串前面的空格
    int idx = src_buf.find_first_not_of(' ');
    if(idx != -1) {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);        
    }
    
    // 去掉字符串后面的空格
    idx = src_buf.find_last_not_of(' ');
    if(idx != -1) {
        src_buf = src_buf.substr(0, idx + 1);
    }
}
