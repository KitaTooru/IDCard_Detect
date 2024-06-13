#include "FeatureExtraction.h"
#include "LineFinder.h"

void FeatureEx::SetSrc(Mat& src) {
    this->src = src;
}

void FeatureEx::ReSizePic(Mat& src, double TargetSize) {
    double w = src.size().width;
    double h = src.size().height;
    if (w > h)
        resize(src, src, Size(TargetSize, TargetSize * h / w), 0, 0, INTER_LINEAR);
    else
        resize(src, src, Size(TargetSize * w / h, TargetSize), 0, 0, INTER_LINEAR);
}

// ʹ��MSER������ȡ��Ե
void FeatureEx::FindEdge() {
    int height = src.size().height;
    int width = src.size().width;
    EdgImage = Mat::zeros(GrayImage.size(), CV_8U);
    vector<vector<Point>> points;
    vector<Rect> rects;
    Ptr<MSER> ptrMSER = MSER::create(5, 20, 300, 0.25, 0.1);
    ptrMSER->detectRegions(GrayImage, points, rects);
    RNG rng;
    Mat output(GrayImage.size(), CV_8UC3, Scalar(255, 255, 255));
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        Vec3b c(rng.uniform(0, 254), rng.uniform(0, 254), rng.uniform(0, 254));
        for (auto itpts = it->begin(); itpts != it->end(); ++itpts) {
            if (output.at<Vec3b>(*itpts)[0] == 255) {
                output.at<Vec3b>(*itpts) = c;
            }
        }
    }
    for (int i = 1; i < height - 1; i++)
        for (int j = 1; j < width - 1; j++) {
            if (output.ptr<Vec3b>(i)[j] != output.ptr<Vec3b>(i + 1)[j] ||
                output.ptr<Vec3b>(i)[j] != output.ptr<Vec3b>(i - 1)[j] ||
                output.ptr<Vec3b>(i)[j] != output.ptr<Vec3b>(i)[j + 1] ||
                output.ptr<Vec3b>(i)[j] != output.ptr<Vec3b>(i)[j - 1])
                EdgImage.at<uchar>(i, j) = 254;
        }
}

void FeatureEx::FixPic(const Mat& src) {
    Mat tmpimg(src);
    ReSizePic(tmpimg, 1000.0);
    cvtColor(tmpimg, GrayImage, COLOR_BGR2GRAY);
    adaptiveThreshold(GrayImage, BinaryImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 5, 5);
    medianBlur(BinaryImage, BinaryImage, 3);
}

Vec4i FeatureEx::Findline(Mat& contours, double length, double gap, int vote) {
    LineFinder finder;
    finder.setLineLengthAndGap(length, gap);
    finder.setMinVote(vote);
    lines = finder.findLines(contours);
    int Longest = 0;
    int max = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        Point pt1(lines[i][0], lines[i][1]);
        Point pt2(lines[i][2], lines[i][3]);
        float distance = norm(pt1 - pt2);
        if (distance > max) {
            max = distance;
            Longest = i;
        }
    }
    return lines[Longest];
}

Mat FeatureEx::FindRoi() {
    if (LongestLine == Vec4i()) {
        return Mat();
    }

    double roiAspectRatio = 9.0;

    Point2f LinePoint1(LongestLine[0] - 20, LongestLine[1]);
    Point2f LinePoint2(LongestLine[2] + 20, LongestLine[3]);
    Point pt1(LongestLine[0] - 20, LongestLine[1]);
    Point pt2(LongestLine[2] + 20, LongestLine[3]);

    Point2f LeftUp, LeftDown, RightUp, RightDown;
    float a = (LinePoint1.y - LinePoint2.y) / (LinePoint1.x - LinePoint2.x);
    float b = (LinePoint1.y - a * LinePoint1.x);
    float c = -1.0 / a;
    float d = (LinePoint1.y - c * LinePoint1.x);
    float d_2 = (LinePoint2.y - c * LinePoint2.x);

    float distance = norm(LinePoint1 - LinePoint2);
    float ang = (LinePoint1.y - LinePoint2.y) / distance;

    Mat tmpimg;

    if (LinePoint1.y == LinePoint2.y) {
        LeftUp = Point2f(pt1.x, pt1.y - 30);
        LeftDown = Point2f(pt1.x, pt1.y + 30);
        RightUp = Point2f(pt2.x, pt2.y - 30);
        RightDown = Point2f(pt2.x, pt2.y + 30);
        vector<Point2f> Area = { LeftUp, LeftDown, RightUp, RightDown };
        RotatedRect rect = minAreaRect(Area);
        Rect roi = rect.boundingRect();
        roi &= Rect(0, 0, BinaryImage.cols, BinaryImage.rows); // ȷ��ROI��ͼ��Χ��
        tmpimg = BinaryImage(roi);
        return tmpimg;
    }
    else if (ang > (-(2.0 / 3.0)) && ang < 0) {
        LeftUp.y = LinePoint1.y - 30;
        LeftDown.y = LinePoint1.y + 30;
        LeftUp.x = (LeftUp.y - d) / c;
        LeftDown.x = (LeftDown.y - d) / c;
        RightUp.y = LinePoint2.y - 30;
        RightDown.y = LinePoint2.y + 30;
        RightUp.x = (RightUp.y - d_2) / c;
        RightDown.x = (RightDown.y - d_2) / c;
        vector<Point2f> Area = { LeftUp, LeftDown, RightUp, RightDown };
        RotatedRect rect = minAreaRect(Area);
        Point2f center = rect.center;
        Mat rot_mat = getRotationMatrix2D(center, rect.angle + 90, 1.0);
        warpAffine(BinaryImage, tmpimg, rot_mat, BinaryImage.size());
        Rect roi(center.x - (rect.size.height / 2), center.y - (rect.size.width / 2), rect.size.height, rect.size.width);
        roi &= Rect(0, 0, tmpimg.cols, tmpimg.rows); // ȷ��ROI��ͼ��Χ��
        tmpimg = tmpimg(roi);
        medianBlur(tmpimg, tmpimg, 3);
        return tmpimg;
    }
    else if (ang > 0 && ang < (2.0 / 3.0)) {
        LeftUp.y = LinePoint1.y - 30;
        LeftDown.y = LinePoint1.y + 30;
        LeftUp.x = (LeftUp.y - d) / c;
        LeftDown.x = (LeftDown.y - d) / c;
        RightUp.y = LinePoint2.y - 30;
        RightDown.y = LinePoint2.y + 30;
        RightUp.x = (RightUp.y - d_2) / c;
        RightDown.x = (RightDown.y - d_2) / c;
        vector<Point2f> Area = { LeftUp, LeftDown, RightUp, RightDown };
        RotatedRect rect = minAreaRect(Area);
        Point2f center = rect.center;
        Mat rot_mat = getRotationMatrix2D(center, rect.angle, 1.0);
        warpAffine(BinaryImage, tmpimg, rot_mat, BinaryImage.size());
        Rect roi(center.x - (rect.size.width / 2), center.y - (rect.size.height / 2), rect.size.width, rect.size.height);
        roi &= Rect(0, 0, tmpimg.cols, tmpimg.rows); // ȷ��ROI��ͼ��Χ��
        tmpimg = tmpimg(roi);
        medianBlur(tmpimg, tmpimg, 3);
        return tmpimg;
    }
    else if (LinePoint1.x == LinePoint2.x) {
        LeftUp = Point2f(pt1.x - 30, pt1.y);
        LeftDown = Point2f(pt1.x + 30, pt1.y);
        RightUp = Point2f(pt2.x - 30, pt2.y);
        RightDown = Point2f(pt2.x + 30, pt2.y);
        vector<Point2f> Area = { LeftUp, LeftDown, RightUp, RightDown };
        RotatedRect rect = minAreaRect(Area);
        Point center = rect.center;
        Rect roiRect(center.x - (rect.size.height / 2), center.y - (rect.size.width / 2), rect.size.height, rect.size.width);
        roiRect &= Rect(0, 0, BinaryImage.cols, BinaryImage.rows); // ȷ��ROI��ͼ��Χ��
        tmpimg = BinaryImage(roiRect);
        return tmpimg;
    }
    return Mat();
}

Mat FeatureEx::Feature() {
    FixPic(src);
    FindEdge();
    LongestLine = Findline(EdgImage, 50, 10, 80);
    Roi = FindRoi();
    return Roi;
}
