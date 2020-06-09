#include <ctemplate/template.h>
#include <vector>
#include <string>
#include "oj_model.hpp"

class OjView{
public:
    static void ExpandAllQuestionsHtml(std::string* html,std::vector<QUESTION>& ques){
        // all_question 随便起名，不影响，只是个代号
        ctemplate::TemplateDictionary dict("all_questions");
        for(auto que : ques){
            ctemplate::TemplateDictionary* section_dict = dict.AddSectionDictionary("questions");
            section_dict->SetValue("id",que.id_);
            section_dict->SetValue("id",que.id_);
            section_dict->SetValue("name",que.name_);
            section_dict->SetValue("star",que.star_);
        }
        ctemplate::Template *t1 = ctemplate::Template::GetTemplate("./template/all_questions.html",ctemplate::DO_NOT_STRIP);
        t1->Expand(html,&dict);
    }
    //id name star desc header ==> string html
    static bool ExpendOneQuestionHtml(const QUESTION &ques,const std::string &desc,const std::string &header, std::string *html){
        ctemplate::TemplateDictionary dict("question");
        dict.SetValue("id",ques.id_);
        dict.SetValue("name",ques.name_);
        dict.SetValue("star",ques.star_);
        dict.SetValue("desc",desc);
        dict.SetValue("header",header);

        ctemplate::Template* tp1 = ctemplate::Template::GetTemplate("./template/code.html",ctemplate::DO_NOT_STRIP);
        if(!tp1){
            LOG(ERROR,"Expend One Question Html") <<std::endl;
        }
        tp1->Expand(html,&dict);
        return true;
    }
    static void ExpandReason(const std::string &errorno,const std::string &reason,const std::string &stdout_reason,std::string *html){
        ctemplate::TemplateDictionary dict("reason");
        dict.SetValue("errorno",errorno);
        dict.SetValue("reason",reason);
        dict.SetValue("stdout",stdout_reason);
        ctemplate::Template* tp1 = ctemplate::Template::GetTemplate("./template/reason.html",ctemplate::DO_NOT_STRIP);
        tp1->Expand(html,&dict);
    }
};

