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
#define GNUPLOT_DEPRECATE_WARN

#include "jackclient.hpp"
#include "../lib-shared/API.hpp"
#include "gnuplot-iostream.h"
#include <QApplication>
#include <jack/jack.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

void callback(CallbackResponse *clbkRes);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);

int N = (48000); // = sample rate
float audio_buffer[48000];

bool fftFinished = false, ifftFinished = false;
Gnuplot gp;
API api(callback);

int main(int argc, char* argv[])
{
    api.prepareFFT(N, 4, N);
    api.prepareIFFT(N, 4, N);
    //gp << "set yrange [-1:1]\n";

    /**
     * Generate sine
     */
    /*float *xFFT = api.getBufX(CallbackResponse::FFT);
    std::ofstream sinout("../../test/data/sine.txt");
    if (sinout.is_open()){
        for (int i=0; i < N; i++){
            xFFT[PAD + 2*i] = sin(2*M_PI*64*i / (double) N);
            xFFT[PAD + 2*i + 1] = 0;
            sinout << xFFT[PAD + 2*i] << std::endl;
        }
        sinout.close();
    }
    else{
        std::cout << "Couldn't write sine test output" << std::endl;
    }*/

    /*api.ocl_DSPF_sp_fftSPxSP();  
    while(!fftFinished){}
    float *yFFT = api.getBufY(CallbackResponse::FFT);
    float *xIFFT = api.getBufX(CallbackResponse::IFFT);
    for (int i=0; i < 2*N; i++){
        xIFFT[i] = yFFT[i];
    }
    api.ocl_DSPF_sp_ifftSPxSP();
    while(!ifftFinished){}*/

    JACKClient jackClient;
    jackClient.addCallback(callbackJACK);

    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);
    //while(42){}

    return 0;
}

/**
 * Callbacks
 */
void callback(CallbackResponse *clbkRes){
    //std::cout << "Event type callback FFT: " << type << std::endl;
    float *y = clbkRes->getDataPtr();

    if (clbkRes->getOp() == CallbackResponse::FFT){
        //int n = clbkRes->getN();
        std::vector<float> pts(N);
        pts.assign(y, y+N);
        gp << "plot '-' binary" << gp.binFmt1d(pts, "array") << "with lines notitle\n";
        gp.sendBinary1d(pts);
        gp.flush();
    }

    /*if (clbkRes->getOp() == CallbackResponse::FFT){
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

        /*gp << "plot '-' binary" << gp.binFmt1d(pts, "array") << "with lines notitle\n";
        gp.sendBinary1d(pts);
		gp.flush();*/
    /*

        delete clbkRes;
        fftFinished = true;
    }
    else if (clbkRes->getOp() == CallbackResponse::IFFT){
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
    }*/
}

size_t count = 0;
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){
    //std::cout << "Buffer size: " << n_frames << " - in: " << in[n_frames - 1] << std::endl;

    /*for (int i=0; i < n_frames; i++)
        audio_buffer[i+count] = in[i];*/

    float *x = api.getBufX(CallbackResponse::FFT);
    for (int i=0; i < n_frames; i++){
        x[PAD + 2*i + count] = in[i];
        x[PAD + 2*i + 1 + count] = 0;
    }

    count += n_frames;
    if (count == N) 
        api.ocl_DSPF_sp_fftSPxSP();
    count %= N;
}
