//
// Created by 唐仁初 on 2022/10/25.
//

#include "LogSlot.h"
#include "Arena.h"



namespace m_log::internal {

    LogSlot::LogSlot(size_t size, LogServer *pool, Arena *arena)
    : max_size_(size), pool_(pool) {

        start_ = arena ? arena->Allocate(size) : static_cast<char *>(malloc(size));

    }

    LogSlot::~LogSlot() {
        free(start_);
    }

    char *LogSlot::join(int size) {

        int ori_state_ = -1;

        // CAS 控制，尝试加入 slot
        do {
            ori_state_ = state_.load();

            if (ori_state_ < 0 || (ori_state_ + size) > max_size_)
                return nullptr; // 代表当前的slot已经进入了拷贝状态，线程需要加入下一个slot

        } while (!state_.compare_exchange_strong(ori_state_, ori_state_ + size));

        // 如果 ori_state == 0，代表该线程是主线程，需要负责分配下一个缓冲区，并且等待线程组
        if (ori_state_ == 0) {

            // 标记下一个 slot
            nxt_ = pool_->updateSlot();

            flag_.store(1);

            int total;
            // CAS 控制，等待所有线程 join 完毕。
            do {
                total = state_.load();
            } while (!state_.compare_exchange_strong(total, -total - 1));   // 要求所有线程全部join完成

            //
            total_ = total;
        }

        return (start_ + ori_state_);
    }

    void LogSlot::copy(char *pos, const char *str, int size, bool sync) {

        sync_ = std::max(sync, sync_);

        flag_.wait(0);

        memcpy(pos, str, size);

        // 最后一个写入的线程负责将slot写入硬盘
        if (state_.fetch_add(size) == (-size - 1)) {

            commit_flag.wait(false);
            pool_->commit(start_, total_, sync_);
            nxt_->allowCommit();

            flag_.store(2);
        }
    }

    void LogSlot::wait() {
        flag_.wait(1);  // 等待写入完成
    }

    void LogSlot::allowCommit() {
        commit_flag.store(true);
        commit_flag.notify_one();
    }
}