/**
 * @file Item39.cpp
 * @brief Consider *void* features for one-shot event communication
 * @date 2020/7/21
 */

#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <future>


class Notify {

public:
    std::condition_variable cv;
    std::mutex m;

    std::atomic<bool> flag{false};

    bool flag2 = false;

    std::promise<void> p;

public:
    /**
     * 使用条件变量通知另外一个线程
     * 令人生疑的地方：互斥锁按理说应该是用来保证共享数据的访问的原子性，在这里check线程和react线程却
     * 没有任何需要共享的数据，却仍然需要一个互斥锁。
     *
     * 即使抛开上述疑问不谈，使用条件变量还有两点需要注意的地方
     * 1. 由于操作系统的调度，两个线程之间的执行顺序不同，有可能check线程在react线程wait之前就使用条件变量
     * 进行通知，导致react线程一直处于等待的状态
     * 2. 条件变量存在虚假唤醒的情况（仅此一种方法会存在这种情况），因此在react线程收到通知后，首先要做的就是
     * 检查是否真的达到了运行条件。但是既然react线程都能够检查条件了，还要check线程干嘛？也就是说如果存在react线程
     * 无法检查运行条件是否成立的情况，那么条件变量的使用条件就失效了
     */
    void useCV() {
        std::cout << ">>>> Use condition variable to notify the thread" << std::endl;
        // reacting task
        std::thread react([&]{
            {
                // code smell
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk);
                // cv.wait(lk, []{ return /* 确定条件是否发生 */});
                std::cout << "react!" << std::endl;
            }
        });

        std::thread check([&]{
            std::cout << "check!" << std::endl;
            cv.notify_one();
        });

        /* 存在check线程先运行导致react没有收到通知的情况 */
        react.join();
        check.join();
    }

    /**
     * 使用支持原子操作的布尔标志位来进行通知，能够避免条件变量需要互斥体的缺点，并且不存在虚假唤醒的可能性。
     * 但是其缺点在于轮询标志位的成本很高，反应线程在等待通知的时候应该被阻塞，处于休眠状态，而不是一直占用时间片来进行轮询
     */
    void useBool() {
        std::cout << ">>>> Use bool flag to notify the thread" << std::endl;
        std::thread react([&]{
            while(!flag);
            std::cout << "react!" << std::endl;
        });

        std::thread check([&]{
            std::cout << "check!" << std::endl;
            flag = true;
        });

        react.join();
        check.join();
    }

    /**
     * 为了真正的阻塞反应线程，使用不带atomic的标志位和条件变量来通知反应线程，同时使用互斥锁阻止并发访问标志位。
     * 这种方法虽然实现了通知的效果，但是很怪异：
     * - 条件变量是用来通知反应线程事件已经发生，但是反应线程仍要检查标志位事件已经发生
     * - 标志位表明事件已经发生，但是反应线程却仍然需要靠条件变量被通知事件发生
     */
    void useBoolAndMutex() {
        std::cout << ">>>> Use bool flag and mutex to notify the thread" << std::endl;

        std::thread react([&]{
            {
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk, [&] { return flag2; });
                std::cout << "react!" << std::endl;
            }
        });

        std::thread check([&]{
            {
                std::lock_guard<std::mutex> g(m);
                std::cout << "check!" << std::endl;
                flag2 = true;
            }
            cv.notify_one();
        });

        check.join();
        react.join();
    }

    /**
     * 使用期值来通知反应线程，抛弃条件变量和标志位，检测任务使用std::promise<void>，反应任务
     * 使用std::future<void>或者std::shared_future<void>
     *
     * 虽然期望值能够解决之前的问题，但是由于std::promise和std::future是共享状态，建立在堆上，因此就有分配和回收内存的成本
     * 而且std::promise每个对象只能够使用一次，因此期值只适用于一次性通信
     */
    void usePromise() {
        std::cout << ">>>> Use std::promise and std::future to notify thread" << std::endl;
        std::thread react([&] {
            p.get_future().wait();
            std::cout << "react!" << std::endl;
        });

        std::thread check([&] {
            std::cout << "check!" << std::endl;
            p.set_value();
        });

        react.join();
        check.join();
    }

};

int main() {
    Notify c;
    c.useCV();
    c.useBool();
    c.useBoolAndMutex();
    c.usePromise();
}

