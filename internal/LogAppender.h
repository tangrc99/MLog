//
// Created by 唐仁初 on 2022/10/25.
//

#ifndef MONGOLOG_LOGAPPENDER_H
#define MONGOLOG_LOGAPPENDER_H

#include <fstream>
#include <thread>
#include <iostream>

namespace m_log::internal {


    /// @brief Writes the log to the specified location.
    /// @details LogAppender is an abstract base class.
    class LogAppender {
    public:

        explicit LogAppender() = default;

        virtual ~LogAppender() = default;

        /// Interface to write log to specified location.
        /// \param start Ptr of log
        /// \param size Size of log
        /// \param sync Is flush required
        virtual void write(const char *start, size_t size, bool sync) = 0;

    protected:
        std::mutex mtx; // 互斥锁
    };


    /// @brief Writes the log to the specified file.
    class FileAppender : public LogAppender {
    public:

        /// Construct a file log appender.
        /// \param file_name Filename to write log
        explicit FileAppender(const std::string &file_name) : LogAppender() {
            if (!file_name.empty()) {
                os.open(file_name, std::ios::app);
                assert(os.is_open());
            }
        }

        /// Destruction, sync all content to disk
        ~FileAppender() override {
            if (os.is_open()) {
                os.flush();
                os.close();
            }
        }

        /// Interface to write log to file.
        /// \param start Ptr of log
        /// \param size Size of log
        /// \param sync Is flush required
        void write(const char *start, size_t size, bool sync) override {

            std::lock_guard<std::mutex> lg(mtx);    // TODO : 改变成队列单独来写

            os.write(start, static_cast<long>(size));

            if (sync)
                os.flush();    // 确保刷盘
        }

    protected:
        std::mutex mtx;
        std::ofstream os;
    };


    /// @brief Writes the log to stdout.
    class PrintAppender : public LogAppender {
    public:

        /// Construct a print appender
        explicit PrintAppender() : LogAppender() {}

        /// Interface to write log stdout.
        /// \param start Ptr of log
        /// \param size Size of log
        /// \param sync Is flush required
        void write(const char *start, size_t size, bool sync) override {
            std::lock_guard<std::mutex> lg(mtx);
            std::cout << std::string_view(start,size);
        }
    };

}

#endif //MONGOLOG_LOGAPPENDER_H
