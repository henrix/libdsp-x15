/***********************************************************************
 * libdsp-x15 Filter Biquad Test
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

#include "../libdsp-x15/library.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

void callbackDSP(CallbackResponse *clbkRes);

const int N_SAMPLES = 512;
bool DspOperationFinished = false;

int main(){
    API api(callbackDSP);

    api.setDebug(true);
    api.prepareFILTER_BIQUAD(N_SAMPLES);

    float b[3];
    float a[2];
    /*
        Lowpass:
            - 48 kHz sample rate
            - 200 Hz cutoff
            - 0.707 Q factor
            - 6 dB gain
    */
    b[0] = 0.0001682236792723621; //b0
    b[1] = 0.0003364473585447242; //b1
    b[2] = 0.0001682236792723621; //b2
    a[0] = -1.9629797472685724; //a1
    a[1] = 0.9636526419856617; //a2
    //api.configFILTER_BIQUAD(b, a, 0);

    float Fc = 10000.0;
    float Fs = 48000.0;
    float Q = 0.707;
    float peakGain = 6.0;
    API::FILTER_TYPE type = API::LP;
    api.configFILTER_BIQUAD(type, Fc, Fs, Q, peakGain);

    float freq = 1000.0; //Hz
    float *buf_in = api.getBufIn(CallbackResponse::FILTER_BIQUAD);
    std::ofstream sinout("../../test/data/crossover/sine.txt");
    for (int i=0; i < N_SAMPLES; i++){
        buf_in[i] = sin(2*M_PI*freq*i / Fs);
        sinout << buf_in[i] << std::endl;
    }
    sinout.close();

    api.ocl_DSPF_sp_filter_biquad();

    while(!DspOperationFinished){}

    return 0;
}

/**
 * Callbacks
 */
void callbackDSP(CallbackResponse *clbkRes){
    float *y = clbkRes->getDataPtr();

    std::ofstream filter_out("../../test/data/crossover/output.txt");
    for (int i=0; i < N_SAMPLES; i++){
        filter_out << y[i] << std::endl;
    }
    filter_out.close();

    DspOperationFinished = true;
}
