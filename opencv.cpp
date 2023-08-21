#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

Mat& ScanImageAndReduceGresToYellow(Mat& I, const uchar* const table)
{
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch(channels)
    {
    case 1:
        {
            MatIterator_<uchar> it, end;
            for( it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
                *it = table[*it];
            break;
        }
    case 3:
        {
            MatIterator_<Vec3b> it, end,test;
            for( it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
            {
               bool same_pixel_down = false;

               if ((*it)[0] == (*it)[1] && (*it)[1] == (*it)[2] && (*it)[0] >= 221 &&(*it)[0]<=227)
               {
                  (*it)[0] = 0;
                  (*it)[1] = 255;
                  (*it)[2] = 255;
               }

            }
        }
    }

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
   J = ScanImageAndReduceGresToYellow(clone_i, table);
   

   cv::imshow("test",J);
   cv::waitKey(); 
   return 0;
}



