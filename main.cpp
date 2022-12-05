

#include <sstream>
#include <iostream>
#include <thread>
#include "internal/LogSlot.h"
#include "Logger.h"
#include <cstdio>

#include <fstream>


using namespace m_log;
using namespace m_log::internal;

int main() {



    Logger logger("");
    auto start = std::chrono::system_clock::now();

    std::thread th([&logger]{
        for(int i = 0;i < 1000;i++){
            auto s= std::to_string(i);
            logger.debug(s.c_str(),s.size(),Logger::WR_DISK);

        }
    });

    for(int i = 0;i < 100000;i++){
        auto s= std::to_string(i);
        logger.debug(s.c_str(),s.size(),Logger::WR_DISK);

    }
    th.join();

    auto end = std::chrono::system_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;


    return 0;



    return 0;
}
