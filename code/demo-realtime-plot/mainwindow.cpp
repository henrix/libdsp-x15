#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QObject>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <iostream>
#include <algorithm>
#include <complex>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent), _ui(new Ui::MainWindow), _plotRefreshCounter(0)
{
    _ui->setupUi(this);
    setGeometry(400, 250, 542, 390);

    _demoName = "libdsp-x15 - Realtime Spectrum Plot Demo";
    setWindowTitle(_demoName);
    statusBar()->clearMessage();

    _ui->customPlot->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _ui->customPlot->xAxis->setTickLabelFont(font);
    _ui->customPlot->yAxis->setTickLabelFont(font);
    _ui->customPlot->legend->setFont(font);

    _ui->customPlot->xAxis->setLabel("Frequency (Hz)");
    _ui->customPlot->yAxis->setLabel("Magnitude (dBFS)");

    _ui->customPlot->xAxis->setRange(10, 24000);
    _ui->customPlot->yAxis->setRange(-10, 0);
    _ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    _ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    _ui->customPlot->xAxis->grid()->setSubGridVisible(true);
    _ui->customPlot->yAxis->grid()->setSubGridVisible(true);

    _ui->customPlot->addGraph();
    _ui->customPlot->graph(0)->setPen(QPen(Qt::blue));

    _audioProcessor = new AudioProcessor(this);
    QObject::connect(_audioProcessor, SIGNAL(processedDataReady(float*)), this, SLOT(getAudioData(float*)));

    _x = new QVector<double>(256);
    for (int i=0; i < _x->size(); i++)
        (*_x)[i] = (24000.0/_x->size()) * i;

    _jackClient = new JackClient(this);
    _jackClient->start();

    QObject::connect(_jackClient, SIGNAL(dataReady(float*)), _audioProcessor, SLOT(processData(float*)));
}

MainWindow::~MainWindow(){
    _jackClient->stop();
    delete _jackClient;
    delete _audioProcessor;
}

void MainWindow::drawFunction(std::vector<double> &data){
    QVector<double> _data = QVector<double>::fromStdVector(data);

    _ui->customPlot->graph(0)->setData(*_x, _data);
    _ui->customPlot->replot();
}

void MainWindow::setRangeX(double begin, double end){
    _ui->customPlot->xAxis->setRange(begin, end);
}

void MainWindow::setRangeY(double begin, double end){
    _ui->customPlot->yAxis->setRange(begin,end);
}

void MainWindow::getAudioData(float *data){
    std::complex<double> cmplx[512];
    for (int i=0; i < 512; i++){
        cmplx[i].real(data[i*2]);
        cmplx[i].imag(data[i*2 + 1]);
    }
    std::vector<double> _data(256);
    for (int i=0; i < 256; i++)
        _data[i] = std::log10(std::abs(cmplx[i]) / 256.0); //half of magnitude spectrum and normalize

    //if (_plotRefreshCounter % 2 == 0){
        double min = *std::min_element(_data.begin(), _data.end());
        double max = *std::min_element(_data.begin(), _data.end());
        if (min < -10.0 && max > 0.0)
            setRangeY(min, max);
        else if (min < -10.0)
            setRangeY(min, 0);
        else if (max > 0.0)
            setRangeY(-10, max);
        else
            setRangeY(-10, 0);
        drawFunction(_data);
        _plotRefreshCounter = 0;
    //}
    _plotRefreshCounter++;
}
