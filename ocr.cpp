#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>

int main()
{
	tesseract::TessBaseAPI tess;
	tess.Init(NULL,"eng+rus");
	
	tess.SetPageSegMode(tesseract::PSM_AUTO);

	Pix * image = pixRead("Result 2.jpg");
	tess.SetImage(image);

	tess.Recognize(0);
	std::string text = tess.GetUTF8Text();

	std::cout <<text;

	pixDestroy(&image);

	return 0;
}