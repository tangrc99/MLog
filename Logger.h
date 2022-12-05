//
// Created by 唐仁初 on 2022/10/25.
//

#ifndef MONGOLOG_LOGGER_H
#define MONGOLOG_LOGGER_H

#include <string>

namespace m_log {

    namespace internal {

        class LogServer;

        class LogFilter;
    }

    /// @brief User interface
    /// @details Logger refers to the log commit pattern in MongoDB. Logs are group committed sequentially. User can
    /// select log level and sync level while writing each log.
    class Logger {
    public:

        enum SyncLevel {
            /// Return after log is written to slot
            NO_SYNC,
            /// Return after log is written to os cache
            WR_BUF,
            /// Return after log is written to disk
            WR_DISK
        };

        enum FilterLevel {

            /// Allowed level gte DEBUG
            DEBUG,
            /// Allowed level gte INFO
            INFO,
            /// Allowed level gte WARN
            WARN,
            /// Allowed level gte ERROR
            ERROR,
            /// Allowed level gte FATAL
            FATAL,
        };


        /// Construct a logger to assigned file. If file is empty, log will be printed with stdout
        /// \param file File to write log
        /// \param slot_size Initial slot size. Default 1 kb
        explicit Logger(const std::string &file, FilterLevel level = DEBUG, size_t slot_size = 1024);

        ~Logger();


        /// Write a debug log.
        /// \param content Log content
        /// \param size Log size
        /// \param sync Sync level
        void debug(const char *content, size_t size, SyncLevel sync = NO_SYNC);

        /// Write a debug log.
        /// \param content Log content
        /// \param size Log size
        /// \param sync Sync level
        void info(const char *content, size_t size, SyncLevel sync = WR_BUF);

        /// Write a debug log.
        /// \param content Log content
        /// \param size Log size
        /// \param sync Sync level
        void warn(const char *content, size_t size, SyncLevel sync = WR_BUF);

        /// Write a debug log.
        /// \param content Log content
        /// \param size Log size
        /// \param sync Sync level
        void error(const char *content, size_t size, SyncLevel sync = WR_DISK);

        /// Write a debug log.
        /// \param content Log content
        /// \param size Log size
        /// \param sync Sync level
        void fatal(const char *content, size_t size, SyncLevel sync = WR_DISK);


    private:

        void writeLog(FilterLevel level, const char *content, size_t size, SyncLevel sync);

        static std::string toString(FilterLevel level);

        internal::LogServer *pool_;
        internal::LogFilter *filter;
    };


    namespace internal{

        /// @brief Filter logs that do not match the level
        class LogFilter {
        public:

            /// Construct a log filter with given threshold.
            /// \param threshold Minimum threshold
            explicit LogFilter(Logger::FilterLevel threshold) : min_level_(threshold) {

            }

            /// Check whether log level passed.
            /// \param level log level
            /// \param size log size
            /// \return Is passed
            bool pass(Logger::FilterLevel level, size_t size) {
                return level >= min_level_;
            }

        private:
            Logger::FilterLevel min_level_;
        };
    }


}

#endif //MONGOLOG_LOGGER_H
