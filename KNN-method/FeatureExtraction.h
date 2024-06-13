#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class FeatureEx {
private:
    Mat src;
    Mat GrayImage;    // �Ҷ�ͼ��
    Mat BinaryImage;  // ��ֵͼ��
    Mat EdgImage;     // ��Եͼ��
    Mat Roi;          // ����Ȥ����
    Vec4i LongestLine;
    vector<Vec4i> lines;

public:
    void SetSrc(Mat& src);           // ����Դͼ��
    Mat Feature();                   // ִ��������ȡ
    void ReSizePic(Mat& src, double TargetSize);  // ����ͼ��ߴ�
    void FindEdge();                 // ���ұ�Ե
    void FixPic(const Mat& src);     // �޸�ͼ��
    Vec4i Findline(Mat& contours, double length, double gap, int vote);  // �������ֱ��
    Mat FindRoi();                   // ���Ҹ���Ȥ����
};

