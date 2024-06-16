#include <iostream>
#include "../comm/httplib.h"// 引入
#include "oj_control.hpp"

using namespace httplib;// 引入
using namespace ns_control;

using namespace httplib;
using namespace ns_control;

static Control *ctrl_ptr = nullptr;

void Recovery(int signo)
{
    ctrl_ptr->RecoveryMachine();
}


int main()
{
    // 注册信号,发送sigquit信号时,执行Recovery回调函数
    signal(SIGQUIT, Recovery);

    // 用户请求的服务器路由功能
    Server svr;
    Control ctrl;
    ctrl_ptr = &ctrl;
    
    // 获取所有的题目列表
    svr.Get("/all_questions",[&ctrl](const Request&req,Response &resp){
        // 返回一张包含所有题目的html网页
        std::string html;// 待处理
        ctrl.AllQuestions(&html);
        resp.set_content(html,"text/html;charset=utf-8");
    });

    // 根据题目编号,获取题目内容
    // \d+ 是正则表达式的特殊符号
    svr.Get(R"(/question/(\d+))",[&ctrl](const Request&req,Response &resp){
        std::string number = req.matches[1];
        std::string html;
        ctrl.Question(number,&html);
        resp.set_content(html,"text/html;charset=utf-8");
    });

    // 判断用户提交的代码(1.每道题c测试用例,2.compile_and_run)
    svr.Post(R"(/judge/(\d+))",[&ctrl](const Request&req,Response &resp){
        std::string number = req.matches[1];
        std::string result_json;
        ctrl.Judge(number,req.body,&result_json);
        resp.set_content(result_json,"application/json;charset=utf-8");
    });
    svr.set_base_dir("./wwwroot");
    svr.listen("0.0.0.0",8080);
    return 0;
}

