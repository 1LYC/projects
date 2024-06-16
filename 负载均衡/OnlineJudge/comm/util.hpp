#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <atomic>
#include <fstream>
#include <vector>
// boost库头文件
#include <boost/algorithm/string.hpp>

namespace ns_util
{
    class TimeUtil
    {
    public:
        static std::string GetTimeStamp(){
            struct timeval _time;
            gettimeofday(&_time,nullptr);
            return std::to_string(_time.tv_sec);
        }
        
        // 获得毫秒级时间戳
        static std::string GetTimeMs()
        {
            struct timeval _time;
            gettimeofday(&_time,nullptr);
            // 秒*1000 = 毫秒,微秒/1000 = 毫秒
            return std::to_string(_time.tv_sec*1000+_time.tv_usec/1000);
        }
    };

    const std::string temp_path = "./temp/";
    
    class PathUtil
    {
    public:
        static std::string AddSuffix(const std::string &file_name,const std::string &suffix){
            std::string path_name = temp_path;//  ./temp/
            path_name += file_name;// ./temp/1234
            path_name += suffix;// ./temp/1234后缀名
            return path_name;
        }
        // 编译时需要的临时文件
        // 将文件名为1234 -> ./temp/1234.cpp
        static std::string Src(const std::string &file_name){
            return AddSuffix(file_name,".cpp");
        }
        // 将文件名为1234 -> ./temp/1234.exe
        static std::string Exe(const std::string &file_name){
            return AddSuffix(file_name,".exe");
        }
        // 将文件名为1234 -> ./temp/1234.compile_error
        static std::string CompilerError(const std::string &file_name){
            return AddSuffix(file_name,".compile_error");
        }

        // 运行时需要的临时文件
        static std::string Stdin(const std::string &file_name){
            return AddSuffix(file_name,".stdin");
        }
        static std::string Stdout(const std::string &file_name){
            return AddSuffix(file_name,".stdout");
        }
        static std::string Stderr(const std::string &file_name){
            return AddSuffix(file_name,".stderr");
        }
    };
  
    class FileUtil
    {
    public:
        static bool IsFileExists(const std::string &path_name){
            struct stat st;//可以记录文件的各种属性信息
            if(stat(path_name.c_str(),&st) == 0){
                // 获取属性成功,文件已经存在
                return true;
            } 
            return false;
        }

        static std::string UniqFileName(){
            static std::atomic_uint id(0);// 具有原子性
            id++;
            // 使用毫秒级时间戳+原子性递增唯一值: 来保证唯一性
            std::string ms = TimeUtil::GetTimeMs();// 获得毫秒级时间戳
            std::string uniq_id = std::to_string(id);
            return ms+"_"+uniq_id;
        }

        static bool WriteFile(const std::string&target,const std::string &content){
            std::ofstream out(target);// 输出流
            if(!out.is_open()){
                return false;
            }
            out.write(content.c_str(),content.size());
            out.close();
            return true;
        }

        // 第三个参数为是否保留\n
        static bool ReadFile(const std::string &target,std::string *content,bool keep = false){
            (*content).clear();//以防万一先提前清空
            std::ifstream in(target);
            if(!in.is_open()){
                return false;
            }
            std::string line;
            while (std::getline(in,line)){
                (*content) += line;
                (*content) += (keep ? "\n" : "");
            }
            in.close();
            return true;
        }
    };

    class StringUtil
    {
    public:
        /*************************************
         * str: 输入型，目标要切分的字符串
         * target: 输出型，保存切分完毕的结果
         * sep: 指定的分割符
         * **********************************/
        static void SplitString(const std::string &str, std::vector<std::string> *target, const std::string &sep)
        {
            //boost split
            boost::split((*target), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
}