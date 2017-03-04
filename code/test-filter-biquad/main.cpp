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

/*
    libdsp-x15 offers two configuration methods for DSP filter biquad operation
        1. Using normalized filter coefficients b0, b1, b2, a1, a2. 
           More information can be found here: 
           http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
        2. Using the following user defined parameters
            - Filter length (N)
            - Cutoff Frequency
            - Sampling Frequency
            - Quality Factor
            - Gain
*/

#include "../libdsp-x15/library.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <algorithm>

const int FILTER_LENGTH = 512;
bool DspOperationFinished = false;

void callback(DspTask& task);
void calcTransferFunction(float xPoints[], float yPoints[], int N, FilterBiquadSP::TYPE type, float b0, float b1, float b2, float a1, float a2);

int main(){
    DspTaskFactory& factory = DspTaskFactory::getInstance();
    TaskProcessor processor(true);

    FilterBiquadSP* filter = factory.createFilterBiquadSP(FILTER_LENGTH, callback, processor);
    
    /* configure filter */
    float Fc = 10000.0;
    float Fs = 48000.0;
    float Q = 1.0;
    float peakGain = 6.0;
    FilterBiquadSP::TYPE type = FilterBiquadSP::NOTCH;
    filter->configFilter(type, Fc, Fs, Q, peakGain);

    float xTransfer[filter->getLength()], yTransfer[filter->getLength()];
    std::vector<float> coeffs = FilterBiquadSP::calcCoefficients(type, Fc, Fs, Q, peakGain);
    calcTransferFunction(xTransfer, yTransfer, filter->getLength(), type, coeffs.at(0), coeffs.at(1), coeffs.at(2), coeffs.at(4), coeffs.at(5));
    
    float freq1 = 1000.0; 
    float* inputBuffer = filter->getInputBuffer();
    std::ofstream sinout1k("../../test/data/filter/sine_1kHz.txt");
    std::ofstream transferXOut("../../test/data/filter/transferX.txt");
    std::ofstream transferYOut("../../test/data/filter/transferY.txt");
    for (unsigned int i=0; i < filter->getLength(); i++){
        inputBuffer[i] = sin(2.0*M_PI*freq1*(float)i / Fs);
        sinout1k << inputBuffer[i] << std::endl;
        transferXOut << xTransfer[i] << std::endl;
        transferYOut << yTransfer[i] << std::endl;
    }
    sinout1k.close();
    transferXOut.close();
    transferYOut.close();

    processor.enqueueTask(*filter);

    while(!DspOperationFinished){}
    DspOperationFinished = false;

    float freq2 = 10000.0;
    std::ofstream sinout10k("../../test/data/filter/sine_10kHz.txt");
    for (unsigned int i=0; i < filter->getLength(); i++){
        inputBuffer[i] = sin(2.0*M_PI*freq2*(float)i / Fs);
        sinout10k << inputBuffer[i] << std::endl;
    }
    sinout10k.close();

    processor.enqueueTask(*filter);

    while(!DspOperationFinished){}

    return 0;
}

void callback(DspTask& task){
    static int count = 0;
    float* outputBuffer = task.getOutputBuffer();
    FilterBiquadSP& filter = (FilterBiquadSP&) task;

    if (count == 0){ //1kHz sine
        std::ofstream filteredSine1kHz("../../test/data/filter/filtered_sine_1kHz.txt");
        for (unsigned int i=0; i < filter.getLength(); i++)
            filteredSine1kHz << outputBuffer[i] << std::endl;
        filteredSine1kHz.close();
    }
    else { //10 kHz sine
        std::ofstream filteredSine10kHz("../../test/data/filter/filtered_sine_10kHz.txt");
        for (unsigned int i=0; i < filter.getLength(); i++)
            filteredSine10kHz << outputBuffer[i] << std::endl;
        filteredSine10kHz.close();
    }
    count++;
    DspOperationFinished = true;
}

void calcTransferFunction(float xPoints[], float yPoints[], int length, FilterBiquadSP::TYPE type, float a0, float a1, float a2, float b1, float b2){
    for (int idx=0; idx < 512; idx++){
        float w = idx / ((float)length - 1.0) * M_PI; //lin scale
        float phi = std::pow(sin(w/2.0), 2);
        float y = std::log(std::pow(a0+a1+a2, 2.0) - 4.0*(a0*a1 + 4.0*a0*a2 + a1*a2)*phi + 16.0*a0*a2*phi*phi) - std::log(std::pow(1+b1+b2, 2.0) - 4.0*(b1 + 4*b2 + b1*b2)*phi + 16.0*b2*phi*phi);
        y = y * 10.0 / std::log(10.0);
        if (y < -200.0)
            y = -200.0;
        xPoints[idx] = (idx / ((float)length - 1.0) * 48000.0 / 2.0); //lin scale
        yPoints[idx] = y;
    }
}
