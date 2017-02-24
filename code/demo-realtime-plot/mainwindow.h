#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "jackclient.h"
#include "audioprocessor.h"
#include "../qcustomplot/qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include <vector>
#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
  
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void drawFunction(std::vector<double> &data);
    void setRangeX(double begin, double end);
    void setRangeY(double begin, double end);

private:
    Ui::MainWindow *_ui;
    QString _demoName;
    JackClient *_jackClient;
    AudioProcessor *_audioProcessor;
    QVector<double> *_x;
    int _plotRefreshCounter;

public slots:
    void getAudioData(float *data);
};

#endif // MAINWINDOW_H
