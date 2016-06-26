/***********************************************************************
 * BeagleBoard-X15 audio DSP lib
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
#define __CL_ENABLE_EXCEPTIONS

#include "display.hpp"
#include "jackclient.hpp"
#include "../lib/AudioAPI.hpp"
#include "../lib/CallbackResponse.hpp"
#include <CL/cl.hpp>
#include <QApplication>
#include <jack/jack.h>
#include <math.h>
#include <iostream>
#include <fstream>

int N = (16*1024);
bool fftFinished = false, ifftFinished = false;

void callbackFFT(cl_event ev, cl_int type, void *user_data);
void callbackIFFT(cl_event ev, cl_int type, void *user_data);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);

int main(int argc, char* argv[])
{
    AudioAPI *api;
    int sampling_rate = 48000;
    int bufsize = 2*N;
    float *x = (float*) __malloc_ddr(sizeof(float)*2*N);
    float *y = (float*) __malloc_ddr(sizeof(float)*2*N);

    //QApplication a(argc, argv);
    //Display w;
    //w.show();

    /**
     * Generate sine
     */
    std::ofstream sinout("../../test/data/sine.txt");
    if (sinout.is_open()){
        for (int i=0; i < N; i++){
            x[PAD + 2*i] = sin(2*M_PI*64*i / (double) N);
            x[PAD + 2*i + 1] = 0;
            sinout << x[PAD + 2*i] << std::endl;
        }
        sinout.close();
    }
    else{
        std::cout << "Couldn't write sine test output" << std::endl;
    }

    //JACKClient jackClient;
    //jackClient.addCallback(callbackJACK);
    api = new AudioAPI();
    api->ocl_DSPF_sp_fftSPxSP(N, x, y, 4, 16384, callbackFFT);
    while(!fftFinished){}
    for (int i=0; i < 2*N; i++){
        //x[PAD + 2*i] = y[PAD + 2*i];
        //x[PAD + 2*i + 1] = 0;
        x[i] = y[i];
    }

    api->ocl_DSPF_sp_ifftSPxSP(N, x, y, 4, 16384, callbackIFFT);

    while(!ifftFinished){}
    std::cout << "Main finished" << std::endl;

    delete api;
    __free_ddr(x);
    __free_ddr(y);

    //return a.exec(); //TODO: Create JACKClient in new thread (fix QT return a.exec()); Move memory allocation to AudioAPI;
}

/**
 * Callbacks
 */
void callbackFFT(cl_event ev, cl_int type, void *user_data){
    std::cout << "Event type callback FFT: " << type << std::endl;
    CallbackResponse *clbkRes = (CallbackResponse*) user_data;
    float *y = clbkRes->getDataPtr();

    if (clbkRes->getOp() == AudioAPI::FFT){
        std::ofstream fftoutsine("../../test/data/fft_sine.txt");
        if (fftoutsine.is_open()){
            for (int i=0; i < N; i++){
                fftoutsine << y[PAD + 2*i + 1] << std::endl;
            }
            fftoutsine.close();
        }
        else{
            std::cout << "Couldn't write FFT sine test output" << std::endl;
        }
        std::cout << "FFT calculation completed." << std::endl;
        delete clbkRes;
        fftFinished = true;
    }
}

void callbackIFFT(cl_event ev, cl_int type, void *user_data){
    std::cout << "Event type callback IFFT: " << type << std::endl;
    CallbackResponse *clbkRes = (CallbackResponse*) user_data;
    float *y = clbkRes->getDataPtr();

    if (clbkRes->getOp() == AudioAPI::IFFT){
        std::ofstream ifftout("../../test/data/ifft_sine_spectrum.txt");
        if (ifftout.is_open()){
            for (int i=0; i < N; i++){
                ifftout << y[PAD + 2*i] << std::endl;
                //ifftout << y[i] << std::endl;
            }
            ifftout.close();
        }
        else{
            std::cout << "Couldn't write IFFT sine spectrum test output" << std::endl;
        }
        std::cout << "IFFT calculation completed." << std::endl;
        delete clbkRes;
        ifftFinished = true;
    }
}

void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){
    //std::cout << "_process() callback called with " << n_frames << " called" << std::endl;
}
