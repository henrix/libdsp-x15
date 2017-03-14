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

#ifndef LIBDSP_X15_TASKPROCESSOR_H
#define LIBDSP_X15_TASKPROCESSOR_H

#include "DspTask.h"
#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <map>
#include <CL/cl.hpp>

const std::string DSPLIB_PATH = "../libdsp-x15/dsplib.ae66";
const std::string CL_PROGRAM_PATH = "../libdsp-x15/audiokernel.cl";

/**
 * @brief manages command queue and is responsible for
 * correct DSP task execution.
 */
class TaskProcessor {
    friend class DspTaskFactory;
public:
    /**
     * constructs task processor
     * @param debug enables benchmark results and more verbose output
     */
    explicit TaskProcessor(bool debug = 0);
    ~TaskProcessor();

    /**
     * Enqueue a DSP task to command queue
     * @param task
     */
    void enqueueTask(DspTask &task);

private:
    const bool _debug;
    std::map<unsigned long, bool> _taskBuffersEnqueued;
    std::deque<DspTask*> _queue;
    std::vector<cl::Device> _clDevices;
    std::shared_ptr<cl::CommandQueue> _clCommandQueue;
    std::shared_ptr<cl::Context> _clContext;
    std::shared_ptr<cl::Program> _clProgram;
    std::vector<cl::Kernel> _clKernels;
};


#endif //LIBDSP_X15_TASKPROCESSOR_H
