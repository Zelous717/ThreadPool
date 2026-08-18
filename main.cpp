#include <iostream>
#include <chrono>
#include "ThreadPool.h"

std::mutex cout_mutex;
int main() {
    // 1. 创建一个包含 4 个工作线程的线程池
    ThreadPool pool(4);

    // 2. 提交 8 个任务
    for (int i = 0; i < 8; ++i)
    {
       
        pool.enqueue([i]
        {
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "hello " << i << std::endl;
            }
            // 模拟耗时：原图是 seconds(1)，这里改成毫秒方便测试
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        });
    }

    // 3. 主线程继续往下走，立即打印
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "所有任务已提交" << std::endl;
    }

    // 此时主线程会隐式等待剩下的子线程做完任务才真正退出程序。
    return 0;
}