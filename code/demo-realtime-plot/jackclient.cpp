#include "jackclient.h"
#include <iostream>

JackClient::JackClient(QObject *parent) : QObject(parent){

}

void JackClient::start(){

    //create connection to JACK server
    if ((_client = jack_client_open("Realtime Spectrum Plot Demo", JackNullOption, NULL)) == 0){
        std::cerr << "couldn't access JACK server. running?" << std::endl;
        exit(1);
    }

    //register input ports (stereo)
    _in = jack_port_register(_client, "Realtime_Plot_In", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    //set callback
    jack_set_process_callback(_client, _jackStaticCallback, (void*)this);

    //init buffer size
    setBufSize(512);

    if (jack_activate(_client)){
        std::cerr << "failed to activate JACK" << std::endl;
        exit(1);
    }
}

void JackClient::stop(){
    jack_port_unregister(_client, _in);
    jack_client_close(_client);
}

int JackClient::getBufSize(){
    return jack_get_buffer_size(_client);
}

void JackClient::setBufSize(int size){
    jack_set_buffer_size(_client, size);
}

int JackClient::getRate(){
    return jack_get_sample_rate(_client);
}

int JackClient::_jackStaticCallback(jack_nframes_t nframes, void *arg){
    return ((JackClient*) arg)->_jackCallback(nframes);
}

int JackClient::_jackCallback(jack_nframes_t nframes){
    float *buf = (float *)(jack_port_get_buffer(_in, nframes));

    emit(dataReady(buf));

    return 0;
}
