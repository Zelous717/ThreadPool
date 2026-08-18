#include "ThreadPool.h"



ThreadPool::ThreadPool(size_t threads) : stop(false) 
{
    for (size_t i = 0; i < threads; ++i) 
    {
        workers.emplace_back([this]
             {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                   
                    this->condition.wait(lock, [this]
                         { 
                        return this->stop || !this->tasks.empty(); 
                    });
                    
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
               
                task();//防止线性池退化成单线程
            }
        });
    }
}


// //加入两个监控接口：getTaskCount
// size_t ThreadPool::getTaskCount() const
// {

//     std::lock_guard<std::mutex> lock(queue_mutex);//必须加锁避免读到错误数据
//     return tasks.size();
// }

// //getThreadCount
// size_t ThreadPool::getThreadCount() const
// {
    
//    return workers.size();//
// }

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
       stop = true;
    } 
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}
