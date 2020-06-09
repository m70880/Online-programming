#pragma once 
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "tools.hpp"
#include "oj_log.hpp"

struct QUESTION{
    std::string id_;
    std::string name_;
    std::string path_;
    std::string star_;
};

class OjModel{
public:
    OjModel(){
        LoadQuestions("./config.cfg");
    }
    // 获取列表所有题目HTML
    bool GetAllQuestions(std::vector<QUESTION> *ques){
        for(const auto kv:model_map_){
            ques->push_back(kv.second);
        }
        sort(ques->begin(),ques->end(),[](const QUESTION &a,const QUESTION &b){
                return std::stoi(a.id_) < std::stoi(b.id_); 
                });
        return true;
    }

    // 获取单个题目HTML
    bool GetOneQuestionHtml(std::string& id,std::string *desc,std::string *header){
        auto iter = model_map_.find(id);
        if(iter == model_map_.end()){
            LOG(ERROR,"Not Find Question id is") << id << std::endl;
            return false;
        }
        bool ret = FileOper::RreadDataFromFile(DescPath(iter->second.path_),desc);

        if(!ret){
            LOG(ERROR,"Read desc failed!") << std::endl;
            return false;
        }

        ret = FileOper::RreadDataFromFile(HeaderPath(iter->second.path_),header);

        if(!ret){
            LOG(ERROR,"Read desc failed!") << std::endl;
            return false;
        }
        
        return true;
    }
    // 获取单个题目
    QUESTION GetOneQuestion(const std::string &id){
        auto iter = model_map_.find(id);
        if(iter == model_map_.end()){
            LOG(ERROR,"Get One Question") << std::endl;
        }
        return iter->second;
    }
    bool SplicingCode(const std::string user_code,const std::string &ques_id,std::string *code){
        //1.查找对应id的题目是否存在
        auto iter = model_map_.find(ques_id);   
        if(iter == model_map_.end()){
            LOG(ERROR,"Can Not Find Question id is :") << ques_id <<std::endl;
            return false;
        }
        std::string tail;
        bool ret = FileOper::RreadDataFromFile(TailPath(iter->second.path_),&tail);
        if(!ret){
            LOG(ERROR,"Open tail is failed!") << std::endl;
            return false;
        }

        *code = user_code + tail;
        return true;
    }

private:
    std::string TailPath(const std::string &ques_path){
        return ques_path + "/tail.cpp";
    }
    std::string DescPath(const std::string &ques_path){
        return ques_path + "/desc.txt";
    }
    std::string HeaderPath(const std::string &ques_path){
        return ques_path + "/header.cpp";
    }
    
    bool LoadQuestions(const std::string &config_path){
        std::ifstream file(config_path.c_str());
        if(!file.is_open()){
            return false;
        }
        std::string line;
        while(getline(file,line)){
            std::vector<std::string> vs;
            StringTools::split(line," ",&vs);
            if(vs.size() != 4){
                perror("split");
                continue;
            }
            QUESTION q;
            q.id_ = vs[0];
            q.name_ = vs[1];
            q.path_ = vs[2];
            q.star_ = vs[3];
            model_map_[q.id_] = q;
        }
        file.close();
        return true;
    }

private:
    std::unordered_map<std::string,QUESTION> model_map_;
};
