#pragma once
#include <sys/time.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

//当前实现的log服务也是在控制台进行输出
//输出的格式
//时间 日至登记 文件：行号 具体日志信息

class LogTime{
public:
    static int64_t GetTimeStamp(){
        struct timeval tv;
        gettimeofday(&tv,NULL);
        return tv.tv_sec;
    }

    static void GetTimeStamp(std::string *TimeStamp){
        time_t System;
        time(&System);
        struct tm* st= localtime(&System);
        char buf[30] = {0};
        sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",st->tm_year + 1900 ,st->tm_mon + 1,st->tm_mday,st->tm_hour,st->tm_min,st->tm_sec);
        TimeStamp->assign(buf,strlen(buf));
    }
};

const char* Level[]={
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
    "DEBUG"
};
enum LogLevel{
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    DEBUG
};
 std::ostream& Log(LogLevel lev,const char* file,int line,const std::string &logmsg){

    std::string level_info = Level[lev];
    std::string TimeStamp;

    LogTime::GetTimeStamp(&TimeStamp);
    std::cout <<"["<<TimeStamp<<" "<<level_info<<" "<<file<<":"<<line<<"]"<<logmsg;

    return std::cout;
}


#define LOG(lev,msg) Log(lev,__FILE__,__LINE__,msg)
