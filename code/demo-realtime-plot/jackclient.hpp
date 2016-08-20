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

#ifndef JACKCLIENT_HPP_
#define JACKCLIENT_HPP_

#include <jack/jack.h>
#include <iostream>
#include <vector>

class JACKClient {
    jack_port_t *_input_port, *_output_port;
    jack_client_t *_jack_client;
    std::vector<void (*)(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out)> _callbacks;
    static JACKClient *_self;

    static int _process (jack_nframes_t nframes, void *arg);
    static void _jackShutdown (void *arg);

public:
    JACKClient(const char *client_name = "X15-LIB-AUDIO", const char *server_name = NULL);
    ~JACKClient();
    void stop();
    void addCallback(void (*callback)(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out));
    void writeAudio(jack_nframes_t n_frames, jack_default_audio_sample_t *samples);
};

#endif //JACKCLIENT_HPP_
