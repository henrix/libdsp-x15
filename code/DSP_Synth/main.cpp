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

void callback(CallbackResponse *clbkRes);
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out);

int N = (16*1024);
bool fftFinished = false, ifftFinished = false;
float *y;
//Gnuplot gp;
API api(callback);
float *xFFT, *xIFFT;

int main(int argc, char* argv[])
{
    api.prepareFFT(N);
    //gp << "set yrange [-1:1]\n";

    xFFT = new float[2*N];
    xIFFT = new float[2*N];

    /**
     * Generate sine
     */
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
    }

    //JACKClient jackClient;
    //jackClient.addCallback(callbackJACK);
    api.prepareFFT(N);
    api.prepareIFFT(N); 

    api.ocl_DSPF_sp_fftSPxSP(xFFT, 4, N);   

    while(!fftFinished){}
    for (int i=0; i < 2*N; i++){
        xIFFT[i] = y[i];
    }
    api.ocl_DSPF_sp_ifftSPxSP(xIFFT, 4, N);
    while(!ifftFinished){}

    delete[] xFFT;
    delete[] xIFFT;

    return 0;
}

/**
 * Callbacks
 */
void callback(CallbackResponse *clbkRes){
    //std::cout << "Event type callback FFT: " << type << std::endl;
    y = clbkRes->getDataPtr();
    //int n = clbkRes->getN();
    /*std::vector<float> pts(n);
    pts.assign(_y, _y+n);
    for (int i=0; i < pts.size(); i++){
        std::cout << i << ": " << pts[PAD + 2*i + 1] << std::endl;
    }*/

    if (clbkRes->getOp() == CallbackResponse::FFT){
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
    }
}

bool flag = false;
void callbackJACK(jack_nframes_t n_frames, jack_default_audio_sample_t *in, jack_default_audio_sample_t *out){
    //std::cout << "_process() callback called with " << n_frames << " called" << std::endl;
    /*std::cout << "in: " << in[n_frames - 1] << std::endl;
    if (!flag){
        x = new float[n_frames*2];
        flag = true;
    }

    for (int i=0; i < n_frames; i++){
        x[PAD + 2*i] = in[i];
        x[PAD + 2*i + 1] = 0;
    }*/
    //api.ocl_DSPF_sp_fftSPxSP(x, 4, n_frames);
}
