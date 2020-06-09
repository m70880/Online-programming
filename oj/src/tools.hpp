#pragma once 
#include <vector>
#include <unordered_map>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "oj_log.hpp"

class StringTools{
    public:
        static void split(const std::string &input,const std::string ch,std::vector<std::string> *output){
            boost::split(*output,input,boost::is_any_of(ch),boost::token_compress_off);
        }
};

//文件操作
class FileOper{
    public:
        static bool RreadDataFromFile(const std::string &FileName_Path,std::string *content){
            std::ifstream file(FileName_Path.c_str());
            if(!file.is_open()){
                LOG(ERROR,"Open file falied! file name is") << FileName_Path << std::endl;
                return false;
            }
            std::string line;
            while(std::getline(file,line)){
                *content += line + "\n";
            }
            file.close();
            return true;
        }
        static bool WriteDataToFile(const std::string &data,const std::string &filename){
            std::ofstream file(filename);
            if(!file.is_open()){
                LOG(ERROR,"Open is failed") << filename << std::endl;
                return false;
            }
            file.write(data.data(),data.size());
            file.close();
            return true;
        }
        static void Clean(const std::string &filename){
            unlink(filename.c_str());
        }


};
// Url解析
class UrlUtil{
    public:
        static void PraseBody(const std::string &body,std::unordered_map<std::string,std::string> *pram){
            std::vector<std::string> v;
            StringTools::split(body,"&",&v); 
            for(const auto &index : v){
                std::vector<std::string> vec;
                StringTools::split(index,"=",&vec);
                if(vec.size() != 2){
                    continue;
                }
                (*pram)[vec[0]] = UrlDecode(vec[1]);
            }

        }
    private:
        static unsigned char ToHex(unsigned char x) 
        { 
            return  x > 9 ? x + 55 : x + 48; 
        }

        static unsigned char FromHex(unsigned char x) 
        { 
            unsigned char y;
            if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
            else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
            else if (x >= '0' && x <= '9') y = x - '0';
            else assert(0);
            return y;
        }

        static std::string UrlEncode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (isalnum((unsigned char)str[i]) || 
                        (str[i] == '-') ||
                        (str[i] == '_') || 
                        (str[i] == '.') || 
                        (str[i] == '~'))
                    strTemp += str[i];
                else if (str[i] == ' ')
                    strTemp += "+";
                else
                {
                    strTemp += '%';
                    strTemp += ToHex((unsigned char)str[i] >> 4);
                    strTemp += ToHex((unsigned char)str[i] % 16);

                }

            }
            return strTemp;

        }

        static std::string UrlDecode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (str[i] == '+') strTemp += ' ';
                else if (str[i] == '%')
                {
                    assert(i + 2 < length);
                    unsigned char high = FromHex((unsigned char)str[++i]);
                    unsigned char low = FromHex((unsigned char)str[++i]);
                    strTemp += high*16 + low;

                }
                else strTemp += str[i];

            }
            return strTemp;

        }
};
