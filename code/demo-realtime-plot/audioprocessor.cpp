#include "audioprocessor.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#define	PAD	0

AudioProcessor* AudioProcessor::_instance = nullptr;

AudioProcessor::AudioProcessor(QObject *parent, int bufSize)
    : QObject(parent), _bufSize(bufSize)
{
    _api = new API(_callbackDSP);
    _api->setDebug(false);
    _api->prepareFFT(bufSize, 4, bufSize);
    _instance = this;
}

AudioProcessor::~AudioProcessor(){
    delete _api;
}

void AudioProcessor::_callbackDSP(CallbackResponse *clbkRes){
    float *outBuf = clbkRes->getDataPtr();

    emit(_instance->processedDataReady(outBuf));

    delete clbkRes;
}

void AudioProcessor::processData(float *data){
    float *x = _api->getBufIn(CallbackResponse::FFT);
    for (int i=0; i < _bufSize; i++){
        //x[PAD + 2*i] = sin((float)2*M_PI*(float)1000.0*(float)i / (float)48000.0);
        x[PAD + 2*i] = data[i]; //real part
        x[PAD + 2*i + 1] = 0; //imaginary part
    }

    if (!_api->isBusy(CallbackResponse::FFT)){
        _api->ocl_DSPF_sp_fftSPxSP();
    }
}
