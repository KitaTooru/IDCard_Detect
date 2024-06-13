#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;
using namespace cv::ml;

class Detect {
private:
    Mat src;
    Mat data;  // ��������
    Mat labels;
    int K;
    Size DataSize;
    vector<int> idnumber;
    Ptr<TrainData> tData;
    Ptr<KNearest> model;
    vector<Rect> rect;  //���ÿ�����ֵľ���

public:
    Detect() : K(5), DataSize(Size(45, 45)) {}
    Detect(Mat& src) : K(5), DataSize(Size(45, 45)), src(src) {}
    //��ż�⵽������
    vector<int> DetectNum();
    //KNNѵ��ģ��
    void KNNTrainData();

    void SetK(int k);
    void SetSrc(Mat& src);

    vector<Rect> FindRect(const Mat& src); 
    void ShowNum();

    //�ȽϺ���
    static bool comp_rect(const Rect& rect1, const Rect& rect2) {
        return rect1.tl().x < rect2.tl().x;
    }
};
