//
// Created by 唐仁初 on 2022/10/25.
//


#include "Logger.h"
#include "internal/LogServer.h"
#include "internal/LogSlot.h"
#include <thread>
#include <sstream>

namespace m_log {

    void Logger::debug(const char *content, size_t size, SyncLevel sync) {
        writeLog(DEBUG, content, size, sync);
    }

    void Logger::info(const char *content, size_t size, SyncLevel sync) {
        writeLog(INFO, content, size, sync);
    }

    void Logger::warn(const char *content, size_t size, SyncLevel sync) {
        writeLog(WARN, content, size, sync);
    }

    void Logger::error(const char *content, size_t size, SyncLevel sync) {
        writeLog(ERROR, content, size, sync);
    }

    void Logger::fatal(const char *content, size_t size, SyncLevel sync) {
        writeLog(FATAL, content, size, sync);
    }

    void Logger::writeLog(FilterLevel level, const char *content, size_t size, SyncLevel sync) {

        if (!filter->pass(level, size))
            return;

        time_t rawtime;
        time(&rawtime);
        tm *ptminfo = localtime(&rawtime);

        char str[20];

        strftime(str, 20, "%Y-%m-%d %I:%M:%S", ptminfo);

        std::stringstream ss;
        ss << str << " " << toString(level) << " [" << std::this_thread::get_id() << "] - " << content << "\n";

        std::string log(ss.str());

        auto slot = pool_->getSlot(log.size());

        char *pos = slot->join(log.size());

        while (pos == nullptr) {
            slot = pool_->getSlot(log.size());
            pos = slot->join(log.size());
        }

        slot->copy(pos, log.data(), log.size(), sync == WR_DISK);

        if (sync > NO_SYNC)
            slot->wait();
    }

    Logger::Logger(const std::string &file,FilterLevel level,size_t log_size) : pool_(new internal::LogServer(file, log_size)),
                                              filter(new internal::LogFilter(level)) {

    }

    Logger::~Logger() {
        delete filter;
        delete pool_;
    }

    std::string Logger::toString(FilterLevel level) {
        switch (level) {

            case DEBUG:
                return "DEBUG";
            case INFO:
                return "INFO";
            case WARN:
                return "WARN";
            case ERROR:
                return "ERROR";
            case FATAL:
                return "FATAL";
        }
        return "";
    }


}