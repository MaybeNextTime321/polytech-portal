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

bool PixelIsWhite(cv::Vec<unsigned char, 3>& current_cell){

   return current_cell[0] == current_cell[1] && current_cell[1] == current_cell[2] && 
   (int)current_cell[0] >= 233;
}

bool PixelIsYellow(cv::Vec<unsigned char, 3>& current_cell){

   Vec3b new_color;
   new_color.val[0] = 0;
   new_color.val[1] = 255;
   new_color.val[2] = 255;

   return current_cell == new_color; 
}

bool PixelIsGrey(cv::Vec<unsigned char, 3>& current_cell){

    return current_cell[0] == current_cell[1] && current_cell[1] == current_cell[2] && 
   (int)current_cell[0] >= 215 && (int)current_cell[0] <= 240;
}

bool GreyPixelAround(cv::Vec<unsigned char, 3> currentCell, cv::Vec<unsigned char, 3> firstPreviewCell, 
cv::Vec<unsigned char, 3> secondPreviewCell,cv::Vec<unsigned char, 3> firstNextCell,cv::Vec<unsigned char, 3> secondNextCell){

   bool nextPixelsIsGray = (PixelIsGrey(firstNextCell) || PixelIsYellow(firstNextCell))  && 
   (PixelIsGrey(secondNextCell) || PixelIsYellow(secondNextCell));

   bool previewPixelsIsGrey =  (PixelIsGrey(firstPreviewCell) || PixelIsYellow(firstPreviewCell))  && 
   (PixelIsGrey(secondPreviewCell) || PixelIsYellow(secondPreviewCell));

   bool nextAndPreview = (PixelIsGrey(firstPreviewCell) || PixelIsYellow(firstPreviewCell))  &&
   (PixelIsGrey(firstNextCell) || PixelIsYellow(firstNextCell));

   return (nextPixelsIsGray ||  previewPixelsIsGrey|| nextAndPreview) ;
} 

void GenerateAndOutputImage(const cv::Range rows, const cv::Range cols, std::string output_name, Mat& I){
   output_name += ".jpg";
   Mat rez = I(rows,cols);
   cv::imwrite(output_name,rez);
}

Mat& SplitToSingleQuestions(Mat& I, const uchar* const table){

   CV_Assert(I.depth() == CV_8U);
   std::set<int> line_change;
   std::vector<int> only_yellow_line;
   Vec3b new_color;
   new_color.val[0] = 0;
   new_color.val[1] = 255;
   new_color.val[2] = 255;

   for(int i = 2; i < I.rows-2;++i){ 

      bool change_current_line_color = true;
      for(int j = 0; j< I.cols;++j){

         cv::Vec<unsigned char, 3> pixel = I.at<Vec3b>(i,j);

         if(PixelIsWhite(pixel)){
            continue;
         }

         if(change_current_line_color &&
            !GreyPixelAround(I.at<Vec3b>(i,j),I.at<Vec3b>(i-1,j),
            I.at<Vec3b>(i-2,j),I.at<Vec3b>(i+1,j),I.at<Vec3b>(i+2,j)) && PixelIsGrey(pixel)){

            I.at<Vec3b>(i,j) = new_color;
            line_change.insert(i);
         }

      }

   }

   int last_line = -1;

   Vec3b grey_color;
   grey_color.val[0] = 227;
   grey_color.val[1] = 227;
   grey_color.val[2] = 227;
   for(int i : line_change){

      int counter = 0; 
      for( int j = 0; j < I.cols;++j){

         cv::Vec<unsigned char, 3> pixel = I.at<Vec3b>(i,j);
         
         if(pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 255){
            I.at<Vec3b>(i,j) = grey_color;
            ++counter;
         }

      }

      if(counter >= I.cols * 0.7){

         if(last_line == -1 || (i - last_line) > 5 ){

            only_yellow_line.push_back(i);
         }

         last_line = i;
      }
   }

   last_line = 0;
   for (int i = 0; i < only_yellow_line.size();++i){

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

int main(int argc, char* argv[]){
   
    if (argc <= 1){

        cout << "Not enough parameters" << endl;
        return -1;
    }

    Mat I, J;

   I = imread(argv[1], IMREAD_COLOR);

    if (I.empty()){

        cout << "The image" << argv[1] << " could not be loaded." << endl;
        return -1;
    }

    uchar table[256];
    
   cv::Mat clone_i = I.clone();
   J = SplitToSingleQuestions(clone_i, table);
   

   return 0;
}



