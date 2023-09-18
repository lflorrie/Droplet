#include "opencv_utils.h"
#include <vector>

using namespace std;
using namespace cv;

cv::Mat qimage_to_mat_ref(QImage &img, int format)
{
    return cv::Mat(img.height(), img.width(), format, img.bits(), img.bytesPerLine());
}
QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows, format);
}

Mat getContours(cv::Mat mat, int threshold, int threshold2Offset)
{
    Mat canny_output;
    RNG rng(12345);
//    int thresh = 100;

    Canny( mat, canny_output, threshold, threshold2Offset + threshold );
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }
    imshow( "Contours", drawing );
    imshow( "Mat", mat );

    return drawing;
}

void imageSubtraction(cv::Mat &a, cv::Mat &b, int thresh, int blurNum)
{
    blur( a, a, Size(blurNum,blurNum) );
    blur( b, b, Size(blurNum,blurNum) );
    for (int i = 0; i < a.rows && i < b.rows; i++)
    {
        for (int j = 0; j < a.cols && j < b.cols; j++)
        {
            int diff1 = abs(a.at<Vec<uchar, 1>>(i, j)[0] - b.at<Vec<uchar, 1>>(i, j)[0]);
//            int diff2 = abs(a.at<Vec3b>(i, j)[1] - b.at<Vec3b>(i, j)[1]);
//            int diff3 = abs(a.at<Vec3b>(i, j)[2] - b.at<Vec3b>(i, j)[2]);

            if ((diff1 < thresh /*|| diff2 < thresh || diff3 < thresh*/ ))
            {
                a.at<Vec<uchar, 1>>(i, j)[0] = 255;
//                a.at<Vec3b>(i, j)[1] = 0;
//                a.at<Vec3b>(i, j)[2] = 0;
            }
        }
    }
}

float getMeanByMask(const cv::Mat &image, const cv::Mat &mask)
{
    float result = 0;
    int count = 0;
    for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            if (mask.at<Vec<uchar, 1>>(i, j)[0] != 255)
            {
                result += image.at<Vec<uchar, 1>>(i, j)[0];
                ++count;
            }
        }
    }
    return count == 0 ? 0 : result / count;
}

float getRefractionByMask(cv::Mat &mat, cv::Mat matBack, cv::Mat mask)
{
    float result = 0;
    int count = 0;
    for (int i = 0; i < mat.rows; ++i)
    {
        for (int j = 0; j < mat.cols; ++j)
        {
            if (mask.at<Vec<uchar, 1>>(i, j)[0] != 255)
            {
                result += mat.at<Vec<uchar, 1>>(i, j)[0] / (float)matBack.at<Vec<uchar, 1>>(i, j)[0];
                ++count;
            }
        }
    }
    return count == 0 ? 0 : result / count;
}
