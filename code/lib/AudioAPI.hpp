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

#ifndef AUDIOAPI_HPP_
#define AUDIOAPI_HPP_

#include "CallbackResponse.hpp"
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <map>

class AudioAPIImpl;
class AudioAPI 
{
	unsigned int _sampling_rate, _bufsize_fft, _bufsize_ifft;
	int _N_fft, _N_ifft;
	float *_xFFT, *_xIFFT, *_yFFT, *_yIFFT, *_wFFT, *_wIFFT;
	AudioAPIImpl *_pimpl;

	static void (*_pCallbackFFT)(CallbackResponse *resData);
	static void (*_pCallbackIFFT)(CallbackResponse *resData);

	void* _allocBuffer(std::size_t size);
	static void _twGenFFT(float *w, int n);
	static void _twGenIFFT(float *w, int n);

public:
	AudioAPI();
	~AudioAPI();

	int ocl_DSPF_sp_fftSPxSP(int N, float *x,
		float *y, int n_min, int n_max,
		void (*callback)(CallbackResponse *resData));
	int ocl_DSPF_sp_ifftSPxSP(int N, float *x,
		float *y, int n_min, int n_max,
		void (*callback)(CallbackResponse *resData));
	//int convReverbFromWAV(int N, float *x, const std::string &filename, float *y,
	//	void (*callback)(cl_event ev, cl_int e_status, void *user_data));
};

#endif //AUDIOAPI_HPP_
