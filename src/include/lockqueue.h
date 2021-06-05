/*************************************************************************
    > File Name: lockqueue.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2021年05月21日 星期五 09时41分14秒
 ************************************************************************/

#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 日志队列

template<typename T>
class LockQueue {
public:
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_all();
    }
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_queue.empty()) {
            m_condvariable.wait(lock);
        }
        T val = m_queue.front();
        m_queue.pop();
        lock.unlock();
        return val;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};


