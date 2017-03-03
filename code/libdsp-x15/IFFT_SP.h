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

#ifndef LIBDSP_X15_IFFT_SP_H
#define LIBDSP_X15_IFFT_SP_H

#include "DspTask.h"
#include <memory>
#include <complex>
#include <vector>
#include <CL/cl.hpp>

class IFFT_SP : public DspTask{
    friend class DspTaskFactory;
public:
    virtual ~IFFT_SP();

    /* implementation of abstract base class */
    std::size_t getBufferSize();
    float* getInputBuffer(unsigned int index = 0);
    float* getOutputBuffer(unsigned int index = 0);

    unsigned int getN();

    /**
     * copies complex values to input buffer in correct format
     * @param values vector with complex samples in time domain
     */
    void setInputBuffer(std::vector<std::complex<float>>& values);

    /**
     * returns a copy of input buffer
     * @return vector with complex samples in time domain
     */
    std::shared_ptr<std::vector<std::complex<float>>> getInputBufferCopy();

    /**
     * returns a copy of output buffer
     * @return vector with complex samples in frequency domain
     */
    std::shared_ptr<std::vector<std::complex<float>>> getOutputBufferCopy();

    /**
     * helper method to generate twiddle factors for FFT.
     * this method is usually only used in library internal functions
     * @param N lenth of FFT
     * @param pointer to empty buffer
     */
    static void generateTwiddleFactors(unsigned int N, float *buffer);

private:
    IFFT_SP(unsigned int N, std::function<void(DspTask &task)> callback,
            std::shared_ptr<cl::Context> clContext, std::shared_ptr<cl::Program> clProgram);

    std::vector<cl::Event> _assignClInputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue);
    std::vector<cl::Event> _assignClOutputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue,
                                       std::vector<cl::Event>& preEvents);

    const unsigned int _N;
    const std::size_t _buffer_size;

    /* zero copy buffers in contiguous memory (cmem) */
    float *_bufferInput;
    float *_bufferTwiddle; //twiddle factors
    float *_bufferOutput;

    /* OpenCL buffers (abstraction layer for low level zero copy buffers) */
    std::unique_ptr<cl::Buffer> _clBufferInput;
    std::unique_ptr<cl::Buffer> _clBufferTwiddle;
    std::unique_ptr<cl::Buffer> _clBufferOutput;
};

#endif //LIBDSP_X15_IFFT_SP_H
