#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>

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

	tess.Recognize(0);
	std::string text = tess.GetUTF8Text();

	std::cout <<text;

	pixDestroy(&image);

	return 0;
}