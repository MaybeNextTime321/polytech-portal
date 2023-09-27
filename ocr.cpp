#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include <sstream> 
#include <fstream>

bool questionIsChoiced(const std::string& question, const std::vector<std::string> &correct_symbols){
	bool result = false;

	for(std::string symbol : correct_symbols){
		size_t position = question.find(symbol);
		if(position != std::string::npos){
			result = true;
			break;
		}
	}

	return result;
}

bool questionIsGraded(double& precentage, const std::string& question){

	size_t space,slash;
	precentage = -1;
	space = question.find(' ');
	slash = question.find('/');

	if(space != std::string::npos && slash != std::string::npos){
		precentage = std::stod(question.substr(0,slash)) / std::stod(question.substr(slash+1,space-(slash+1)));
	}

	return (space != std::string::npos && slash != std::string::npos);
}

int main(int argc, char* argv[]){

    if (argc <= 1){

        std::cout << "Not enough parameters" << std::endl;
        return -1;
    }

	tesseract::TessBaseAPI tess;
	tess.Init(NULL,"eng+rus");

	tess.SetPageSegMode(tesseract::PSM_AUTO);

	Pix * image = pixRead(argv[1]);
	tess.SetImage(image);
	pixDestroy(&image);

	tess.Recognize(0);
	std::string text = tess.GetUTF8Text();


	std::string question = "";

	std::stringstream ss(text);
	std::string new_line_text;

	std::vector<std::string> answer;
	std::vector<std::string> choiced_answer;

	text = "";
	while(std::getline(ss,new_line_text,'\n')){

		if(new_line_text.length() != 0){
			text += new_line_text;
		}
		else{
			if(question.length() == 0){
				question = text;
			}
			else{
				answer.push_back(text);
			}

			text = "";
		}
	}

	std::cout << "Question is: " << question << std::endl;
	
	double precentage = 0;
	if(questionIsGraded(precentage, answer[0]))
	{
		std::cout << "Question is graded, grade is: " << precentage*100 << "%" << std::endl;
	}
	else
	{
		std::cout << "Can't recognize image grade" << std::endl;
	}
	std::vector<std::string> correct_symbols {"@", "©", "С"};

	std::cout << "Choices is: " << std::endl;
	for(int i = 1; i < answer.size(); ++i)
	{
		if(questionIsChoiced(answer[i],correct_symbols))
		{
			size_t position = answer[i].find(" ");
			std::cout << answer[i].substr(position+1,answer[i].length()-position) << std::endl;
			choiced_answer.push_back(answer[i]);
		}
	}

	return 0;
}