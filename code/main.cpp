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

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include "ocl_util.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

const char* kernelStr =
    "void DSPF_sp_fftSPxSP(int N, \n"
    "    global float *x, global float *w, global float *y, \n"
    "    unsigned char *brev, int n_min, int offset, int n_max); \n"
    "";

int main(int argc, const char* argv[]) {

    try {
        cl::Context context(CL_DEVICE_TYPE_ACCELERATOR);
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        cl::Program::Sources    source(1, std::make_pair(kernelStr,strlen(kernelStr)));
        cl::Program             program = cl::Program(context, source);
        program.build(devices, "./dsplib.ae66");


    }
    catch(cl::Error err) {

        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}
