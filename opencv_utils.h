#ifndef OPENCV_UTILS_H
#define OPENCV_UTILS_H
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <QImage>

cv::Mat qimage_to_mat_ref(QImage &img, int format);

QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format);

cv::Mat getContours(cv::Mat mat, int threshold, int threshold2Offset);

void imageSubtraction(cv::Mat &a, cv::Mat &b, int thresh, int blurNum);

float getMeanByMask(const cv::Mat &image, const cv::Mat &mask);

float getRefractionByMask(cv::Mat &mat, cv::Mat matBack, cv::Mat mask);

#endif // OPENCV_UTILS_H
