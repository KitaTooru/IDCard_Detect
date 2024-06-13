#include <opencv2/opencv.hpp>
#include "FeatureExtraction.h"

using namespace std;
using namespace cv;

int main() {
    string imagePath = "image.path";
    Mat src = imread(imagePath);

    if (src.empty()) {
        cerr << "Could not open or find the image" << endl;
        return -1;
    }

    FeatureEx feature;
    feature.SetSrc(src);

    Mat result;
    try {
        result = feature.Feature();
    }
    catch (const cv::Exception& e) {
        cerr << "Error during feature extraction: " << e.what() << endl;
        return -1;
    }
    if (result.empty()) {
        cerr << "Feature extraction failed or no valid ROI found." << endl;
        return -1;
    }

    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", src);
    namedWindow("ROI", WINDOW_AUTOSIZE);
    imshow("ROI", result);
    waitKey(0);

    return 0;
}
