//
// Created by erkan on 1.04.2025.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "TaskQueue.h"
#include "Thread.h"


class ThreadPool {
private:
    TaskQueue taskQueue;
    int numberOfThreads = 1;
    std::vector<Thread> threads;
public:
    // ThreadPool() = default;
    ThreadPool(int numberOfThreads);
    ~ThreadPool();
    void Parallel(int num_obj, std::function<void(int start, int end)>&& callback);
};



#endif //THREADPOOL_H
