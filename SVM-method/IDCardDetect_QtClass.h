#ifndef IDCARDDETECT_QTCLASS_H
#define IDCARDDETECT_QTCLASS_H

#include <QtWidgets/QMainWindow>
#include "ui_IDCardDetect_Qt.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Card.h"

class IDCardDetect_QtClass : public QMainWindow
{
    Q_OBJECT

public:
    IDCardDetect_QtClass(QWidget* parent = Q_NULLPTR);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked(); // 上一张按钮槽函数
    void on_pushButton_2_clicked(); // 下一张按钮槽函数

private:
    Ui::IDCardDetect_QtClass ui;
    QGraphicsScene* scene;
    std::vector<cv::Mat> images;
    std::vector<std::string> imagePaths;
    int currentIndex;
    Card card;

    void displayImage(int index);
    void displayIDCardInfo(const std::string& filePath);
};

#endif
