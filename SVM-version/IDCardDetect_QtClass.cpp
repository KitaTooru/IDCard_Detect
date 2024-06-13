#include "IDCardDetect_QtClass.h"
#include <QDir>
#include <QDebug>

IDCardDetect_QtClass::IDCardDetect_QtClass(QWidget* parent)
    : QMainWindow(parent), currentIndex(-1)
{
    ui.setupUi(this);
    scene = new QGraphicsScene(this);
    ui.graphicsView->setScene(scene);

    // 初始化
    card.setPicFolderPath("E:\\Data\\ID_Card_Recognize\\test");
    card.setTrainDataFolderPath("E:\\Data\\ID_Card_Recognize\\reference\\idcard-opencv-svm\\idcard\\data\\trainData\\");
    card.setSavePath("E:\\Data\\ID_Card_Recognize\\save\\");
    card.setTrain("FALSE");
    card.setDebug("FALSE");

    // 连接按钮点击信号到槽函数
    connect(ui.pushButton, &QPushButton::clicked, this, &IDCardDetect_QtClass::on_pushButton_clicked);
    connect(ui.pushButton_3, &QPushButton::clicked, this, &IDCardDetect_QtClass::on_pushButton_3_clicked);
    connect(ui.pushButton_2, &QPushButton::clicked, this, &IDCardDetect_QtClass::on_pushButton_2_clicked);
}

void IDCardDetect_QtClass::on_pushButton_clicked()
{
    // 打开文件对话框
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Image Files (*.png *.jpg *.bmp)"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    QStringList fileNames = dialog.selectedFiles();

    // 清空图像列表并重置
    images.clear();
    imagePaths.clear();
    currentIndex = -1;
    scene->clear();

    // 使用OpenCV读取图像并存储
    for (const QString& fileName : fileNames)
    {
        cv::Mat img = cv::imread(fileName.toStdString());
        if (!img.empty())
        {
            images.push_back(img);
            imagePaths.push_back(fileName.toStdString());
        }
    }

    // 确保图像和路径顺序一致
    if (!images.empty())
    {
        currentIndex = 0;
        displayImage(currentIndex);
    }
}

void IDCardDetect_QtClass::on_pushButton_3_clicked()
{
    if (currentIndex > 0)
    {
        currentIndex--;
        displayImage(currentIndex);
    }
}

void IDCardDetect_QtClass::on_pushButton_2_clicked()
{
    if (currentIndex < images.size() - 1)
    {
        currentIndex++;
        displayImage(currentIndex);
    }
}

void IDCardDetect_QtClass::displayImage(int index)
{
    if (index < 0 || index >= images.size())
        return;

    qDebug() << "Displaying image at index: " << index;

    cv::Mat img = images[index];
    cv::Mat rgbImg;
    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);

    QImage qImg(rgbImg.data, rgbImg.cols, rgbImg.rows, rgbImg.step, QImage::Format_RGB888);

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qImg));
    scene->clear();
    scene->addItem(item);
    ui.graphicsView->fitInView(item, Qt::KeepAspectRatio);

    // 显示身份证识别信息
    std::string filePath = imagePaths[index];
    displayIDCardInfo(filePath);
}

void IDCardDetect_QtClass::displayIDCardInfo(const std::string& filePath)
{
    std::string txt;
    int res = card.identify(filePath, txt);

    if (res == 0)
    {
        // 获取省份信息
        std::string provinceCode = txt.substr(0, 2);
        std::string province = card.getMapPlace().at(stoi(provinceCode));

        // 获取出生日期信息
        std::string birthYear = txt.substr(6, 4);
        std::string birthMonth = txt.substr(10, 2);
        std::string birthDay = txt.substr(12, 2);

        // 获取性别信息
        char genderCode = txt[16];
        std::string gender = (genderCode - '0') % 2 == 0 ? "Female" : "Male";

        // 设置 QLabel 文本
        QString labelText = QString("ID Number: %1\nProvince: %2\nBirthdate: %3-%4-%5\nGender: %6")
            .arg(QString::fromStdString(txt))
            .arg(QString::fromStdString(province))
            .arg(QString::fromStdString(birthYear))
            .arg(QString::fromStdString(birthMonth))
            .arg(QString::fromStdString(birthDay))
            .arg(QString::fromStdString(gender));
        ui.label->setText(labelText);
    }
    else
    {
        ui.label->setText("Error identifying ID card.");
    }
}
