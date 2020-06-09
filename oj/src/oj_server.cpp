#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>
#include "httplib.h"

#include "oj_view.hpp"
#include "oj_model.hpp"
#include "oj_log.hpp"
#include "compile.hpp"

using namespace httplib;
int main(){
    Server svr;
    OjModel ojmodel; 
    svr.Get("/all",[&ojmodel](const Request& req,Response& resp){
            //获取题目列表
            std::vector<QUESTION> ques;
            ojmodel.GetAllQuestions(&ques);
            //渲染界面
            std::string html;
            OjView::ExpandAllQuestionsHtml(&html,ques);
            //响应页面
            resp.set_content(html,"text/html;charset=UTF-8");

            });
    // 获取单个题目信息描述，并返回界面

    svr.Get(R"(/question/(\d+))",[&ojmodel](const Request& req,Response& resp){
            //  查看matches
            printf("path:%s\n",req.path.c_str());
            LOG(INFO,"req.matches") << req.matches[0] << ":" <<req.matches[1] << std::endl;

            // 1.获取id 
            std::string id = req.matches[1];

            // 2.在题目路径下加载，题目描述信息
            std::string desc;
            std::string header;
            ojmodel.GetOneQuestionHtml(id,&desc,&header);

            // 3.并组织好返回给浏览器
            std::string html;
            OjView::ExpendOneQuestionHtml(ojmodel.GetOneQuestion(id),desc,header,&html);
            resp.set_content(html,"text/html;charset=UTF-8");
                    
            });
    // 提交代码
    svr.Post(R"(/question/(\d+))",[&ojmodel](const Request& req,Response& resp){
            std::unordered_map<std::string,std::string> pram;
            // 1.Url转码
            UrlUtil::PraseBody(req.body,&pram);
            
            // 2.编译&运行
            //  2.1 给提交的代码添加头文件(拼接头尾)，并测试
            std::string code;
            ojmodel.SplicingCode(pram["name"],req.matches[1],&code);
            // FileOper::WriteDataToFile(code,"./req_matches.cpp");
            // FileOper::WriteDataToFile(pram["name"],"./pram.cpp");
            // FileOper::WriteDataToFile(code,"./code.cpp");
            Json::Value req_jsons;
            req_jsons["name"] = code;

            Json::Value Resp_json;
            Compiler::ComplieAndRun(req_jsons,&Resp_json);

            //3.构造响应，jso
            std::string html;
            const std::string errorno= Resp_json["errorno"].asString();
            const std::string reason = Resp_json["reason"].asString();
            const std::string stdout_reason= Resp_json["stdout"].asString();
            OjView::ExpandReason(errorno,reason,stdout_reason,&html);
            resp.set_content(html,"text/html;charset=UTF-8");
            
    });

    LOG(INFO,"listen in  0.0.0.0 : 19999") << std::endl;
    LOG(INFO,"server ready") << std::endl;

    svr.listen("0.0.0.0",19999);

    return 0;
}

