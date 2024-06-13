#include "IDCardDetect_QtClass.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    IDCardDetect_QtClass w;
    w.show();
    return a.exec();
}
