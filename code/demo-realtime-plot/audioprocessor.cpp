#include "audioprocessor.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#define	PAD	0

AudioProcessor* AudioProcessor::_instance = nullptr;

AudioProcessor::AudioProcessor(QObject *parent, int bufSize)
    : QObject(parent), _bufSize(bufSize),
      _dspTaskFactory(DspTaskFactory::getInstance()),
      _taskProcessor(*(new TaskProcessor()))
{
    _fftTask = _dspTaskFactory.createFFT_SP(bufSize, _callbackDSP, _taskProcessor);
    _instance = this;
}

AudioProcessor::~AudioProcessor(){

}

void AudioProcessor::_callbackDSP(DspTask& task){
    FFT_SP& fftTask = (FFT_SP&) task;
    float *outBuf = fftTask.getOutputBuffer();
    emit(_instance->processedDataReady(outBuf));
}

void AudioProcessor::processData(float *data){
    float *x = _fftTask->getInputBuffer();
    for (int i=0; i < _bufSize; i++){
        x[PAD + 2*i] = data[i]; //real part
        x[PAD + 2*i + 1] = 0; //imaginary part
    }

    _taskProcessor.enqueueTask(*_fftTask);
}
