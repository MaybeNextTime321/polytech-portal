#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>

using namespace std;
using namespace cv;

bool pixelIsWhite(cv::Vec<unsigned char, 3>& currentCell)
{
   return currentCell[0] == currentCell[1] && currentCell[1] == currentCell[2] && 
   (int)currentCell[0] >= 233;
}

bool pixelIsYellow(cv::Vec<unsigned char, 3>& currentCell)
{
   Vec3b new_color;
   new_color.val[0] = 0;
   new_color.val[1] = 255;
   new_color.val[2] = 255;

   return currentCell == new_color; 
}

bool pixelIsGrey(cv::Vec<unsigned char, 3>& currentCell)
{
    return currentCell[0] == currentCell[1] && currentCell[1] == currentCell[2] && 
   (int)currentCell[0] >= 219 && (int)currentCell[0] <= 237;
}

bool GreyPixelAround(cv::Vec<unsigned char, 3> currentCell, cv::Vec<unsigned char, 3> firstPreviewCell, 
cv::Vec<unsigned char, 3> secondPreviewCell,cv::Vec<unsigned char, 3> firstNextCell,cv::Vec<unsigned char, 3> secondNextCell)
{

   bool nextPixelsIsGray = (pixelIsGrey(firstNextCell) || pixelIsYellow(firstNextCell))  && 
   (pixelIsGrey(secondNextCell) || pixelIsYellow(secondNextCell));

   bool previewPixelsIsGrey =  (pixelIsGrey(firstPreviewCell) || pixelIsYellow(firstPreviewCell))  && 
   (pixelIsGrey(secondPreviewCell) || pixelIsYellow(secondPreviewCell));

   bool nextAndPreview = (pixelIsGrey(firstPreviewCell) || pixelIsYellow(firstPreviewCell))  &&
   (pixelIsGrey(firstNextCell) || pixelIsYellow(firstNextCell));

   return (nextPixelsIsGray ||  previewPixelsIsGrey|| nextAndPreview) ;
} 

void GenerateAndOutputImage(const cv::Range rows, const cv::Range cols, std::string output_name, Mat& I)
{
   output_name += ".jpg";
   Mat rez = I(rows,cols);
   cv::imwrite(output_name,rez);
}

Mat& ScanImageAndReduceGreyToYellow(Mat& I, const uchar* const table)
{
    CV_Assert(I.depth() == CV_8U);
   std::set<int> line_change;
   std::vector<int> only_yellow_line;
   Vec3b new_color;
   new_color.val[0] = 0;
   new_color.val[1] = 255;
   new_color.val[2] = 255;

   for(int i = 2; i < I.rows-2;++i)
   { 
      bool change_current_line_color = true;
      for(int j = 0; j< I.cols;++j)
      {
         cv::Vec<unsigned char, 3> pixel = I.at<Vec3b>(i,j);

         if(pixelIsWhite(pixel))
            continue;

         if(change_current_line_color &&
            !GreyPixelAround(I.at<Vec3b>(i,j),I.at<Vec3b>(i-1,j),
            I.at<Vec3b>(i-2,j),I.at<Vec3b>(i+1,j),I.at<Vec3b>(i+2,j)) && pixelIsGrey(pixel))
         {
            I.at<Vec3b>(i,j) = new_color;
            line_change.insert(i);
         }

      }

   }

   int last_line = -1;
   for(int i : line_change)
   {
      int counter = 0; 
      for( int j = 0; j < I.cols;++j)
      {
         cv::Vec<unsigned char, 3> pixel = I.at<Vec3b>(i,j);
         
         if(pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 255 )
            ++counter;
      }
      if(counter >= I.cols * 0.7)
      {
         if(last_line == -1 || (i - last_line) > 5 )
         {
            only_yellow_line.push_back(i);
         }

         last_line = i;
      }
   }

   last_line = 0;
   for (int i = 0; i < only_yellow_line.size();++i)
   {
      std::string name = ("Result " + std::to_string(i));
      cv::Range rows (last_line,only_yellow_line[i]);
      cv::Range cols (0,I.cols);
      GenerateAndOutputImage(rows,cols,name,I);
      last_line = only_yellow_line[i];
   }

   std::string name = "Result " + std::to_string(only_yellow_line.size());
   cv::Range rows (last_line,I.rows);
   cv::Range cols (0,I.cols);
   GenerateAndOutputImage(rows,cols,name,I);
    return I;
}

int main( int argc, char* argv[])
{
   
    if (argc <= 1)
    {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    Mat I, J;

   I = imread(argv[1], IMREAD_COLOR);

    if (I.empty())
    {
        cout << "The image" << argv[1] << " could not be loaded." << endl;
        return -1;
    }

    uchar table[256];
    
   cv::Mat clone_i = I.clone();
   J = ScanImageAndReduceGreyToYellow(clone_i, table);
   
   cv::imshow("test",J);
   cv::waitKey(); 
   
   return 0;
}



