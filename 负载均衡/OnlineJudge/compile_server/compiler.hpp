#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler
{
    // 引入路径拼接功能
    using namespace ns_util;
    using namespace ns_log;

    class Compiler
    {
    public:
        Compiler(){}
        ~Compiler(){}
        static bool Compile(const std::string &file_name){
            pid_t pid = fork();
            if(pid < 0){
                LOG(ERROR) << "内部错误,创建子进程失败" << "\n";
                // std::cout << "内部错误,创建子进程失败" << std::endl;
                return false;
            }
            else if(pid == 0){
                // 子进程
                int _stderr = open(PathUtil::CompilerError(file_name).c_str(),O_CREAT | O_WRONLY,0644);
                if(_stderr < 0){
                    LOG(WARNING) << "没有成功形成stderr文件" << "\n";
                    // std::cout << "没有成功形成stderr文件" << std::endl;
                }
                // 将标准错误 重定向 到 _stderr中
                dup2(_stderr,2);

                // 程序替换,并不影响进程的文件描述符表
                // 子进程: 调用编译器,完成对代码的编译工作
                // g++ -o target src -std=c++11
                execlp("g++","g++","-o",PathUtil::Exe(file_name).c_str(),\
                PathUtil::Src(file_name).c_str(),"-D","COMPILER_ONLINE","-std=c++11",nullptr/*不要忘记*/);
                LOG(ERROR) << "启动编译器g++失败, 可能是参数错误" << "\n";
                // std::cout << "启动编译器g++失败, 可能是参数错误" << std::endl;
                exit(2);
            }
            else{
                // 父进程
                waitpid(pid,nullptr,0);// 不关注返回值,且阻塞式等待
                // 编译是否成功,就看有没有形成对应的可执行程序
                if(FileUtil::IsFileExists(PathUtil::Exe(file_name))){
                    LOG(INFO) << "编译成功" << "\n";
                    // std::cout << "编译成功" << std::endl;
                    return true;
                }
            }
            LOG(ERROR) << "编译失败,没有生成可执行程序" << "\n";
            // std::cout << "编译失败,没有生成可执行程序" << std::endl;
            return false;
        }
    };
}