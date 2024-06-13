#include "DetectNumber.h"

void Detect::SetK(int k) {
    this->K = k;
}

void Detect::SetSrc(Mat& src) {
    this->src = src;
}

void Detect::KNNTrainData() {
    string numfilename;
    int flag = 0;
    for (flag = 0; flag < 10; flag++) {
        string filename;
        stringstream sstr;
        sstr << flag;
        filename = sstr.str();
        numfilename = "E:\\Data\\ID_Card_Recognize\\reference\\idNumDetect\\train\\num" + filename + "\\Eng.txt";
        string ImgName;
        ifstream fin(numfilename); //��ԭʼ����ͼƬ�ļ��б�  
        while (getline(fin, ImgName)) {
            Mat getnum;
            ImgName = "E:\\Data\\ID_Card_Recognize\\reference\\idNumDetect\\train\\num" + filename + "\\" + ImgName;
            getnum = imread(ImgName);

            cvtColor(getnum, getnum, COLOR_BGR2GRAY);
            threshold(getnum, getnum, 128, 255, THRESH_BINARY);

            resize(getnum, getnum, DataSize, 0, 0, INTER_LINEAR);
            data.push_back(getnum.reshape(0, 1));
            labels.push_back(flag);
        }
    }
    data.convertTo(data, CV_32FC1);
    tData = TrainData::create(data, ROW_SAMPLE, labels);
    model = KNearest::create();
    model->setDefaultK(K);
    model->setIsClassifier(true);
    model->train(tData);
}

vector<Rect> Detect::FindRect(const Mat& src) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(src, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
    // ɾ��̫�̻�̫��������
    int cmin = 50; // ��С��������
    int cmax = 400; // �����������
    auto itc = contours.begin();

    // �����������
    while (itc != contours.end()) {
        // ��֤������С
        if (itc->size() < cmin || itc->size() > cmax)
            itc = contours.erase(itc);
        else {
            Rect r1 = boundingRect(*itc);
            //��������΢����
            r1 = r1 + Point(-2, -2);
            r1 = r1 + Size(4, 4);
            rect.push_back(r1);
            ++itc;
        }
    }
    //��ͼƬ�е����ְ�X�����������
    sort(this->rect.begin(), this->rect.end(), Detect::comp_rect);
    return rect;
}

vector<int> Detect::DetectNum() {
    KNNTrainData();
    FindRect(src);
    for (auto it = rect.begin(); it != rect.end(); ++it) {
        Mat roi = src(*it); //��ȡ��������
        roi.convertTo(roi, CV_32FC1);

        threshold(roi, roi, 1, 255, THRESH_BINARY);
        int x = (DataSize.width - roi.cols) / 2;
        int y = (DataSize.height - roi.rows) / 2;
        Mat sbackground(DataSize, CV_32FC1, Scalar(0));
        Mat newroi(sbackground, Rect(x, y, roi.cols, roi.rows));
        roi.copyTo(newroi);
        sbackground = sbackground.reshape(0, 1);
        float r = model->predict(sbackground);
        idnumber.push_back((int)r);
    }
    return idnumber;
}

void Detect::ShowNum() {
    for (auto it : idnumber) {
        cout << it;
    }
}
