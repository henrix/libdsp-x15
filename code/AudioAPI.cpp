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

#include "AudioAPI.hpp"
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include "ocl_util.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#define	PAD	0

AudioAPI::AudioAPI(unsigned int sampling_rate) : _sampling_rate(sampling_rate), 
	_N(sampling_rate/2)

{
	//_x = float[N*2];
	//_y = float[N*2];
	//_w = float[N*2];

	try {
        cl::Context context(CL_DEVICE_TYPE_ACCELERATOR);
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        std::ifstream t("./audio_kernel.cl");
        if (!t) { std::cout << "Error Opening Kernel Source file\n"; exit(-1); }

        int bufsize = sizeof(float) * (2*_N + PAD + PAD);

        //Buffer _bufX(context, CL_MEM_READ_ONLY,  bufsize);
     	//Buffer _bufY(context, CL_MEM_WRITE_ONLY, bufsize);
     	//Buffer _bufW(context, CL_MEM_READ_ONLY,  bufsize);

        std::string kSrc((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        cl::Program::Sources    source(1, std::make_pair(kSrc.c_str(),kSrc.length()));
        cl::Program             program = cl::Program(context, source);
        program.build(devices, "./dsplib.ae66");

    }
    catch(cl::Error err) {

        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

AudioAPI::~AudioAPI()
{

}

void AudioAPI::ocl_DSPF_sp_fftSPxSP_r2c(int N, float *x, float *w, 
	float *y, int n_min, int n_max)
{

}

void AudioAPI::ocl_DSPF_sp_ifftSPxSP_c2r(int N, float *x, float *w, 
	float *y, int n_min, int n_max)
{

}

/* 
    Function for generating Specialized sequence of twiddle factors 
    (took from TI OpenCL (FFT) example)
*/
void AudioAPI::_tw_gen(float *w, int n)
{
    int i, j, k;
    const double PI = 3.141592654;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {
            w[k]     = (float) sin (2 * PI * i / n);
            w[k + 1] = (float) cos (2 * PI * i / n);
            w[k + 2] = (float) sin (4 * PI * i / n);
            w[k + 3] = (float) cos (4 * PI * i / n);
            w[k + 4] = (float) sin (6 * PI * i / n);
            w[k + 5] = (float) cos (6 * PI * i / n);
            k += 6;
        }
    }
}
