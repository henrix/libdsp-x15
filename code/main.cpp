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
#include <CL/cl.hpp>

#include "AudioAPI.hpp"
#include <math.h>
#include <iostream>
#include <fstream>

void callback(cl_event, cl_int type, void *user_data)
{
	std::cout << "FFT finished" << std::endl;
}

int main(int argc, const char* argv[])
{
	int sampling_rate = 48000;
	int N = 16384;
	int bufsize = 2*N;
	float *x = (float*) __malloc_ddr(sizeof(float)*2*N);
	float *y = (float*) __malloc_ddr(sizeof(float)*2*N);
	AudioAPI *api = new AudioAPI();

	/**
	 * FFT Test Sine 1kHz
	 */
	std::ofstream sinout("../test/data/1kHz_sin.txt");
	if (sinout.is_open()){
		for (int i=0; i < N; i++){
			x[PAD + 2*i] = sin(2*M_PI*1000*i / (double) N);
			x[PAD + 2*i + 1] = 0;
			sinout << x[PAD + 2*i] << std::endl;
		}
		sinout.close();
	}
	else{
		std::cout << "Couldn't write sine test output" << std::endl;
	}

	api->ocl_DSPF_sp_fftSPxSP(N, x, y, 4, 16384);

	std::ofstream fftoutsine("../test/data/fft_1kHz_sin.txt");
	if (fftoutsine.is_open()){
		for (int i=0; i < N*2; i++){
			fftoutsine << y[i] << std::endl;
		}
		fftoutsine.close();
	}
	else{
		std::cout << "Couldn't write fft sine test output" << std::endl;
	}


	/**
	 * FFT Test Rect 1kHz
	 */
	std::ofstream rectout("../test/data/1kHz_rect.txt");
	if (rectout.is_open()){
		for (int i=0; i < N; i++){
			if (i % (N/1000) <= (N/1000)/2){
				x[PAD + 2*i] = 1.0;
				x[PAD + 2*i + 1] = 0.0;
			}
			else{
				x[PAD + 2*i] = 0.0;
				x[PAD + 2*i + 1] = 0.0;
			}
			rectout << x[PAD + 2*i] << std::endl;
		}
		rectout.close();
	}
	else{
		std::cout << "Couldn't write rect test output" << std::endl;
	}

	api->ocl_DSPF_sp_fftSPxSP(N, x, y, 4, 16384);

	std::ofstream fftoutrect("../test/data/fft_1kHz_rect.txt");
	if (fftoutrect.is_open()){
		for (int i=0; i < bufsize; i++){
			fftoutrect << y[i] << std::endl;
		}
		fftoutrect.close();
	}
	else{
		std::cout << "Couldn't write fft rect test output" << std::endl;
	}


	/**
	 * IFFT of rect spectrum
	 */
	for (int i=0; i < 2*N; i++){
		x[i] = y[i];
	}
	api->ocl_DSPF_sp_ifftSPxSP(N, x, y, 4, 16384);
	std::ofstream ifftoutrect("../test/data/ifft_1kHz_rect_spectrum.txt");
	if (ifftoutrect.is_open()){
		for (int i=0; i < bufsize; i++){
			ifftoutrect << y[i] << std::endl;
		}
		ifftoutrect.close();
	}
	else{
		std::cout << "Couldn't write ifft rect spectrum test output" << std::endl;
	}

	delete api;
	__free_ddr(x);
	__free_ddr(y);
}
