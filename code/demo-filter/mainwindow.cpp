#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QObject>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <QHBoxLayout>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent), _ui(new Ui::MainWindow), _plotRefreshCounter(0)
{
    _ui->setupUi(this);
 	setGeometry(400, 250, 542, 390);

    _demoName = "Filter Demo";
    setWindowTitle(_demoName);
    statusBar()->clearMessage();

    // give the axes some labels:
    _ui->customPlot->xAxis->setLabel("Frequency (Hz)");
    _ui->customPlot->yAxis->setLabel("Magnitude (dB)");

    // set axes ranges, so we see all data:
    _ui->customPlot->xAxis->setRange(0, 1.0/48000.0 * 512.0);
    _ui->customPlot->yAxis->setRange(-1, 1);

    _ui->customPlot->addGraph();
    _ui->customPlot->graph(0)->setPen(QPen(Qt::blue));

    _audioProcessor = new AudioProcessor(this);
    _audioProcessor->setFilterCoefficients(API::LP, 1000.0, 48000.0, 0.707, 6);
    //QObject::connect(_audioProcessor, SIGNAL(dataReady(float*)), this, SLOT(getAudioData(float*)));
    QObject::connect(_ui->inputCutoff, SIGNAL(valueChanged(int)), this, SLOT(inputValueChanged(int)));
    QObject::connect(_ui->inputQ, SIGNAL(valueChanged(int)), this, SLOT(inputValueChanged(int)));
    QObject::connect(_ui->inputPeakGain, SIGNAL(valueChanged(int)), this, SLOT(inputValueChanged(int)));
    QObject::connect(_ui->inputFilterType, SIGNAL(valueChanged(int)), this, SLOT(inputValueChanged(int)));

    _x = new QVector<double>(512);
    for (int i=0; i < _x->size(); i++)
        (*_x)[i] = 1.0/48000.0 * i;

    _jackClient = new JackClient(this);
    _jackClient->start();

    QObject::connect(_jackClient, SIGNAL(dataReady(float*)), _audioProcessor, SLOT(processData(float*)));
    QObject::connect(_audioProcessor, SIGNAL(dataReady(float*)), _jackClient, SLOT(writeData(float*)));
    inputValueChanged(0); //plot default filter setting transfer function
}

MainWindow::~MainWindow(){
    //delete _ui;
    _jackClient->stop();
    delete _jackClient;
    delete _audioProcessor;
}

void MainWindow::drawFunction(std::vector<double> &data){
    QVector<double> _data = QVector<double>::fromStdVector(data);

    _ui->customPlot->graph(0)->setData(*_x, _data);
    _ui->customPlot->replot();
}

void MainWindow::drawFunction(std::vector<double> &xPoints, std::vector<double> &yPoints){
    QVector<double> _xPoints = QVector<double>::fromStdVector(xPoints);
    QVector<double> _yPoints = QVector<double>::fromStdVector(yPoints);

    _ui->customPlot->graph(0)->setData(_xPoints, _yPoints);
    _ui->customPlot->replot();
}

void MainWindow::setRangeX(double begin, double end){
    _ui->customPlot->xAxis->setRange(begin, end);
}

void MainWindow::setRangeY(double begin, double end){
    _ui->customPlot->yAxis->setRange(begin,end);
}

void MainWindow::_plotTransferFunction(API::FILTER_TYPE type, float a0, float a1, float a2, float b1, float b2){
    const int len = 512;
    std::vector<double> xPoints(512), yPoints(512);

    for (int idx=0; idx < 512; idx++){
        double w = idx / ((double)len - 1.0) * M_PI; //lin scale
        //double w = std::exp(std::log(1.0 / 0.001) * idx / ((double)len - 1.0)) * 0.001 * M_PI;	// 0.001 to 1, times pi, log scale
        double phi = std::pow(sin(w/2.0), 2);
        double y = std::log(std::pow(a0+a1+a2, 2.0) - 4.0*(a0*a1 + 4.0*a0*a2 + a1*a2)*phi + 16.0*a0*a2*phi*phi) - std::log(std::pow(1+b1+b2, 2.0) - 4.0*(b1 + 4*b2 + b1*b2)*phi + 16.0*b2*phi*phi);
        y = y * 10.0 / std::log(10.0);
        if (y < -200.0)
            y = -200.0;
        xPoints.push_back(idx / ((double)len - 1.0) * 48000.0 / 2.0); //lin scale
        //xPoints.push_back(idx / (double)(len - 1.0) / 2.0); //log scale
        yPoints.push_back(y);
    }
    float minValY = *std::min_element(yPoints.begin(), yPoints.end());
    float maxValY = *std::max_element(yPoints.begin(), yPoints.end());
    float minValX = *std::min_element(xPoints.begin(), xPoints.end());
    float maxValX = *std::max_element(xPoints.begin(), xPoints.end());

    switch(type){
    default:
    case API::LP:
    case API::HP:
    case API::BP:
    case API::NOTCH:
        minValY = -100.0;
        if (maxValY < 0.0)
           maxValY = 0.0;
        break;
    case API::PEAK:
    case API::LOWSHELF:
    case API::HIGHSHELF:
        minValY = -10.0;
        if (maxValY < 10.0)
            maxValY = 10.0;
        else if (minValY > -10.0)
            minValY = -10.0;
    }

    _ui->customPlot->xAxis->setRange(minValX, maxValX);
    _ui->customPlot->yAxis->setRange(minValY, maxValY);
    drawFunction(xPoints, yPoints);
}

void MainWindow::getAudioData(float *data){
    std::vector<double> _data;
    _data.assign(data, data + 512);
    double max = *std::max_element(_data.begin(), _data.end());
    if (max > 1.0)
        setRangeY(-1.0*max, max);
    else
        setRangeY(-1.0, 1.0);

    if (_plotRefreshCounter % 10 == 0){ //refresh ~ 100ms
        drawFunction(_data);
        _plotRefreshCounter = 0;
    }
    _plotRefreshCounter++;
}

void MainWindow::inputValueChanged(int val){
    float Fc = (float) _ui->inputCutoff->value();
    float Q = (float) _ui->inputQ->value() / 1000.0;
    float peakGain = _ui->inputPeakGain->value();
    float Fs = 48000.0;
    API::FILTER_TYPE type = API::LP;
    switch(_ui->inputFilterType->value()){
    case 0: //LP
        type = API::LP;
        _ui->labelFilterType->setText(QString("Type: Lowpass"));
        break;
    case 1: //HP
        type = API::HP;
        _ui->labelFilterType->setText(QString("Type: Highpass"));
        break;
    case 2: //BP
        type = API::BP;
        _ui->labelFilterType->setText(QString("Type: Bandpass"));
        break;
    case 3: //Notch
        type = API::NOTCH;
        _ui->labelFilterType->setText(QString("Type: Notch"));
        break;
    case 4: //Peak
        type = API::PEAK;
        _ui->labelFilterType->setText(QString("Type: Peak"));
        break;
    case 5: //Lowshelf
        type = API::LOWSHELF;
        _ui->labelFilterType->setText(QString("Type: Lowshelf"));
        break;
    case 6: //Highshelf
        type = API::HIGHSHELF;
        _ui->labelFilterType->setText(QString("Type: Highself"));
        break;
    default:
        type = API::LP;
        _ui->labelFilterType->setText(QString("Filter Type: Lowpass"));
    }
    std::string cutoffString("Cutoff: " + std::to_string((int)Fc) + " Hz");
    _ui->labelCutoff->setText(QString(cutoffString.c_str()));
    std::ostringstream stringStream;
    stringStream << std::setprecision(2) << Q;
    std::string QFactorString("Q: " + stringStream.str());
    _ui->labelQ->setText(QString(QFactorString.c_str()));
    std::string peakGainString("Peak Gain: " + std::to_string((int)peakGain) + " dB");
    _ui->labelPeakGain->setText(QString(peakGainString.c_str()));

    _audioProcessor->setFilterCoefficients(type, Fc, Fs, Q, peakGain);
    std::vector<float> filterCoeffs = AudioProcessor::calcBiquadCoefficients(type, Fc, Fs, Q, peakGain);
    _plotTransferFunction(type, filterCoeffs.at(0), filterCoeffs.at(1), filterCoeffs.at(2), filterCoeffs.at(3), filterCoeffs.at(4));
}
