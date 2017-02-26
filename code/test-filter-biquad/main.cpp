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
#include <algorithm>

void callbackDSP(CallbackResponse *clbkRes);
void calcTransferFunction(float xPoints[], float yPoints[], int N, API::FILTER_TYPE type, float a0, float a1, float a2, float b1, float b2);

const int N_SAMPLES = 512;
bool DspOperationFinished = false;

/*
    libdsp-x15 offers two configuration methods for DSP filter operation
        1. Using normalized filter coefficients a0, a1, a2, b1, b2. 
           More information can be found here: 
           http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
        2. Using the following user defined parameters
            - Filter length (N)
            - Cutoff Frequency
            - Sampling Frequency
            - Quality Factor
            - Gain
*/

int main(){
    API api(callbackDSP);

    api.setDebug(true);
    api.prepareFILTER_BIQUAD(N_SAMPLES);
    
    /*
        - Type: Lowpass
        - 48 kHz sample rate
        - 200 Hz cutoff
        - 0.707 Q factor
        - 6 dB gain
    */
    float b[3];
    float a[2];
    b[0] = 0.0001682236792723621; //b0
    b[1] = 0.0003364473585447242; //b1
    b[2] = 0.0001682236792723621; //b2
    a[0] = -1.9629797472685724; //a1
    a[1] = 0.9636526419856617; //a2
    //api.configFILTER_BIQUAD(b, a, 0);

    float Fc = 10000.0;
    float Fs = 48000.0;
    float Q = 1.0;
    float peakGain = 6.0;
    API::FILTER_TYPE type = API::NOTCH;
    float xTransfer[N_SAMPLES], yTransfer[N_SAMPLES];
    std::vector<float> coeffs = API::calcBiquadCoefficients(type, Fc, Fs, Q, peakGain);
    calcTransferFunction(xTransfer, yTransfer, N_SAMPLES, type, coeffs.at(0), coeffs.at(1), coeffs.at(2), coeffs.at(3), coeffs.at(4));
    api.configFILTER_BIQUAD(type, Fc, Fs, Q, peakGain);

    float freq1 = 1000.0; //Hz
    float *buf_in = api.getBufIn(CallbackResponse::FILTER_BIQUAD);
    std::ofstream sinout1k("../../test/data/filter/sine_1kHz.txt");
    std::ofstream transferXOut("../../test/data/filter/transferX.txt");
    std::ofstream transferYOut("../../test/data/filter/transferY.txt");
    for (int i=0; i < N_SAMPLES; i++){
        buf_in[i] = sin(2.0*M_PI*freq1*(float)i / Fs);
        sinout1k << buf_in[i] << std::endl;
        transferXOut << xTransfer[i] << std::endl;
        transferYOut << yTransfer[i] << std::endl;
    }
    sinout1k.close();

    api.ocl_DSPF_sp_filter_biquad();
    while(!DspOperationFinished){}
    DspOperationFinished = false;
    float freq2 = 10000.0;
    std::ofstream sinout10k("../../test/data/filter/sine_10kHz.txt");
    for (int i=0; i < N_SAMPLES; i++){
        buf_in[i] = sin(2.0*M_PI*freq2*(float)i / Fs);
        sinout10k << buf_in[i] << std::endl;
    }
    sinout10k.close();

    api.ocl_DSPF_sp_filter_biquad();
    while(!DspOperationFinished){}

    return 0;
}

/**
 * Callbacks
 */
void callbackDSP(CallbackResponse *clbkRes){
    static int count = 0;
    float *y = clbkRes->getDataPtr();

    if (count == 0){ //1kHz sine
        std::ofstream filteredSine1kHz("../../test/data/filter/filtered_sine_1kHz.txt");
        for (int i=0; i < N_SAMPLES; i++)
            filteredSine1kHz << y[i] << std::endl;
        filteredSine1kHz.close();
    }
    else { //10 kHz sine
        std::ofstream filteredSine10kHz("../../test/data/filter/filtered_sine_10kHz.txt");
        for (int i=0; i < N_SAMPLES; i++)
            filteredSine10kHz << y[i] << std::endl;
        filteredSine10kHz.close();
    }
    count++;
    DspOperationFinished = true;
}

void calcTransferFunction(float xPoints[], float yPoints[], int N, API::FILTER_TYPE type, float a0, float a1, float a2, float b1, float b2){
    for (int idx=0; idx < 512; idx++){
        double w = idx / ((double)N - 1.0) * M_PI; //lin scale
        double phi = std::pow(sin(w/2.0), 2);
        double y = std::log(std::pow(a0+a1+a2, 2.0) - 4.0*(a0*a1 + 4.0*a0*a2 + a1*a2)*phi + 16.0*a0*a2*phi*phi) - std::log(std::pow(1+b1+b2, 2.0) - 4.0*(b1 + 4*b2 + b1*b2)*phi + 16.0*b2*phi*phi);
        y = y * 10.0 / std::log(10.0);
        if (y < -200.0)
            y = -200.0;
        xPoints[idx] = (idx / ((double)N - 1.0) * 48000.0 / 2.0); //lin scale
        yPoints[idx] = y;
    }
}
