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
#include <fstream>
#include <cstring>
#include <cmath>

void callbackDSP(CallbackResponse *clbkRes);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);
void shutdown(int status);

const int N_SAMPLES = 512;
float audio_buf_out[N_SAMPLES];
API api(callbackDSP);
JACKClient *jackClient;
std::mutex mtx;
std::condition_variable cv;

int main(){

	float b[3];
	float a[2];

	signal(SIGABRT, shutdown);
    signal(SIGTERM, shutdown);

    api.setDebug(true);
	api.prepareFILTER_BIQUAD(N_SAMPLES);

	/*
		Lowpass:
			- 48 kHz sample rate
			- 200 Hz cutoff
			- 0.707 Q factor
			- 6 dB gain
	*/
	b[0] = 0.0001682236792723621; //b0
	b[1] = 0.0003364473585447242; //b1
	b[2] = 0.0001682236792723621; //b2
	a[0] = -1.9629797472685724; //a1
	a[1] = 0.9636526419856617; //a2
	api.configFILTER_BIQUAD(b, a, 0);

	float freq = 1000.0; //Hz
	float sample_rate = 48000.0;
	float *buf_in = api.getBufIn(CallbackResponse::FILTER_BIQUAD);
	std::ofstream sinout("../../test/data/crossover/sine.txt");
	for (int i=0; i < N_SAMPLES; i++){
		buf_in[i] = sin(2*M_PI*freq*i / sample_rate);
		sinout << buf_in[i] << std::endl;
	}
	sinout.close();

	//jackClient = new JACKClient();
	//jackClient->addCallback(callbackJACK);

	api.ocl_DSPF_sp_filter_biquad();

	std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);

	return 0;
}

void shutdown(int status){
    jackClient->stop();
    delete jackClient;
    cv.notify_one();
    exit(status);
}

/**
 * Callbacks
 */
void callbackDSP(CallbackResponse *clbkRes){
	float *y = clbkRes->getDataPtr();
	memcpy(audio_buf_out, y, N_SAMPLES);

	std::ofstream filter_out("../../test/data/crossover/output.txt");
	for (int i=0; i < N_SAMPLES; i++){
		filter_out << y[i] << std::endl;
	}
	filter_out.close();

	shutdown(0);
}

void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){
	memcpy(out, audio_buf_out, n_frames);
	float *buf_in = api.getBufIn(CallbackResponse::FILTER_BIQUAD);
	memcpy(buf_in, in, n_frames);
	api.ocl_DSPF_sp_filter_biquad();
}
