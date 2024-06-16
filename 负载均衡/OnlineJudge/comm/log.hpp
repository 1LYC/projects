#pragma once

#include <iostream>
#include <string>
#include "util.hpp"

namespace ns_log
{
    using namespace ns_util;

    // 日志等级
    enum {
        INFO, // 正常
        DEBUG,// 调试
        WARNING,// 警告
        ERROR, // 错误 
        FATAL // 致命
    };
    inline std::ostream &Log(const std::string &level,const std::string &file_name,int line)
    {
        // 添加日志等级
        std::string messagge = "[";
        messagge += level;
        messagge += "]";

        // 添加报错文件名称
        messagge = "[";
        messagge += file_name;
        messagge += "]";
        
        // 添加报错行
        messagge = "[";
        messagge += std::to_string(line);
        messagge += "]";
        
        // 日志时间戳
        messagge += "[";
        messagge += TimeUtil::GetTimeStamp();
        messagge += "]";

        // cout本质 内部就是包含缓冲区的
        std::cout << messagge;
        return std::cout;
    }
    // 为了直接LOG(INFO) << "message" << "\n";这样调用
    // 直接实现开发式日志
    #define LOG(level) Log(#level, __FILE__, __LINE__)
} // namespace ns_log
