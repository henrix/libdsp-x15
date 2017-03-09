#include "audioprocessor.h"
#include <iostream>

AudioProcessor* AudioProcessor::_instance = nullptr;

AudioProcessor::AudioProcessor(QObject *parent, uint32_t bufSize)
    : QObject(parent), _filterIsReady(false), _bufSize(bufSize),
      _dspTaskFactory(DspTaskFactory::getInstance()), _taskProcessor(*(new TaskProcessor()))
{
    _instance = this;
    _filter = _dspTaskFactory.createFilterBiquadSP(bufSize, _callbackDSP, _taskProcessor);
}

AudioProcessor::~AudioProcessor(){
}

std::vector<float> AudioProcessor::calcBiquadCoefficients(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain){
    return FilterBiquadSP::calcCoefficients(type, Fc, Fs, Q, peakGain);
}

void AudioProcessor::setFilterCoefficients(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain){
    _filter->configFilter(type, Fc, Fs, Q, peakGain);
}

void AudioProcessor::_callbackDSP(DspTask& task){
    FilterBiquadSP& filter = (FilterBiquadSP&) task;
    float *processedData = task.getOutputBuffer();
    emit(_instance->audioDataReady(processedData));
}

void AudioProcessor::processData(float *data){
    float *bufIn = _filter->getInputBuffer();
    memcpy(bufIn, data, _bufSize * sizeof(float));
    _taskProcessor.enqueueTask(*_filter);
}
