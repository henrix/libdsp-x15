/***********************************************************************
 * Author: Henrik Langer (henni19790@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "jackclient.hpp"

JACKClient* JACKClient::_self = 0;

JACKClient::JACKClient(const char *client_name, const char *server_name) : _input_port(0), _output_port(0){
    _self = this;
    const char **ports;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    _jack_client = jack_client_open(client_name, options, &status, server_name);

    if (_jack_client == NULL) {
        std::cerr << "jack_client_open() failed, status = " << status << std::endl;
        if (status & JackServerFailed){
            std::cerr << "Unable to connect to JACK server" << std::endl;
        }
        return;
    }
    if (status & JackServerStarted) {
        std::cerr << "JACK server started" << std::endl;
    }
    if (status & JackNameNotUnique) {
        client_name = jack_get_client_name(_jack_client);
        std::cerr << "JACK client name not unique. New client name: " << client_name << std::endl;
    }

    jack_set_process_callback(_jack_client, _process, 0);
    jack_on_shutdown(_jack_client, _jackShutdown, 0);

    std::cout << "Using " << jack_get_sample_rate (_jack_client) << " Hz sample rate" << std::endl;
    _input_port = jack_port_register(_jack_client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    _output_port = jack_port_register(_jack_client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if ((_input_port == NULL) || (_output_port == NULL)) {
        std::cerr << "No more JACK ports available" << std::endl;
        return;
    }

    if (jack_activate(_jack_client)){
        std::cerr << "Can't activate JACK client" << std::endl;
        return;
    }

    ports = jack_get_ports(_jack_client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
    if (ports == NULL) {
        std::cerr << "No physical capture ports" << std::endl;
        return;
    }

    if (jack_connect (_jack_client, ports[0], jack_port_name (_input_port))) {
        std::cerr << "Can't connect input ports" << std::endl;
    }

    delete ports;

    ports = jack_get_ports (_jack_client, NULL, NULL,
                JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL) {
        std::cerr << "No physical playback ports" << std::endl;
        return;
    }

    if (jack_connect (_jack_client, jack_port_name (_output_port), ports[0])) {
        std::cerr << "Can't connect output ports" << std::endl;
    }

    delete ports;
}

JACKClient::~JACKClient(){
    _self = NULL;
}

void JACKClient::stop(){
    jack_client_close (_jack_client);
}

void JACKClient::addCallback(void (*callback)(
        jack_nframes_t n_frames,
        jack_default_audio_sample_t *in,
        jack_default_audio_sample_t *out)){

    _callbacks.push_back(callback);
}

void JACKClient::writeAudio(jack_nframes_t n_frames, jack_default_audio_sample_t *samples){
    jack_default_audio_sample_t *out;
    //out = (jack_default_audio_sample_t*)jack_port_get_buffer (_output_port, nframes);

    //TODO
}


/**
 * Statics
 */
int JACKClient::_process (jack_nframes_t nframes, void *arg){
    jack_default_audio_sample_t *in, *out;

    in = (jack_default_audio_sample_t*)jack_port_get_buffer (_self->_input_port, nframes);
    out = (jack_default_audio_sample_t*)jack_port_get_buffer (_self->_output_port, nframes);

    for (int i=0; i < _self->_callbacks.size(); i++)
        _self->_callbacks[i](nframes, in, out);

    return 0;
}

void JACKClient::_jackShutdown (void *arg){
    std::cout << "JACK client stopped" << std::endl;
}
