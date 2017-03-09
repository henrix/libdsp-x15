#include <QApplication>
#include <QObject>
#include "mainwindow.h"
#include "audioprocessor.h"
#include "../libdsp-x15/library.h"

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
