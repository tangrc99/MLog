//
// Created by 唐仁初 on 2022/10/25.
//

#include "LogSlot.h"
#include "LogServer.h"
#include "LogAppender.h"
#include <memory>
#include <utility>

namespace m_log::internal {

    LogServer::LogServer(const std::string &file, size_t slot_size)
            : max_size_(slot_size), cur_slot_(std::make_shared<LogSlot>(slot_size, this, nullptr)){

        if(file.empty())
            appender_ = new PrintAppender;
        else
            appender_ = new FileAppender(file);

        cur_slot_->allowCommit();
    }

    LogServer::~LogServer() {
        delete appender_;
    }

    std::shared_ptr<LogSlot> LogServer::updateSlot() {

        auto new_slot = std::make_shared<LogSlot>(max_size_, this );

        // 先分配再进入临界区

        std::lock_guard<std::mutex> lg(mtx);
        cur_slot_ = new_slot;

        return new_slot;
    }

    void LogServer::commit(const char *str, size_t size, bool sync) {

        // 本次 slot 提交
        appender_->write(str, size, sync);
        // 允许下一个 slot 提交，因为一次只能够提交一个，不用加锁

        //        std::cout << std::string(start_, total_);// << std::endl;
    }

    std::shared_ptr<LogSlot> LogServer::getSlot(size_t size) {
        if (size > max_size_) {
            changeSlotSize(size);
            return updateSlot();
        }
        std::lock_guard<std::mutex> lg(mtx);
        return cur_slot_;
    }
}