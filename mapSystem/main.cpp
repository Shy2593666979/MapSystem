#include "mainwindow.h"
#include <QApplication>

resolve_xml XML;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    XML.imortant();
    w.showComBox();
    w.zoomInOut();
    w.show();
    return a.exec();
}
