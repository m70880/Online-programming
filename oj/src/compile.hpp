#pragma once
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "oj_log.hpp"
#include "json/json.h"
#include "tools.hpp"

enum Errorno{
    OK = 0,
    COMPILE_ERROR,
    PRAM_ERROR,
    INTERNAL_ERROR,
    RUN_ERROR
};

class Compiler{
    public:
        static void ComplieAndRun(Json::Value Req,Json::Value *Resp){
            // Resp:
            // 0:编译运行正确
            // 1.编译错误
            // 2.参数错误
            // 3.内存错误
            // 4.时间错误
            
            // 1.判空
            if(Req["name"].empty()){
                (*Resp)["errorno"] = PRAM_ERROR;
                LOG(ERROR,"Request is Empty") << std::endl;
                return;
            }

            // 2.将代码写到文件中，
            std::string code = Req["name"].asString();
            // 为了使多用户同时访问，需要给单个用户一个特定的处理(加上时间戳)
            // tmp_time
            std::string tmp_filename = WriteTmpFile(code);
            if(tmp_filename == ""){
                (*Resp)["errorno"] = INTERNAL_ERROR;
                (*Resp)["reson"] = "Write Tmp File is failed!";
                LOG(ERROR,"Write tmp_filename failed") << std::endl;
                return;
            }
            // 3.编译 tmp_filename.cpp
            if(!Compile(tmp_filename)){
                (*Resp)["errorno"] = COMPILE_ERROR;
                LOG(ERROR,"Compile Error ") << std::endl;
                return;
            }
            // 4.运行
            int sig = Run(tmp_filename);
            if(sig != 0){
                (*Resp)["errorno"] = RUN_ERROR;
                (*Resp)["reson"] = "Program exit by signal: " + std::to_string(sig);
                LOG(ERROR,"Run Error !") << std::endl;
            }
            // 5.构造响应
            (*Resp)["errorno"] = OK;
            (*Resp)["reason"] = "Compile and Run is okey!";
            // 标准输出
            std::string stdout_reason;
            FileOper::RreadDataFromFile(StdoutPath(tmp_filename),&stdout_reason);
            (*Resp)["stdout"] = stdout_reason;

            // 标准错误
            std::string stderr_reason;
            FileOper::RreadDataFromFile(StderrPath(tmp_filename),&stderr_reason);
            (*Resp)["stderr"] = stderr_reason;
            Clean(tmp_filename);
            return;
        }
    private:
        static bool Compile(const std::string &filename){
            // 1.构造编译命令-> g++ src -o [exec] -std=c++11
            const int commandcount = 20;
            char buf[commandcount][50] = {{0}};
            char* Command[commandcount] = {0};
            for(int i = 0;i < commandcount;i++){
                Command[i] = buf[i];
            }
            snprintf(Command[0],49,"%s","g++");
            snprintf(Command[1],49,"%s",SrcPath(filename).c_str());
            snprintf(Command[2],49,"%s","-o");
            snprintf(Command[3],49,"%s",ExePath(filename).c_str());
            snprintf(Command[4],49,"%s","-std=c++11");
            Command[5] = NULL;
            // 2.创建子进程
            //  2.1 父进程 -> 等待子进程退出
            //  2.2 子进程 -> 进程替换 g++
            int pid = fork();
            if(pid < 0){
                LOG(ERROR,"Create child is failed!") << std::endl;
                return false;
            }
            else if(pid == 0){
                // child
                int fd = open(StderrPath(filename).c_str(),O_CREAT | O_RDWR,0664);
                if(fd < 0){
                    LOG(ERROR,"open Complie errorfile failed") << std::endl;
                    exit(0);
                }
                // 重定向
                dup2(fd,2);
                execvp(Command[0],Command);   
                LOG(ERROR,"execvp failed") << std::endl;
                return false;
                exit(0);
            }
            else{
                //father
                waitpid(pid,NULL,0);
            }
            // 3. 验证是否产生可执行结果
            struct stat st;
            int ret = stat(ExePath(filename).c_str(),&st);
            if(ret < 0){
                LOG(ERROR,"Compile ERROR! sec filename is ") << ExePath(filename) << std::endl;
                return false;
            }
            // 4.
            return true;
        }
        // 运行
        static int Run(const std::string filename){

            // 可执行程序
            // 1.创建子进程
            //   父进程：进程等待
            //   子进程：进程替换
            //      输出：重定向到文件
            //      错误：重定向到文件
            int pid = fork();
            if(pid < 0){
                // 失败
                LOG(ERROR,"Exec pragma failed! Create childe process failed") << std::endl;
                return -1;
            }
            else if(pid == 0){
                // child
                // 输出：重定向到文件
                int stdout_fd = open(StdoutPath(filename).c_str(),O_CREAT | O_RDWR,0664);
                if(stdout_fd < 0){
                    LOG(ERROR,"Open stdout is failed") << std::endl;
                    return -1;
                }
                dup2(stdout_fd,1);
                // 错误：重定向到文件
                int stderr_fd = open(StdoutPath(filename).c_str(),O_CREAT | O_RDWR,0664);
                if(stderr_fd < 0){
                    LOG(ERROR,"Open stderr is failed") << std::endl;
                    return -1;
                }
                dup2(stderr_fd,2);
                // 内存限制
                struct rlimit rl;
                rl.rlim_cur = 1024 * 30 * 1000;
                rl.rlim_max = RLIM_INFINITY;
                setrlimit(RLIMIT_AS, &rl);

                // 时间限制
                alarm(1);

                execl(ExePath(filename).c_str(),ExePath(filename).c_str(),NULL);
                exit(EXIT_FAILURE);
            }
            //father
            int status = -1;
            waitpid(pid,&status,0);
            // 将受到信号的信息返回给调用者，如果测试
            // 低7位位信号标志 7f -> 3+4;
            // 返回0，运行正常，否则收到异常结束信号
            return status & 0x7f;
        }
        static std::string WriteTmpFile(const std::string &code){
            std::string tmp_filename = "tmp_" + std::to_string(LogTime::GetTimeStamp());
            // 写文件
            bool ret = FileOper::WriteDataToFile(code,SrcPath(tmp_filename));
            if(!ret){
                LOG(ERROR,"Write tmp_filename is failed : ") << tmp_filename << std::endl;
            }
            return tmp_filename;
        }
        static void Clean(const std::string &filename){
            FileOper::Clean(ExePath(filename));
            FileOper::Clean(StdoutPath(filename));
            FileOper::Clean(StderrPath(filename));
            FileOper::Clean(SrcPath(filename));
        }
        static std::string SrcPath(const std::string &filename){
            return "./tmp_files/" + filename + ".cpp";
        }
        static std::string ExePath(const std::string &filename){
            return "./tmp_files/" + filename + ".executable";
        }
        static std::string StdoutPath(const std::string &filename){
            return "./tmp_files/" + filename + ".stdout";
        }
        static std::string StderrPath(const std::string &filename){
            return "./tmp_files/" + filename + ".stderr";
        }
};
