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

#ifndef LIBDSP_X15_DSPTASK_H
#define LIBDSP_X15_DSPTASK_H

#include <memory>
#include <functional>
#include <ocl_util.h>

/**
 * @brief represents abstract DSP task. all task implementations
 * must inherit from this class.
 */
class DspTask {
    friend class TaskProcessor;
    friend class DspTaskFactory;
public:
    /** all available DSP operations. new operations must be added here. */
    enum OPERATION {FFT, IFFT, FILTER_BIQUAD};

    /** unique DSP task id */
    const unsigned long id;

    /** type of DSP operation */
    const OPERATION operation;

    /**
     * returns the actual buffer size of DSP operation
     * (assumed that input, output and internal buffers have equal size)
     * @return buffer size in bytes
     */
    virtual std::size_t getBufferSize() = 0;

    /**
     * returns zero copy input buffer of DSP operation in contiguous memory (DDR)
     * ATTENTION: every DSP operation buffer has its own data layout.
     * @param index specific input buffer index of DSP operation
     * @return pointer to allocated memory
     */
    virtual float* getInputBuffer(unsigned int index = 0) = 0;
    /**
     * returns zero copy output buffer of DSP operation in contiguous memory (DDR)
     * ATTENTION: every DSP operation buffer has its own data layout.
     * @param index specific output buffer index of DSP operation
     * @return
     */
    virtual float* getOutputBuffer(unsigned int index = 0) = 0;

    /**
     * returns operation name (enum to string)
     * @return
     */
    virtual std::string getOperationName();

protected:
    /**
     * abstract constructor (is only called by factory)
     * @param id unique task id
     * @param operation DSP operation
     * @param callback callback function
     * @param clContext OpenCL context (dependency)
     * @param clProgram OpenCL program (dependency)
     */
    explicit DspTask(unsigned long id, OPERATION operation, std::function<void(DspTask &task)> callback,
                     std::shared_ptr<cl::Context> clContext, std::shared_ptr<cl::Program> clProgram);

    /*
     * all derived DSP tasks must implement these methods to add its buffers to OpenCL
     * command queue. tasks are responsible for data. task processor is responsible
     * for execution.
     */
    /**
     * assigns OpenCL input buffers to command queue.
     * all derived DSP tasks must implement this method.
     * tasks are responsible for data.
     * task processor is responsible for execution.
     * @param clCmdQueue OpenCL command queue
     * @return vector with events (triggered when buffers has been assigned to command queue)
     */
    virtual std::vector<cl::Event> _assignClInputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue) = 0;

    /**
     * assigns OpenCL output buffers to command queue.
     * all derived DSP tasks must implement this method.
     * tasks are responsible for data.
     * task processor is responsible for execution.
     * @param clCmdQueue OpenCL command queue
     * @return vector with events (triggered when buffers has been assigned to command queue)
     */
    virtual std::vector<cl::Event> _assignClOutputBuffersToQueue(std::shared_ptr<cl::CommandQueue> clCmdQueue,
                                                                  std::vector<cl::Event>& preEvents) = 0;

    std::function<void(DspTask &task)> _callback;

    /* OpenCL kernel */
    std::shared_ptr<cl::Kernel> _clKernel;

    /* OpenCL dependencies */
    std::shared_ptr<cl::Context> _clContext;
    std::shared_ptr<cl::Program> _clProgram;

    /**
     * generates a unique task id
     * @return
     */
    static unsigned long _createId();

    /**
     * allocates buffer in contiguous memory
     * @param size size in bytes
     * @return pointer to allocated memory
     */
    static void* _mallocBuffer(std::size_t size);

    /**
     * frees buffer in contiguous memory
     * @param pointer to allocated memory
     */
    static void _freeBuffer(void *ptr);

private:
    static unsigned long _id_counter;
};

#endif //LIBDSP_X15_DSPTASK_H
