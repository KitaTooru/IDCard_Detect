#include "LineFinder.h"
LineFinder::LineFinder() : deltaRho(1), deltaTheta(CV_PI / 180), minVote(10), minLength(0), maxGap(0) {}

void LineFinder::setAccResolution(double dRho, double dTheta) {
    deltaRho = dRho;
    deltaTheta = dTheta;
}

void LineFinder::setMinVote(int minv) {
    minVote = minv;
}

void LineFinder::setLineLengthAndGap(double length, double gap) {
    minLength = length;
    maxGap = gap;
}

vector<Vec4i> LineFinder::findLines(Mat& binary) {
    vector<Vec4i> lines;
    HoughLinesP(binary, lines, deltaRho, deltaTheta, minVote, minLength, maxGap);
    return lines;
}

void LineFinder::drawDetectedLines(Mat& image, Scalar color) {
    for (const auto& line : lines) {
        Point pt1(line[0], line[1]);
        Point pt2(line[2], line[3]);
        cv::line(image, pt1, pt2, color, 2);
    }
}

vector<Rect> LineFinder::extractROIs(Mat& image, double aspectRatio, double tolerance) {
    vector<Rect> rois;
    for (const auto& line : lines) {
        Point pt1(line[0], line[1]);
        Point pt2(line[2], line[3]);
        Rect roi = boundingRect(vector<Point>{pt1, pt2});

        // ȷ�� ROI ��ͼ��Χ��
        roi &= Rect(0, 0, image.cols, image.rows);

        // ���� ROI �Ŀ��߱�
        double currentRatio = static_cast<double>(roi.width) / roi.height;

        // �����߱��Ƿ����ݲΧ��
        if (abs(currentRatio - aspectRatio) < tolerance) {
            rois.push_back(roi);
        }
    }
    return rois;
}
