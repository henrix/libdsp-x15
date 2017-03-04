/***********************************************************************
 * Author: Henrik Langer (henni19790@gmail.com)
 * Copyright: Henrik Langer
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

#include "FilterBiquadSP.h"
#include <iostream>
#include <cmath>

FilterBiquadSP::FilterBiquadSP(unsigned int length, std::function<void(DspTask &task)> callback,
                               std::shared_ptr<cl::Context> clContext, std::shared_ptr<cl::Program> clProgram)
        : DspTask(DspTask::_createId(), DspTask::FILTER_BIQUAD, callback, clContext, clProgram), _length(length),
          _buffer_size(sizeof(float)*length)
{
    /* allocate zero copy buffers in contiguous memory */
    _bufferInput = (float*) DspTask::_mallocBuffer(_buffer_size);
    _bufferB = (float*) DspTask::_mallocBuffer(sizeof(float)*3);
    _bufferA = (float*) DspTask::_mallocBuffer(sizeof(float)*3);
    _bufferDelay = (float*) DspTask::_mallocBuffer(sizeof(float)*2);
    _bufferDelay[0] = 0.0;
    _bufferDelay[1] = 0.0;
    _bufferOutput = (float*) DspTask::_mallocBuffer(_buffer_size);

    /* create OpenCL buffers from zero copy memory */
    _clBufferInput = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, _buffer_size, _bufferInput)
    );
    _clBufferB = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(float)*3, _bufferB)
    );
    _clBufferA = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(float)*3, _bufferA)
    );
    _clBufferDelay = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, sizeof(float)*2, _bufferDelay)
    );
    _clBufferOutput = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, _buffer_size, _bufferOutput)
    );

    _clKernel = std::make_shared<cl::Kernel>(*_clProgram, "ocl_DSPF_sp_biquad");
    _clKernel->setArg(0, *_clBufferInput);
    _clKernel->setArg(1, *_clBufferB);
    _clKernel->setArg(2, *_clBufferA);
    _clKernel->setArg(3, *_clBufferDelay);
    _clKernel->setArg(4, *_clBufferOutput);
    _clKernel->setArg(5, _length);
}

FilterBiquadSP::~FilterBiquadSP(){
    DspTask::_freeBuffer(_bufferInput);
    DspTask::_freeBuffer(_bufferB);
    DspTask::_freeBuffer(_bufferA);
    DspTask::_freeBuffer(_bufferDelay);
    DspTask::_freeBuffer(_bufferOutput);
}

std::vector<cl::Event> FilterBiquadSP::_assignClInputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue){
    try{
        std::vector<cl::Event> finished(4);

        /*
         * blocking buffer i/o (otherwise buffer ptr can not be used by application anymore)
         * offset is 0
         */
        clCmdQueue->enqueueWriteBuffer(*_clBufferInput, CL_FALSE, 0, _buffer_size, _bufferInput, 0, &finished[0]);
        clCmdQueue->enqueueWriteBuffer(*_clBufferB, CL_FALSE, 0, sizeof(float)*3, _bufferB, 0, &finished[1]);
        clCmdQueue->enqueueWriteBuffer(*_clBufferA, CL_FALSE, 0, sizeof(float)*3, _bufferA, 0, &finished[2]);
        clCmdQueue->enqueueWriteBuffer(*_clBufferDelay, CL_FALSE, 0, sizeof(float)*2, _bufferDelay, 0, &finished[3]);

        return finished;
    }
    catch(const cl::Error& err){
        std::cerr << "Error in biquad_filter(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

std::vector<cl::Event> FilterBiquadSP::_assignClOutputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue,
                                                     std::vector<cl::Event>& preEvents){
    try{
        std::vector<cl::Event> finished(1);

        clCmdQueue->enqueueReadBuffer(*_clBufferOutput, CL_TRUE, 0, _buffer_size, _bufferOutput, &preEvents, &finished[0]);

        return finished;
    }
    catch(const cl::Error& err){
        std::cerr << "Error in biquad_filter(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

unsigned int FilterBiquadSP::getLength() {
    return _length;
}

std::size_t FilterBiquadSP::getBufferSize(){
    return _buffer_size;
}

float* FilterBiquadSP::getInputBuffer(unsigned int index){
    if (index == 0)
        return _bufferInput;
    else {
        std::cout << "WARNING: requested non available input buffer in biquad filter. returning nullptr" << std::endl;
        return nullptr;
    }
}

float* FilterBiquadSP::getOutputBuffer(unsigned int index){
    if (index == 0)
        return _bufferOutput;
    else {
        std::cout << "WARNING: requested non available output buffer in biquad filter. returning nullptr" << std::endl;
        return nullptr;
    }
}

void FilterBiquadSP::configFilter(TYPE type, float Fc, float Fs, float Q, float peakGain){
    std::vector<float> coefficents = calcCoefficients(type, Fc, Fs, Q, peakGain);
    for (int i=0; i < 3; i++)
        _bufferB[i] = coefficents.at(i);
    for (int i=0; i < 3; i++)
        _bufferA[i] = coefficents.at(i+3);
}

void FilterBiquadSP::configFilter(std::vector<float>& b, std::vector<float>& a){
    for (int i=0; i < 3; i++)
        _bufferB[i] = b.at(i);
    for (int i=0; i < 3; i++)
        _bufferA[i] = a.at(i);
}

std::vector<float> FilterBiquadSP::calcCoefficients(TYPE type, float Fc, float Fs, float Q, float peakGain){
    float b0, b1, b2, a1, a2, norm;

    float V = std::pow(10.0, std::abs(peakGain) / 20.0);
    float K = std::tan(M_PI * Fc / Fs);
    switch(type){
        case LOWPASS:
            norm = 1.0 / (1.0 + K / Q + K * K);
            b0 = K * K * norm;
            b1 = 2.0 * b0;
            b2 = b0;
            a1 = 2.0 * (K * K - 1.0) * norm;
            a2 = (1.0 - K / Q + K * K) * norm;
            break;
        case HIGHPASS:
            norm = 1.0 / (1.0 + K / Q + K * K);
            b0 = 1.0 * norm;
            b1 = -2.0 * b0;
            b2 = b0;
            a1 = 2.0 * (K * K - 1.0) * norm;
            a2 = (1.0 - K / Q + K * K) * norm;
            break;
        case BANDPASS:
            norm = 1.0 / (1.0 + K / Q + K * K);
            b0 = K / Q * norm;
            b1 = 0.0;
            b2 = -b0;
            a1 = 2.0 * (K * K - 1.0) * norm;
            a2 = (1.0 - K / Q + K * K) * norm;
            break;
        case NOTCH:
            norm = 1.0 / (1.0 + K / Q + K * K);
            b0 = (1.0 + K * K) * norm;
            b1 = 2.0 * (K * K - 1.0) * norm;
            b2 = b0;
            a1 = b1;
            a2 = (1.0 - K / Q + K * K) * norm;
            break;
        case PEAK:
            if (peakGain >= 0.0) {    // boost
                norm = 1.0 / (1.0 + 1.0/Q * K + K * K);
                b0 = (1.0 + V/Q * K + K * K) * norm;
                b1 = 2.0 * (K * K - 1.0) * norm;
                b2 = (1.0 - V/Q * K + K * K) * norm;
                a1 = b1;
                a2 = (1.0 - 1.0/Q * K + K * K) * norm;
            }
            else {    // cut
                norm = 1.0 / (1.0 + V/Q * K + K * K);
                b0 = (1.0 + 1.0/Q * K + K * K) * norm;
                b1 = 2.0 * (K * K - 1.0) * norm;
                b2 = (1.0 - 1.0/Q * K + K * K) * norm;
                a1 = b1;
                a2 = (1.0 - V/Q * K + K * K) * norm;
            }
            break;
        case LOWSHELF:
            if (peakGain >= 0.0) {    // boost
                norm = 1.0 / (1.0 + std::sqrt(2.0) * K + K * K);
                b0 = (1.0 + std::sqrt(2.0*V) * K + V * K * K) * norm;
                b1 = 2.0 * (V * K * K - 1.0) * norm;
                b2 = (1.0 - std::sqrt(2.0*V) * K + V * K * K) * norm;
                a1 = 2.0 * (K * K - 1.0) * norm;
                a2 = (1.0 - std::sqrt(2.0) * K + K * K) * norm;
            }
            else {    // cut
                norm = 1.0 / (1.0 + std::sqrt(2.0*V) * K + V * K * K);
                b0 = (1.0 + std::sqrt(2.0) * K + K * K) * norm;
                b1 = 2.0 * (K * K - 1.0) * norm;
                b2 = (1.0 - std::sqrt(2.0) * K + K * K) * norm;
                a1 = 2.0 * (V * K * K - 1.0) * norm;
                a2 = (1.0 - std::sqrt(2.0*V) * K + V * K * K) * norm;
            }
            break;
        case HIGHSHELF:
            if (peakGain >= 0.0) {    // boost
                norm = 1.0 / (1.0 + std::sqrt(2.0) * K + K * K);
                b0 = (V + std::sqrt(2.0*V) * K + K * K) * norm;
                b1 = 2.0 * (K * K - V) * norm;
                b2 = (V - std::sqrt(2.0*V) * K + K * K) * norm;
                a1 = 2.0 * (K * K - 1.0) * norm;
                a2 = (1.0 - std::sqrt(2.0) * K + K * K) * norm;
            }
            else {    // cut
                norm = 1.0 / (V + std::sqrt(2.0*V) * K + K * K);
                b0 = (1.0 + std::sqrt(2.0) * K + K * K) * norm;
                b1 = 2.0 * (K * K - 1.0) * norm;
                b2 = (1.0 - std::sqrt(2.0) * K + K * K) * norm;
                a1 = 2.0 * (K * K - V) * norm;
                a2 = (V - std::sqrt(2.0*V) * K + K * K) * norm;
            }
            break;
    }
    std::vector<float> coefficients {b0, b1, b2, 0.0, a1, a2};

    return coefficients;
}