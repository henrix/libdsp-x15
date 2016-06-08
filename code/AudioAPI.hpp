/***********************************************************************
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

class AudioAPI
{
	unsigned int _sampling_rate, _N; //0.5 Hz resolution
	//float _x[], _y[], _z[];
	//std::Buffer _bufX, _bufY, _bufW;
	//cl::Kernel _audioKernel;

	void _tw_gen(float *w, int n);

public:
	AudioAPI(unsigned int sampling_rate = 48000);
	~AudioAPI();
	void ocl_DSPF_sp_fftSPxSP_r2c(int N, float *x, float *w, 
		float *y, int n_min, int n_max);
	void ocl_DSPF_sp_ifftSPxSP_c2r(int N, float *x, float *w, 
		float *y, int n_min, int n_max);
};

#endif
