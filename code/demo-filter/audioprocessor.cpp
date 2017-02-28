#include "audioprocessor.h"
#include <iostream>

AudioProcessor* AudioProcessor::_instance = nullptr;

AudioProcessor::AudioProcessor(QObject *parent, uint32_t bufSize)
    : QObject(parent), _filterIsReady(false), _bufSize(bufSize)
{
    _api = new API(_callbackDSP);
    _api->setDebug(false);
    _api->prepareFILTER_BIQUAD(bufSize);
    _api->prepareFFT(bufSize, 4, bufSize);
    _instance = this;
}

AudioProcessor::~AudioProcessor(){
    delete _api;
}

std::vector<float> AudioProcessor::calcBiquadCoefficients(API::FILTER_TYPE type, float Fc, float Fs, float Q, float peakGain){
    return API::calcBiquadCoefficients(type, Fc, Fs, Q, peakGain);
}

void AudioProcessor::setFilterCoefficients(API::FILTER_TYPE type, float Fc, float Fs, float Q, float peakGain){
    _filterIsReady = false;
    _api->configFILTER_BIQUAD(type, Fc, Fs, Q, peakGain);
    _filterIsReady = true;
}

void AudioProcessor::_callbackDSP(CallbackResponse *clbkRes){
    float *processedData = clbkRes->getDataPtr();
    if (clbkRes->getOp() == CallbackResponse::FILTER_BIQUAD){
        /*
        if (!_instance->_api->isBusy(CallbackResponse::FFT)){
            float *buf = _instance->_api->getBufIn(CallbackResponse::FFT);
            for (unsigned int i=0; i < 512; i++)
                //buf[i*2] = processedData[i];
                buf[i*2] = sin((float)2.0*M_PI*(float)1000.0*(float)i / (float)48000.0);
            _instance->_api->ocl_DSPF_sp_fftSPxSP();
        }
        */
        emit(_instance->audioDataReady(processedData));
    }
    else if (clbkRes->getOp() == CallbackResponse::FFT)
        emit(_instance->spectrumDataReady(processedData));
}

void AudioProcessor::processData(float *data){
    if (_filterIsReady){
        float *bufIn = _api->getBufIn(CallbackResponse::FILTER_BIQUAD);
        memcpy(bufIn, data, _bufSize * sizeof(float));
        //for (int i=0; i < 512; i++){
        //    bufIn[i] = sin((float)2.0*M_PI*(float)1000.0*(float)i / (float)48000.0);
        //}
        if (!_api->isBusy(CallbackResponse::FILTER_BIQUAD))
            _api->ocl_DSPF_sp_filter_biquad();
    }
}
