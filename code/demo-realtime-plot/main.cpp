/***********************************************************************
 * libdsp-x15 Demo Application
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

#define	PAD	0

#include "jackclient.hpp"
#include "display.hpp"
#include "../libdsp-x15/API.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <jack/jack.h>
#include <condition_variable>
#include <signal.h>

void test_FFT_IFFT(int n);
void shutdown(int status);
void callbackDSP(CallbackResponse *clbkRes);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);

const int N = 512; // should be equal or higher than audio buffer size
bool testEnabled = false;
API api(callbackDSP);
Display display(N, N);
JACKClient *jackClient;
std::mutex mtx;
std::condition_variable cv;

int main(int argc, char* argv[]) {

    signal(SIGABRT, shutdown);
    signal(SIGTERM, shutdown);

    /* Execute FFT / IFFT test */
	test_FFT_IFFT(16*1024);

	/* Realtime plots of audio input signal */
    api.prepareFFT(N, 4, N);
    jackClient = new JACKClient();
    jackClient->addCallback(callbackJACK);
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);

    return 0;
}

/**
 * Test function
 */
bool fftFinished = false, ifftFinished = false;
void test_FFT_IFFT(int n){

	testEnabled = true;
    api.setDebug(true);
	api.prepareFFT(n, 4, n);
	api.prepareIFFT(n, 4, n);

	float *xFFT = api.getBufIn(CallbackResponse::FFT);

	/* Generate sine */
	std::ofstream sinout("../../test/data/sine.txt");
    if (sinout.is_open()){
        for (int i=0; i < n; i++){
            xFFT[PAD + 2*i] = sin(2*M_PI*64*i / (double) n);
            xFFT[PAD + 2*i + 1] = 0;
            sinout << xFFT[PAD + 2*i] << std::endl;
        }
        sinout.close();
    }
    else{
        std::cout << "Couldn't write sine test output" << std::endl;
    }

    api.ocl_DSPF_sp_fftSPxSP();
    while(!fftFinished){}
    float *yFFT = api.getBufOut(CallbackResponse::FFT);
    float *xIFFT = api.getBufIn(CallbackResponse::IFFT);
    for (int i=0; i < 2*n; i++){
        xIFFT[i] = yFFT[i];
    }
    api.ocl_DSPF_sp_ifftSPxSP();
    while(!ifftFinished){}
    api.setDebug(false);
}

/**
 * Custom exit function for clean up etc.
 */
void shutdown(int status){
    SDL_Quit();
    jackClient->stop();
    delete jackClient;
    cv.notify_one();
    exit(status);
}


/**
 * Callbacks
 */
//TODO: return const dataptr (btw deny manipulation)
void callbackDSP(CallbackResponse *clbkRes){
    float *y = clbkRes->getDataPtr();
    int data_size = clbkRes->getDataSize();
    int n = data_size / 2;

    if (clbkRes->getOp() == CallbackResponse::FFT){

    	/*Handle test callback */
        if (testEnabled){
        	std::ofstream fftoutsine("../../test/data/fft_sine.txt");
        	if (fftoutsine.is_open()){
            	for (int i=0; i < n; i++){
                	fftoutsine << y[PAD + 2*i + 1] << std::endl;
            	}
            	fftoutsine.close();
        	}
        	else{
            	std::cout << "Couldn't write FFT sine test output" << std::endl;
        	}
        	fftFinished = true;
        }

        /* Get max of calculated spectrum for amplitude scaling */
        float max = 0.0;
        for (int i=0; i < n; i++){
            if (y[PAD + 2*i + 1] > max)
                max = (float)y[PAD + 2*i + 1];
        }

        float pixels[n];
        for (int i=0; i < n/2; i++){
            pixels[i*2] = fabs((float)y[PAD + 2*i + 1] / (float)max); //Amplitude of spectrum
            pixels[i*2+1] = pixels[i*2];
        }

        max = 0.0;
        for (int i=0; i < n; i++){
            if (pixels[i] > max)
                max = pixels[i];
        }

        //display.drawPixels(n, pixels);
        display.drawLines(n, pixels);
    }
    else if (clbkRes->getOp() == CallbackResponse::IFFT){

    	/*Handle test callback */
    	if (testEnabled){
    		std::ofstream ifftout("../../test/data/ifft_sine_spectrum.txt");
        	if (ifftout.is_open()){
            	for (int i=0; i < n; i++){
                	ifftout << y[PAD + 2*i] << std::endl;
            	}
            	ifftout.close();
        	}
        	else{
            	std::cout << "Couldn't write IFFT sine spectrum test output" << std::endl;
        	}
        	ifftFinished = true;
    	}
    }

    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT){ 
        shutdown(0); 
    }

    delete clbkRes;
}

size_t count = 0;
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){

    float *x = api.getBufIn(CallbackResponse::FFT);
    for (unsigned int i=0; i < n_frames; i++){
        x[PAD + 2*i + count] = in[i];
        x[PAD + 2*i + 1 + count] = 0;
    }

    count += n_frames;
    if (count >= N && !api.isBusy(CallbackResponse::FFT)){
        api.ocl_DSPF_sp_fftSPxSP();
    }
    count %= N;
}
