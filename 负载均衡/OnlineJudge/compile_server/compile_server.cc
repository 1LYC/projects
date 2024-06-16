#include "compile_run.hpp"
#include "../comm/httplib.h"// 引入

using namespace ns_compile_and_run;
using namespace httplib;// 引入

void Usage(std::string proc){
    std::cerr << "Usage: " << "\n\t" << proc << " port" << std::endl;
}

//./copile_server port
int main(int argc,char *argv[])
{
    if(argc != 2){
        Usage(argv[0]);
        return 1;
    }
    Server svr;
    svr.Get("/hello",[](const Request &req, Response &resp){
        // 用来进行基本测试
        resp.set_content("hello httplib,你好 httplib!", "text/plain;charset=utf-8");
    });
    svr.Post("/compile_and_run",[](const Request&req,Response & resp){
        // 用户请求的服务正文是我们想要的json string
        std::string in_json = req.body;
        //std::cout << in_json << std::endl;
        std::string out_json;
        if(!in_json.empty()){
            CompileAndRun::Start(in_json,&out_json);
            resp.set_content(out_json,"application/json;charset=uft-8");
        }
    });
    svr.listen("0.0.0.0",atoi(argv[1]));
    return 0;
}


