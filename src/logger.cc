#include "logger.h"
#include <time.h>

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    // 异步写日志
    std::thread writeLogTask([&](){
        for(; ;) {
            // 获取当天日期
            time_t now = time(nullptr);
            auto nowtm = localtime(&now);
            
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt",
                    nowtm->tm_year+1900, nowtm->tm_mon+1,
                    nowtm->tm_mday);
            
            FILE* pf = fopen(file_name, "a+");
            if(pf == nullptr) {
                std::cout << "logger file open failed!" << std::endl;
                exit(EXIT_FAILURE);
            }
            
            std::string msg = m_lckQue.Pop();
            
            char buf[128];
            sprintf(buf, "%d:%d:%d =>[%s] ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec, 
                                             (m_loglevel == INFO ? "info" : "error"));
            msg.insert(0, buf);
            msg.append("\n");


            fputs(msg.c_str(), pf);

            fclose(pf);
        }
    });
    writeLogTask.detach();
} 


void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

void Logger::Log(std::string msg) {
    // 把日志信息放到队列中
    m_lckQue.Push(msg);
}

