#include <unistd.h>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex testMutex;

void threadFunc() {
    while (1) {
        testMutex.lock();
        // std::cout << "Thread got lock" << std::endl;
        usleep(100);
        testMutex.unlock();
    }
}

int main() {
    std::thread thread1{threadFunc};
    std::thread thread2{threadFunc};
    std::thread thread3{threadFunc};

    std::cout << "Hello" << std::endl;
    sleep(2);
    return 0;
}