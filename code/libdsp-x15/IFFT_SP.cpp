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

#include "IFFT_SP.h"
#include <iostream>

#define PAD 0

IFFT_SP::IFFT_SP(unsigned int N, std::function<void(DspTask &task)> callback,
                 std::shared_ptr<cl::Context> clContext, std::shared_ptr<cl::Program> clProgram)
        : DspTask(DspTask::_createId(), DspTask::IFFT, callback, clContext, clProgram),
          _buffer_size(sizeof(float) * (2 * N + PAD + PAD)), _N(N)
{
    /* allocate zero copy buffers in contiguous memory */
    _bufferInput = (float*) DspTask::_mallocBuffer(_buffer_size);
    _bufferTwiddle = (float*) DspTask::_mallocBuffer(_buffer_size); //TODO: check if twiddle factors really need 2*N buffer
    _bufferOutput = (float*) DspTask::_mallocBuffer(_buffer_size);

    IFFT_SP::generateTwiddleFactors(N, _bufferTwiddle);

    /* create OpenCL buffers from zero copy memory */
    _clBufferInput = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, _buffer_size, _bufferInput)
    );
    _clBufferTwiddle = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, _buffer_size, _bufferTwiddle)
    );
    _clBufferOutput = std::unique_ptr<cl::Buffer>(
            new cl::Buffer(*_clContext, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, _buffer_size, _bufferOutput)
    );

    /* create OpenCL kernel and assign buffers */
    _clKernel = std::make_shared<cl::Kernel>(*_clProgram, "ocl_DSPF_sp_ifftSPxSP");
    _clKernel->setArg(0, _N);
    _clKernel->setArg(1, *_clBufferInput);
    _clKernel->setArg(2, *_clBufferTwiddle);
    _clKernel->setArg(3, *_clBufferOutput);
    _clKernel->setArg(4, 4); //n_min
    _clKernel->setArg(5, _N); //n_max
}

IFFT_SP::~IFFT_SP(){
    DspTask::_freeBuffer(_bufferInput);
    DspTask::_freeBuffer(_bufferTwiddle);
    DspTask::_freeBuffer(_bufferOutput);
}

unsigned int IFFT_SP::getN(){
    return _N;
}

std::size_t IFFT_SP::getBufferSize(){
    return _buffer_size;
}

float* IFFT_SP::getInputBuffer(unsigned int index){
    if (index == 0)
        return _bufferInput;
    else {
        std::cout << "WARNING: requested non available input buffer in IFFT. returning nullptr" << std::endl;
        return nullptr;
    }
}

float* IFFT_SP::getOutputBuffer(unsigned int index){
    if (index == 0)
        return _bufferOutput;
    else {
        std::cout << "WARNING: requested non available output buffer in IFFT. returning nullptr" << std::endl;
        return nullptr;
    }
}

std::vector<cl::Event> IFFT_SP::_assignClInputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue){
    try{
        std::vector<cl::Event> finished(2);

        /*
         * blocking buffer i/o (otherwise buffer ptr can not be used by application anymore)
         * offset is 0
         */
        clCmdQueue->enqueueWriteBuffer(*_clBufferInput, CL_FALSE, 0, _buffer_size, _bufferInput, 0, &finished[0]);
        clCmdQueue->enqueueWriteBuffer(*_clBufferTwiddle, CL_FALSE, 0, _buffer_size, _bufferTwiddle, 0, &finished[1]);

        return finished;
    }
    catch(const cl::Error& err){
        std::cerr << "Error in ifft(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

std::vector<cl::Event> IFFT_SP::_assignClOutputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue,
                                            std::vector<cl::Event>& preEvents){
    try{
        std::vector<cl::Event> finished(1);
        clCmdQueue->enqueueReadBuffer(*_clBufferOutput, CL_TRUE, 0, _buffer_size, _bufferOutput, &preEvents, &finished[0]);

        return finished;
    }
    catch(const cl::Error& err){
        std::cerr << "Error in ifft(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

void IFFT_SP::setInputBuffer(std::vector<std::complex<float>>& values){
    if (values.size() != _N){
        std::cout << "WARNING: size of IFFT input value vector does not match N. aborting" << std::endl;
        return;
    }

    for (unsigned int i=0; i < _N; i++){
        _bufferInput[PAD + i*2] = values.at(i).real();
        _bufferInput[PAD + i*2 + 1] = values.at(i).imag();
    }
}

std::shared_ptr<std::vector<std::complex<float>>> IFFT_SP::getInputBufferCopy(){
    auto inputCopy = std::make_shared<std::vector<std::complex<float>>>(_N);

    for (unsigned int i=0; i < _N; i++)
        (*inputCopy)[i] = std::complex<float>(_bufferInput[PAD + i*2], _bufferInput[PAD + i*2 + 1]);

    return inputCopy;
}

std::shared_ptr<std::vector<std::complex<float>>> IFFT_SP::getOutputBufferCopy(){
    auto outputCopy = std::make_shared<std::vector<std::complex<float>>>(_N);

    for (unsigned int i=0; i < _N; i++)
        (*outputCopy)[i] = std::complex<float>(_bufferOutput[PAD + 2*i], _bufferOutput[PAD + 2*i + 1]);

    return outputCopy;
}

void IFFT_SP::generateTwiddleFactors(unsigned int N, float *buffer){
    unsigned int i, j, k;

    for (j = 1, k = 0; j <= N >> 2; j = j << 2) {
        for (i = 0; i < N >> 2; i += j) {
            buffer[k]     = (float) (-1.0)*sin (2.0 * M_PI * (float)i / (float)N);
            buffer[k + 1] = (float) cos (2.0 * M_PI * (float)i / (float)N);
            buffer[k + 2] = (float) (-1.0)*sin (4.0 * M_PI * (float)i / (float)N);
            buffer[k + 3] = (float) cos (4.0 * M_PI * (float)i / (float)N);
            buffer[k + 4] = (float) (-1.0)*sin (6.0 * M_PI * (float)i / (float)N);
            buffer[k + 5] = (float) cos (6.0 * M_PI * (float)i / (float)N);
            k += 6;
        }
    }
}