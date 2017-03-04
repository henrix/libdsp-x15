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

#ifndef LIBDSP_X15_FILTERBIQUADSP_H
#define LIBDSP_X15_FILTERBIQUADSP_H

#include "DspTask.h"

class FilterBiquadSP : public DspTask {
    friend class DspTaskFactory;
public:
    /** all available filter types */
    enum TYPE {LOWPASS, HIGHPASS, BANDPASS, NOTCH, PEAK, LOWSHELF, HIGHSHELF};

    virtual ~FilterBiquadSP();

    /* implementation of abstract base class */
    std::size_t getBufferSize();
    float* getInputBuffer(unsigned int index = 0);
    float* getOutputBuffer(unsigned int index = 0);

    /**
     * returns the biquad filter length
     * @return
     */
    unsigned int getLength();

    /**
     * configures filter using user defined parameters
     * @param type filter type
     * @param Fc cutoff frequency
     * @param Fs sample rate
     * @param Q quality factor
     * @param peakGain gain
     */
    void configFilter(TYPE type, float Fc, float Fs, float Q, float peakGain);

    /**
     * configures filter using normalized forward and feedback coefficients
     * @param b forward coefficients (b0, b1, b2)
     * @param a feedback coefficients (a0, a1, a2). a0 is not used.
     */
    void configFilter(std::vector<float>& b, std::vector<float>& a);

    /**
     * calculates normalized coefficients from user defined parameters
     * @param type filter type
     * @param Fc cutoff frequency
     * @param Fs sample rate
     * @param Q quality factor
     * @param peakGain gain
     * @return vector with coefficients (b0, b1, b2, a0, a1, a2)
     */
    static std::vector<float> calcCoefficients(TYPE type, float Fc, float Fs, float Q, float peakGain);
private:
    FilterBiquadSP(unsigned int length, std::function<void(DspTask &task)> callback,
                   std::shared_ptr<cl::Context> clContext, std::shared_ptr<cl::Program> clProgram);

    std::vector<cl::Event> _assignClInputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue);
    std::vector<cl::Event> _assignClOutputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue,
                                                         std::vector<cl::Event>& preEvents);

    /* properties of DSP operation */
    const unsigned int _length;
    const std::size_t _buffer_size;

    /* zero copy buffers in contiguous memory (cmem) */
    float *_bufferInput;
    float *_bufferB; //forward coefficients
    float *_bufferA; //feedback coefficients
    float *_bufferDelay; //delay coefficients
    float *_bufferOutput;

    /* OpenCL buffers (abstraction layer for low level zero copy buffers) */
    std::unique_ptr<cl::Buffer> _clBufferInput;
    std::unique_ptr<cl::Buffer> _clBufferB;
    std::unique_ptr<cl::Buffer> _clBufferA;
    std::unique_ptr<cl::Buffer> _clBufferDelay;
    std::unique_ptr<cl::Buffer> _clBufferOutput;
};

#endif //LIBDSP_X15_FILTERBIQUADSP_H
