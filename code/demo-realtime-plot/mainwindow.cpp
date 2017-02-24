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
#include <cmath>

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent), _ui(new Ui::MainWindow), _plotRefreshCounter(0)
{
    _ui->setupUi(this);
 	setGeometry(400, 250, 542, 390);

    _demoName = "Realtime Spectrum Plot";
    setWindowTitle(_demoName);
    statusBar()->clearMessage();

    // give the axes some labels:
    _ui->customPlot->xAxis->setLabel("frequency (Hz)");
    _ui->customPlot->yAxis->setLabel("amplitude (dBFS)");

    // set axes ranges, so we see all data:
    _ui->customPlot->xAxis->setRange(10, 24000);
    _ui->customPlot->yAxis->setRange(0, 1);

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
    //QObject::connect(_jackClient, SIGNAL(dataReady(float*)), this, SLOT(getAudioData(float*)));
}

MainWindow::~MainWindow(){
    //delete _ui;
    _jackClient->stop();
    delete _jackClient;
    delete _audioProcessor;
}

void MainWindow::drawFunction(std::vector<double> &data){
    double max = *std::max_element(data.begin(), data.end());
    for (int i=0; i < data.size(); i++){
        data[i] = data[i] / max; //normalize
    }
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
    std::vector<double> _data(256);
    for (int i=0; i < _data.size(); i++)
        _data[i] = std::abs(data[i*2]); //take real part and half of spectrum

    if (_plotRefreshCounter % 4 == 0){
        drawFunction(_data);
        _plotRefreshCounter = 0;
    }
    _plotRefreshCounter++;
}
