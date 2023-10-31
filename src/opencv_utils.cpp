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

cv::Mat getContours(cv::Mat mat, int threshold, int threshold2Offset, int blurNum, cv::Mat &dst)
{
	cv::Mat src = mat.clone();
    Mat canny_output;
    RNG rng(12345);
//    int thresh = 100;

    Canny( mat, canny_output, threshold, threshold2Offset + threshold );
	vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
	Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	Mat drawing2 = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
		drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
	}
//	int iterations = 10;
	bitwise_not(mat, mat);

	vector<vector<Point>> contours2;
	findContours( mat, contours2, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
	for (size_t i = 0; i < contours2.size(); ++i)
	{
		Scalar color = Scalar( 255, 0, 0);
		drawContours( dst, contours2, (int)i, color, 2, LINE_8, hierarchy, 0 );
	}
	erode(mat, mat, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(blurNum,blurNum)), cv::Point(-1,-1));
	contours2.clear();

	findContours( mat, contours2, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

	for (size_t i = 0; i < contours2.size(); ++i)
	{
		Scalar color = Scalar( 0, 0, 255 );
		drawContours( dst, contours2, (int)i, color, 2, LINE_8, hierarchy, 0 );
	}

	bitwise_not(mat, mat);

	imshow( "Contours", drawing );
	imshow( "Contours2", dst );
	imshow( "Mat", mat );

    return mat;
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

double getMeanByMask(const cv::Mat &image, const cv::Mat &mask)
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

double getRefractionByMask(cv::Mat &mat, cv::Mat matBack, cv::Mat mask)
{
    double result = 0;
    int count = 0;
    for (int i = 0; i < mat.rows; ++i)
    {
        for (int j = 0; j < mat.cols; ++j)
        {
            int maskVal = mask.at<Vec<uchar, 1>>(i, j)[0];
            if (maskVal != 255 && matBack.at<Vec<uchar, 1>>(i, j)[0] != 0)
            {
//                if (matBack.at<Vec<uchar, 1>>(i, j)[0] == 0)
//                    result += mat.at<Vec<uchar, 1>>(i, j)[0] / 1.0;
//                else
                result += mat.at<Vec<uchar, 1>>(i, j)[0] / (double)matBack.at<Vec<uchar, 1>>(i, j)[0];
                ++count;
            }
        }
    }
    return count == 0 ? 0 : result / count;
}
