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
		std::cout << "Couldn't write sin test output" << std::endl;
	}

	api->ocl_DSPF_sp_fftSPxSP(N, x, y, 4, 16384);

	std::ofstream fftout("../test/data/fft_1kHz_sin.txt");
	if (fftout.is_open()){
		float max = 0;
		int max_i = 0;
		for (int i=0; i < bufsize; i++){
			if (max < y[i]){
				max = y[i];
				max_i = i;
			}
			fftout << y[i] << std::endl;
		}
		std::cout << "Maximum:\t" << max << std::endl << "Index:\t\t" << max_i << std::endl;
		fftout.close();
	}
	else{
		std::cout << "Couldn't write fft test output" << std::endl;
	}

	delete api;
	__free_ddr(x);
	__free_ddr(y);
}
