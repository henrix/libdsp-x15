/***********************************************************************
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

#ifndef AUDIO_API_H_
#define AUDIO_API_H_

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <map>
#include "ocl_util.h"

class AudioAPI
{
	unsigned int _sampling_rate, _bufsize_fft, _bufsize_ifft;
	int _N_fft, _N_ifft;
	float *_wFFT, *_wIFFT;
	//std::map<int, >
	cl::Kernel *_fftKernel, *_ifftKernel;
	cl::Buffer *_bufFFTX, *_bufFFTY, *_bufFFTW;
	cl::Buffer *_bufIFFTX, *_bufIFFTY, *_bufIFFTW;
	cl::Program *_program;
	cl::CommandQueue *_Qfft, *_Qifft;
	cl::Context *_context;

	static void _twGen(float *w, int n);

public:
	enum ops {
		FFT,
		IFFT,
		FIR,
		IIR,
		CONV
	};
	AudioAPI();
	~AudioAPI();
	static void completeFFTEvt(cl_event, cl_int type, void *user_data);
	/**
	 * Calculate FFT
	 * @param N
	 * @param x input buffer
	 * @param y output buffer
	 * @param n_min
	 * @param n_max
	 * @return ID of task
	 */
	int ocl_DSPF_sp_fftSPxSP(int N, float *x, 
		float *y, int n_min, int n_max);
	int ocl_DSPF_sp_ifftSPxSP(int N, float *x, 
		float *y, int n_min, int n_max);
	void data_callback(int id, int size, float *buf);
};

#endif
