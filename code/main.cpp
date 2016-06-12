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

#include <math.h>
#include <iostream>
#include "AudioAPI.hpp"


void callback(cl_event, cl_int type, void *user_data)
{
	std::cout << "FFT finished" << std::endl;
}

int main(int argc, const char* argv[])
{
	float *x = new float[48000]();
	float *y = new float[48000]();
	AudioAPI *api = new AudioAPI();

	for (int i=0; i < 48000; i++){
		x[i] = sin(2*M_PI*1000*i / (double) 48000);
	}

	api->ocl_DSPF_sp_fftSPxSP(24000, x, y, 4, 16384);

	delete api;
	delete[] x;
	delete[] y;
}


