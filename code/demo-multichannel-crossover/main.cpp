/***********************************************************************
 * libdsp-x15 multichannel crossover demo
 *
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

#include "../libdsp-x15/API.hpp"
#include "jackclient.hpp"
#include <iostream>
#include <condition_variable>
#include <jack/jack.h>
#include <signal.h>

void callbackDSP(CallbackResponse *clbkRes);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);
void shutdown(int status);

const int N_SAMPLES = 512;
API api(callbackDSP);
//JACKClient *jackClient;
std::mutex mtx;
std::condition_variable cv;

int main(){

	float b[3];
	float a[2];

	signal(SIGABRT, shutdown);
    signal(SIGTERM, shutdown);

    api.setDebug(true);
	api.prepareFILTER_BIQUAD(N_SAMPLES);

	b[0] = 0.2513790015131591; //b0
	b[1] = 0.5027580030263182; //b1
	b[2] = 0.2513790015131591; //b2
	a[0] = -0.17124071441396285; //a1
	a[1] = 0.1767567204665992; //a2
	api.configFILTER_BIQUAD(b, a, 0);

	float freq = 1000.0; //Hz
	float sample_rate = 48000.0;
	float sine_test[N_SAMPLES];
	float *buf_in = api.getBufIn(ConfigOps::FILTER_BIQUAD);
	for (int i=0; i < N_SAMPLES; i++){
		sine_test[i] = sin(2*M_PI*freq*i / sample_rate);
		buf_in[i] = sine_test[i];
	}

	//jackClient = new JACKClient();
	//jackClient->addCallback(callbackJACK);

	api.ocl_DSPF_sp_filter_biquad();
	//api.ocl_foo();

	std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);

	return 0;
}

void shutdown(int status){
    //jackClient->stop();
    //delete jackClient;
    cv.notify_one();
    exit(status);
}

void callbackDSP(CallbackResponse *clbkRes){
	std::cout << "CallbackDSP executed" << std::endl;

	//cv.notify_one();
}

void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){
	float *in_buf = api.getBufIn(ConfigOps::FILTER_BIQUAD);
	for (int i=0; i < n_frames; i++){
		in_buf[i] = in[i];
		out[i] = in[i];
	}
}
