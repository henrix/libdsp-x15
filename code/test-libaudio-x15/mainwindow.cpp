#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../lib-shared/CallbackResponse.hpp"
#include <iostream>
#include <jack/jack.h>
#include <iostream>
#include <fstream>

#define	PAD	0

void callbackFFT(CallbackResponse *clbkRes);
void callbackIFFT(CallbackResponse *clbkRes);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);

int N = (16*1024);
bool fftFinished = false, ifftFinished = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int sampling_rate = 48000;
    int bufsize = 2*N;

    _x = new float[2*N];
    _y = new float[2*N];

    /**
     * Generate sine
     */
    std::ofstream sinout("../../test/data/sine.txt");
    if (sinout.is_open()){
        for (int i=0; i < N; i++){
            _x[PAD + 2*i] = sin(2*M_PI*64*i / (double) N);
            _x[PAD + 2*i + 1] = 0;
            sinout << _x[PAD + 2*i] << std::endl;
        }
        sinout.close();
    }
    else{
        std::cout << "Couldn't write sine test output" << std::endl;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _audioApi;
    delete[] _x;
    delete[] _y;
}

/**
 * Callbacks
 */
void callbackFFT(CallbackResponse *clbkRes){
    //std::cout << "Event type callback FFT: " << type << std::endl;
    //CallbackResponse *clbkRes = (CallbackResponse*) user_data;
    float *y = clbkRes->getDataPtr();

    if (clbkRes->getOp() == CallbackResponse::FFT){
        std::ofstream fftoutsine("../../test/data/fft_sine.txt");
        if (fftoutsine.is_open()){
            for (int i=0; i < N; i++){
                fftoutsine << y[PAD + 2*i + 1] << std::endl;
            }
            fftoutsine.close();
        }
        else{
            std::cout << "Couldn't write FFT sine test output" << std::endl;
        }
        std::cout << "FFT calculation completed." << std::endl;
        delete clbkRes;
        fftFinished = true;
    }
}

void callbackIFFT(CallbackResponse *clbkRes){
    //std::cout << "Event type callback IFFT: " << type << std::endl;
    //CallbackResponse *clbkRes = (CallbackResponse*) user_data;
    float *y = clbkRes->getDataPtr();

    if (clbkRes->getOp() == CallbackResponse::IFFT){
        std::ofstream ifftout("../../test/data/ifft_sine_spectrum.txt");
        if (ifftout.is_open()){
            for (int i=0; i < N; i++){
                ifftout << y[PAD + 2*i] << std::endl;
                //ifftout << y[i] << std::endl;
            }
            ifftout.close();
        }
        else{
            std::cout << "Couldn't write IFFT sine spectrum test output" << std::endl;
        }
        std::cout << "IFFT calculation completed." << std::endl;
        delete clbkRes;
        ifftFinished = true;
    }
}

void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){
    std::cout << "_process() callback called with " << n_frames << " called" << std::endl;
}
