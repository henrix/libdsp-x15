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

const int N = 512;
bool fftFinished = false, ifftFinished = false;

void callback(DspTask& task);

int main(int argc, char* argv[]){
    DspTaskFactory& factory = DspTaskFactory::getInstance();
    TaskProcessor processor(true);

    FFT_SP* fft = factory.createFFT_SP(N, callback, processor);
    IFFT_SP* ifft = factory.createIFFT_SP(N, callback, processor);

    float* xFFT = fft->getInputBuffer();

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
        std::cout << "couldn't write sine test output" << std::endl;
    }

    processor.enqueueTask(*fft);
    while(!fftFinished){}
    
    float *yFFT = fft->getOutputBuffer();
    float *xIFFT = ifft->getInputBuffer();
    for (int i=0; i < 2*N; i++){
        xIFFT[i] = yFFT[i];
    }
    processor.enqueueTask(*ifft);
    while(!ifftFinished){}

    delete fft;
    delete ifft;
    
    return 0;
}

void callback(DspTask& task){

    if (task.operation == DspTask::FFT){
        FFT_SP& fft = (FFT_SP&) task;

        std::shared_ptr<std::vector<std::complex<float>>> output = fft.getOutputBufferCopy();

        std::ofstream magSpectrum("../../test/data/fft_ifft/spectrum_mag.txt");
        std::ofstream phaseSpectrum("../../test/data/fft_ifft/spectrum_phase.txt");
        if (magSpectrum.is_open()){
            for (unsigned int i=0; i < fft.getN(); i++){
                magSpectrum << std::abs((*output)[i]) / 256.0 << std::endl;
                phaseSpectrum << std::arg((*output)[i]) << std::endl;
            }
            magSpectrum.close();
            phaseSpectrum.close();
        }
        else{
            std::cout << "couldn't write FFT sine test output" << std::endl;
        }
        std::cout << "FFT operation finished" << std::endl;
        fftFinished = true;
    }
    else if (task.operation == DspTask::IFFT){
        IFFT_SP& ifft = (IFFT_SP&) task;
        
        std::shared_ptr<std::vector<std::complex<float>>> output = ifft.getOutputBufferCopy();

        std::ofstream ifftout("../../test/data/fft_ifft/ifft_spectrum.txt");
        if (ifftout.is_open()){
            for (unsigned int i=0; i < ifft.getN(); i++){
                ifftout << (*output)[i].real() << std::endl;
            }
            ifftout.close();
        }
        else{
            std::cout << "couldn't write IFFT sine spectrum test output" << std::endl;
        }
        std::cout << "IFFT operation finished" << std::endl;
        ifftFinished = true;
    }
    
}
