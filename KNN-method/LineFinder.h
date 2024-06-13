#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

class LineFinder {
private:
    vector<Vec4i> lines;  // �����ҵ����߶�
    double deltaRho;  // �ۼ����ľ���ֱ���
    double deltaTheta;  // �ۼ����ĽǶȷֱ���
    int minVote;  // �ۼ����е���СͶƱ��
    double minLength;  // ��С�߶γ���
    double maxGap;  // �߶μ������������

public:
    LineFinder();
    void setAccResolution(double dRho, double dTheta);
    void setMinVote(int minv);
    void setLineLengthAndGap(double length, double gap);
    vector<Vec4i> findLines(Mat& binary);
    void drawDetectedLines(Mat& image, Scalar color = Scalar(0, 0, 255));
    vector<Rect> extractROIs(Mat& image, double aspectRatio = 1.585, double tolerance = 0.1);
};
