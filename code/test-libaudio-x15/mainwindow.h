#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../qcustomplot/qcustomplot.h"
#include "../lib-shared/AudioAPI.hpp"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    float *_x, *_y;
    AudioAPI *_audioApi;
};

#endif // MAINWINDOW_H
