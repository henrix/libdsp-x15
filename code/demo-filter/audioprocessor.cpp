#include "audioprocessor.h"
#include <iostream>

AudioProcessor* AudioProcessor::_instance = nullptr;

AudioProcessor::AudioProcessor(QObject *parent, uint32_t bufSize, FilterBiquadSP::TYPE type,
                               float Fs, float Fc, float Q, float peakGain)
    : QObject(parent), _filterIsReady(false), _bufSize(bufSize),
      _dspTaskFactory(DspTaskFactory::getInstance()), _taskProcessor(*(new TaskProcessor()))
{
    _instance = this;
    _filter1 = _dspTaskFactory.createFilterBiquadSP(bufSize, _callbackDSP, _taskProcessor);
    _filter2 = _dspTaskFactory.createFilterBiquadSP(bufSize, _callbackDSP, _taskProcessor);
    _filter1->configFilter(type, Fs, Fc, Q, peakGain);
    _filter2->configFilter(type, Fs, Fc, Q, peakGain);
}

AudioProcessor::~AudioProcessor(){
    jack_port_unregister(_jackClient, _in1);
    jack_port_unregister(_jackClient, _in2);
    jack_port_unregister(_jackClient, _out1);
    jack_port_unregister(_jackClient, _out2);
    jack_client_close(_jackClient);
}

std::vector<float> AudioProcessor::calcBiquadCoefficients(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain){
    return FilterBiquadSP::calcCoefficients(type, Fc, Fs, Q, peakGain);
}

void AudioProcessor::startJack(){
    //create connection to JACK server
    if ((_jackClient = jack_client_open("Filter Demo", JackNullOption, NULL)) == 0){
        std::cerr << "couldn't access JACK server. running?" << std::endl;
        exit(1);
    }

    //register input ports (stereo)
    _in1 = jack_port_register(_jackClient, "Filter_In_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    _in2 = jack_port_register(_jackClient, "Filter_In_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    _out1 = jack_port_register(_jackClient, "Filter_Out_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    _out2 = jack_port_register(_jackClient, "Filter_Out_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    //set callback
    jack_set_process_callback(_jackClient, _callbackJackStatic, (void*)this);

    //init buffer size
    jack_set_buffer_size(_jackClient, 512);

    if (jack_activate(_jackClient)){
        std::cerr << "failed to activate JACK" << std::endl;
        exit(1);
    }
}

unsigned int AudioProcessor::getBufSize(){
    return jack_get_buffer_size(_jackClient);
}

void AudioProcessor::_callbackDSP(DspTask& task){
    FilterBiquadSP& filter = (FilterBiquadSP&) task;
    float *processedData = filter.getOutputBuffer();
    if (filter.id == 0){ //left channel
        float *buf = (jack_default_audio_sample_t*)jack_port_get_buffer(_instance->_out1, _instance->getBufSize());
        memcpy(buf, processedData, _instance->getBufSize()*sizeof(float));
    }
    else { //right channel
        float *buf = (jack_default_audio_sample_t*)jack_port_get_buffer(_instance->_out2, _instance->getBufSize());
        memcpy(buf, processedData, _instance->getBufSize()*sizeof(float));
    }
}

int AudioProcessor::_callbackJackStatic(jack_nframes_t nframes, void *arg){
    return ((AudioProcessor*) arg)->_callbackJack(nframes);
}

int AudioProcessor::_callbackJack(jack_nframes_t nframes){
    float *bufJackL = (float*)(jack_port_get_buffer(_in1, nframes));
    float *bufJackR = (float*)(jack_port_get_buffer(_in2, nframes));
    float *bufInDsp1 = _filter1->getInputBuffer();
    float *bufInDsp2 = _filter2->getInputBuffer();
    memcpy(bufInDsp1, bufJackL, _bufSize * sizeof(float));
    memcpy(bufInDsp2, bufJackR, _bufSize * sizeof(float));
    _taskProcessor.enqueueTask(*_filter1);
    _taskProcessor.enqueueTask(*_filter2);

    return 0;
}

void AudioProcessor::setFilterParameter(FilterBiquadSP::TYPE type, float Fc, float Fs, float Q, float peakGain){
    _filter1->configFilter(type, Fc, Fs, Q, peakGain);
    _filter2->configFilter(type, Fc, Fs, Q, peakGain);
}
