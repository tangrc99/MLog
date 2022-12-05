//
// Created by 唐仁初 on 2022/10/25.
//

#ifndef MONGOLOG_LOGSERVER_H
#define MONGOLOG_LOGSERVER_H

#include <memory>
#include <atomic>
#include <mutex>


namespace m_log::internal {

    class LogSlot;

    class LogAppender;


    /// @brief LogServer is responsible for allocating all LogSlot and writing LogSlot to disk.
    class LogServer {
    public:

        /// Construct a LogServer written log to given file.If file is not assigned, log will be printed with stdout.
        /// \param file File to write log
        /// \param slot_size Initial slot size.
        explicit LogServer(const std::string &file, size_t slot_size);

        ~LogServer();

        /// Change slot size to given size.
        /// \param slot_size New slot size
        void changeSlotSize(size_t slot_size) {
            max_size_ = slot_size;
        }

        /// Get a new slot with requested size. If initial slot size is too small, it will be increased.
        /// \param size Requested size
        /// \return Allocated LogSlot
        std::shared_ptr<LogSlot> getSlot(size_t size);

        /// Update current LogSlot. This function will be called by current LogSlot after join stage ends.
        /// \return New LogSlot
        std::shared_ptr<LogSlot> updateSlot();

        /// Commit a LogSlot.
        /// \param str Log content
        /// \param size Log size
        /// \param sync Is sync required
        void commit(const char *str, size_t size, bool sync);


    private:

        std::mutex mtx;
        std::shared_ptr<LogSlot> cur_slot_; // ptr pointed to current slot
        std::atomic<size_t> max_size_;  // max size of single max size

        LogAppender *appender_;

    };

}


#endif //MONGOLOG_LOGSERVER_H
