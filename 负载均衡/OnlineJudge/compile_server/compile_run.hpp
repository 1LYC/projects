#pragma once

#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include <jsoncpp/json/json.h>

namespace ns_compile_and_run
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompileAndRun
    {
    public:
        static void RemoveTempFile(const std::string &file_name){
            // 因为清理文件的个数是不确定的,但是总共有哪些文件,这是我们知道的
            // 则清理文件之前还需要判断是否存在
            std::string _src = PathUtil::Src(file_name);
            if(FileUtil::IsFileExists(_src)) unlink(_src.c_str());

            std::string _compiler_error = PathUtil::CompilerError(file_name);
            if(FileUtil::IsFileExists(_compiler_error)) unlink(_compiler_error.c_str());

            std::string _execute = PathUtil::Exe(file_name);
            if(FileUtil::IsFileExists(_execute)) unlink(_execute.c_str());

            std::string _stdin = PathUtil::Stdin(file_name);
            if(FileUtil::IsFileExists(_stdin)) unlink(_stdin.c_str());

            std::string _stdout = PathUtil::Stdin(file_name);
            if(FileUtil::IsFileExists(_stdout)) unlink(_stdout.c_str());
            
            std::string _stderr = PathUtil::Stdin(file_name);
            if(FileUtil::IsFileExists(_stderr)) unlink(_stderr.c_str());
        }
        // 得到错误原因字符串
        // 待完善
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "提交代码是空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                // 这个是代码编译时发生了错误
                FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                break;
            case SIGABRT: // 6号信号
                desc = "内存超过范围";
                break;
            case SIGXCPU: // 24号信号
                desc = "CPU使用超时";
                break;
            case SIGFPE: // 8号信号
                desc = "浮点数溢出";
                break;
            default:
                desc = "未知: " + std::to_string(code);
                break;
            }
            return desc;
        }

        // in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
        // out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
        static void Start(const std::string &in_json, std::string *out_json)
        {
            // step1: 反序列化过程
            Json::Value in_value;
            Json::Reader reader;
            // 把in_json中的数据写到in_value中
            reader.parse(in_json, in_value); // 最后再处理差错问题

            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            Json::Value out_value;
            int status_code = 0;
            int run_result = 0;
            std::string file_name; // 唯一文件名
            if (code.size() == 0)
            {
                status_code = -1; // 代码为空
                goto END;
            }
            // 形成的文件名只居有唯一性,没有目录没有后缀
            // 使用: 毫秒级时间戳 + 原子性递增唯一值 : 来保证唯一性
            file_name = FileUtil::UniqFileName();
            // 形成临时的src文件
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status_code = -2; // 未知错误
                goto END;
            }

            if (!Compiler::Compile(file_name))
            {
                status_code = -3; // 编译错误
                goto END;
            }
            run_result = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_result < 0)
            {
                // runnem模块内部错误
                status_code = -2; // 未知错误
            }
            else if (run_result > 0)
            {
                // 程序运行崩溃
                status_code = run_result; // 这里的run_result是信号
            }
            else
            {
                // 运行成功
                status_code = 0;
            }
        END:
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name); // 得到错误信息字符串
            if (status_code == 0)
            {
                // 整个过程全部成功
                std::string _stdout;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true);
                out_value["stdout"] = _stdout;

                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stderr, true);
                out_value["stdout"] = _stdout;
            }

            // step2: 序列化
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            RemoveTempFile(file_name);// 清理临时文件
        }
    };

}