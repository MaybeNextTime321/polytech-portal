#include <iostream>
#include <fstream>
#include <string>
#include <htmlcxx/html/ParserDom.h>
#include <boost/algorithm/string.hpp>

std::string getQuestionFromContent(const std::string & content ){

    std::string startTag = "problem-progress"; // Первая фраза для поиска
    std::string endTag = "problem-progress";   // Последняя фраза для поиска

    size_t startIndex = content.find(startTag);
    size_t endIndex = content.rfind(endTag);

        if (startIndex != std::string::npos && endIndex != std::string::npos && startIndex < endIndex) {
            
            std::string pre_result = content.substr(startIndex + startTag.length(), endIndex - (startIndex + startTag.length()));
            
            size_t firstIndex = pre_result.find("&gt;");
            size_t secondIndex = pre_result.find("&lt;");

            if (firstIndex != std::string::npos && secondIndex != std::string::npos) {

                std::string final_result = pre_result.substr(firstIndex + 4, secondIndex - (firstIndex + 4));
                final_result.erase(0, 3);
                return final_result;
            } 

            else {
                return "";
        }

    } 

    else {
        return "";
    }
}

std::string getSingleRatioAnswer(const std::string &content){

    std::string answer_block = content;
    
    size_t start = answer_block.find("input-radio submitted");
    size_t end = answer_block.find("&lt;/label&gt;", start);

    if (start != std::string::npos && end != std::string::npos) {
       
        std::string answer = answer_block.substr(start, end - start);

        size_t erase_index = answer.rfind("&gt; ");

        if(erase_index!= std::string::npos){
            answer = answer.substr(erase_index + 4);
            std::cout << "Answer: " << answer << std::endl;
            return answer;
        }

        else{
            return "";
        }

    } else {
        std::cout << "Start or end substring not found" << std::endl;
        return "";
    }


}

void parse_node(const auto &it){

    double data_problem_score = std::stod(it->attribute("data-problem-score").second);
    double data_problem_total_possible = std::stod(it->attribute("data-problem-total-possible").second);
    std::cout << "Score value: " << data_problem_score << ", max: " << data_problem_total_possible << std::endl;
    
    std::string full_content = it->attribute("data-content").second;
    
    std::string question = getQuestionFromContent(full_content);

    if(question != ""){
        std::cout << "Question is: " << question << std::endl;
    }
    else{
        std::cerr<< "Cant't parse question from data: " << full_content << std::endl;
    }

    std::vector<std::string> answer;
    size_t question_type_index = full_content.find("input type=&#34;radio");

    if(question_type_index != std::string::npos){
        std::cout << "Question is single radio" << std::endl;
        answer.push_back(getSingleRatioAnswer(full_content));
    }
    else{
        std::cout << "Question is not single radio, no parse answer now " << std::endl;
    }



}

int main() {
   

    std::ifstream htmlFile("example4.html");
    if (!htmlFile.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return 1;
    }

    std::string htmlContent((std::istreambuf_iterator<char>(htmlFile)), std::istreambuf_iterator<char>());

    
    htmlcxx::HTML::ParserDom parser;
    std::string div_content;
    tree<htmlcxx::HTML::Node> dom = parser.parseTree(htmlContent);

     for (auto it = dom.begin(); it != dom.end(); ++it) {
            if (it->tagName() == "div") {
               
                it->parseAttributes();

                std::string class_name = it->attribute("class").second;

                if(class_name == "problems-wrapper"){
                    parse_node(it);
        
                }
               
            }
        }

  
    return 0;
}
