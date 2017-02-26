/***********************************************************************
 * libdsp-x15 FFT / IFFT Test
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

#define PAD 0

#include "../libdsp-x15/library.h"
#include <iostream>
#include <fstream>
#include <complex>
#include <cmath>

void callbackDSP(CallbackResponse *clbkRes);

const int N = 512;
bool fftFinished = false, ifftFinished = false;

int main(int argc, char* argv[]){
	API api(callbackDSP);
	api.setDebug(true);
	api.prepareFFT(N, 4, N);
	api.prepareIFFT(N, 4, N);

	float *xFFT = api.getBufIn(CallbackResponse::FFT);

	/* Generate sine */
	std::ofstream sinout("../../test/data/fft_ifft/sine.txt");
    if (sinout.is_open()){
        for (int i=0; i < N; i++){
            xFFT[PAD + 2*i] = sin(2.0*M_PI*1000.0*(float)i / (float) 48000.0);
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
    for (int i=0; i < 2*N; i++){
        xIFFT[i] = yFFT[i];
    }
    api.ocl_DSPF_sp_ifftSPxSP();
    while(!ifftFinished){}
    
    return 0;
}

void callbackDSP(CallbackResponse *clbkRes){
	float *y = clbkRes->getDataPtr();
    int data_size = clbkRes->getDataSize();
    int n = data_size / 2;

    if (clbkRes->getOp() == CallbackResponse::FFT){
        std::complex<float> cmplx[N];

        std::ofstream fftoutsine("../../test/data/fft_ifft/spectrum_mag.txt");
        std::ofstream fftoutsinePhase("../../test/data/fft_ifft/spectrum_phase.txt");
        if (fftoutsine.is_open()){
           	for (int i=0; i < n; i++){
                cmplx[i].real(y[PAD + 2*i]);
                cmplx[i].imag(y[PAD + 2*i + 1]);
               	fftoutsine << std::abs(cmplx[i]) / 256.0 << std::endl;
                fftoutsinePhase << std::arg(cmplx[i]) << std::endl;
           	}
           	fftoutsine.close();
        }
        else{
           	std::cout << "Couldn't write FFT sine test output" << std::endl;
        }
        fftFinished = true;
    }
    else if (clbkRes->getOp() == CallbackResponse::IFFT){

    	std::ofstream ifftout("../../test/data/fft_ifft/ifft_spectrum.txt");
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

    delete clbkRes;
}
