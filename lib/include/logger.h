/*************************************************************************
    > File Name: logger.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2021年05月21日 星期五 09时38分40秒
 ************************************************************************/

#pragma once

// 框架的日志系统
//
#include "./lockqueue.h"
#include <string>

enum LogLevel {
    INFO,
    ERROR
};

class Logger {
public:
    void SetLogLevel(LogLevel level);         // 设置日志级别
    void Log(std::string msg);  // 写日志

    static Logger& GetInstance();   //获取唯一实例

private:
    int m_loglevel;     // 日志级别
    LockQueue<std::string> m_lckQue;    //日志缓冲队列

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};


#define LOG_INFO(format, ...)\
    do\
    { \
        Logger& logger = Logger::GetInstance();\
        logger.SetLogLevel(INFO);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, format, ##__VA_ARGS__);\
        logger.Log(buf);\
    } while(0)

#define LOG_ERR(format, ...)\
    do\
    { \
        Logger& logger = Logger::GetInstance();\
        logger.SetLogLevel(ERROR);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, format, ##__VA_ARGS__);\
        logger.Log(buf);\
    } while(0)

