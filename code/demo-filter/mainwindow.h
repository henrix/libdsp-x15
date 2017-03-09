#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "jackclient.h"
#include "audioprocessor.h"
#include "../qcustomplot/qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include <vector>
#include <QMainWindow>
#include <QTimer>
#include <QDial>
#include <QLabel>

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
    void drawFunction(std::vector<double> &xPoints, std::vector<double> &yPoints);
    void setRangeX(double begin, double end);
    void setRangeY(double begin, double end);

private:
    Ui::MainWindow *_ui;
    QString _demoName;
    JackClient *_jackClient;
    AudioProcessor *_audioProcessor;
    QVector<double> *_x, *_xSpectrum;
    QDial *_inputFc, *_inputQ, *_inputPeakGain;
    QLabel *_labelFc, *_labelQ, *_labelPeakGain;
    int _plotRefreshCounter;

    void _plotTransferFunction(FilterBiquadSP::TYPE type, float a0, float a1, float a2, float b1, float b2);

public slots:
    void getAudioData(float *data);
    void inputValueChanged(int val);
    void plotSpectrum(float *magnitude);
};

#endif // MAINWINDOW_H
