cmake_minimum_required(VERSION 3.20)
project(MLog)

set(CMAKE_CXX_STANDARD 20)


add_executable(MLog
        main.cpp
        internal/LogSlot.cpp
        internal/LogSlot.h internal/LogServer.cpp internal/LogServer.h
        internal/Arena.cc
        internal/Arena.h internal/LogAppender.cpp internal/LogAppender.h Logger.cpp Logger.h)
