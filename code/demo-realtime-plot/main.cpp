#include <QApplication>
#include "mainwindow.h"
#include <memory>
#include <iostream>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QApplication::setGraphicsSystem("raster");
#endif
	QApplication a(argc, argv);
	MainWindow w;

    w.show();
	return a.exec();
}
