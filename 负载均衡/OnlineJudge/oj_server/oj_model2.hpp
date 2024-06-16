#pragma once
// 文件版本
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include <unordered_map>
#include <cassert>
#include <vector>

#include "./include/mysql.h"

namespace ns_model
{
    using namespace std;
    using namespace ns_log;
    using namespace ns_util;

    struct Question{
        string number;// 题目编号,唯一
        string title;// 题目标题
        string star;// 难度: 简单 中等 困难
        
        int cpu_limit;// 题目的时间复杂度(S)
        int mem_limit;// 题目的空间复杂度(KB)
        
        string desc;// 题目描述
        string header; // 题目预设给用户在线编辑器的代码
        string tail;// 题目测试用例,需要和header拼接
    };    

    const std::string oj_questions = "oj_table";
    const std::string host = "127.0.0.1";
    const std::string user = "oj_client";
    const std::string passwd = "123456";
    const std::string db = "oj";
    const int port = 3306;

    class Model
    {
    public:
        Model(){
        }
        ~Model(){
            ;
        }
        bool QueryMysql(const std::string &sql,vector<Question>*out){
            // 这里的out是输出型参数
            // 创建mysql句柄
            MYSQL *my = mysql_init(nullptr);
            // 连接数据库
            if(nullptr == mysql_real_connect(my,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,nullptr,0)){
                LOG(FATAL) << "连接数据库失败!" << "\n";
                return false;
            }

            // 一定要设置该链接的编码格式,要不然会出现乱码的问题
            mysql_set_character_set(my,"utf8");
            LOG(INFO) << "连接数据库成功!" << "\n";

            // 执行sql语句
            if(0 != mysql_query(my,sql.c_str())){
                LOG(WARNING) << sql << " execute error! " << "\n";
                return false;
            }

            // 提取结果
            MYSQL_RES *res = mysql_store_result(my);// 本质就是一个2级指针

            // 分析结果
            int rows = mysql_num_rows(res);// 获取行的数量
            int cols = mysql_num_fields(res);// 获取列的数量

            Question q;
            for(int i = 0;i < rows;i++){
                MYSQL_ROW row = mysql_fetch_row(res);
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpu_limit = atoi(row[6]);
                q.mem_limit = atoi(row[7]);
                out->push_back(q);
            }
            if(res){
                // 释放控件
                free(res);
            }
            
            // 关闭mysql连接
            mysql_close(my);

            return true;
        }
        // 获取所有题目,这里的out是输出型参数
        bool GetAllQuestions(vector<Question>*out){
            std::string sql = "select * from ";
            sql += oj_questions;
            return QueryMysql(sql,out);
        }

        // 获取指定题目,这里的q是输出型参数
        bool GetOneQuestion(const string& number,Question* q){
           bool res = false;
           std::string sql = "select * from ";
           sql += oj_questions;
           sql += " where number=";
           sql += number;

           vector<Question> result;
           if(QueryMysql(sql,&result)){
                if(result.size() == 1){
                    *q = result[0];
                    res = true;
                }
           }

           return res;
        }

        
    private:
        // 题号 : 题目细节
        unordered_map<string,Question> questions;
    };
} 

