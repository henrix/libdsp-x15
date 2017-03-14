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

#include "TaskProcessor.h"
#include <iostream>
#include <fstream>

TaskProcessor::TaskProcessor(bool debug) : _debug(debug){
    _clContext = std::make_shared<cl::Context>(CL_DEVICE_TYPE_ACCELERATOR);

    /* discover OpenCL devices and its compute cores */
    _clDevices = _clContext->getInfo<CL_CONTEXT_DEVICES>();
    _clDevices[0].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &_num_compute_cores);
    _num_devices = _clDevices.size();
    std::cout << "found " << _num_devices << " devices and " << _num_compute_cores
              << " compute cores." << std::endl;

    /* load OpenCL program */
    std::ifstream clProgPathStr(CL_PROGRAM_PATH);
    if (!clProgPathStr) {
        std::cerr << "Error Opening Kernel Source file\n";
        exit(-1);
    }
    std::string clProgSrcStr((std::istreambuf_iterator<char>(clProgPathStr)), std::istreambuf_iterator<char>());
    cl::Program::Sources clProgSrc(1, std::make_pair(clProgSrcStr.c_str(),clProgSrcStr.length()));
    _clProgram = std::make_shared<cl::Program>(*_clContext, clProgSrc);

    /* link TI DSPLIB and build OpenCL program */
    _clProgram->build(_clDevices, DSPLIB_PATH.c_str());

    /* create command queue for every compute unit */
    for (unsigned int i=0; i < _num_devices; i++)
        _clCommandQueue.push_back(std::make_shared<cl::CommandQueue>(*_clContext, _clDevices[i], CL_QUEUE_PROFILING_ENABLE));
}

TaskProcessor::~TaskProcessor(){

}

void TaskProcessor::enqueueTask(DspTask &task){
    try {
        /* get specific command queue for task */
        std::shared_ptr<cl::CommandQueue> clCmdQueue = _clCommandQueue[task.id % _num_devices];

        /* check if buffers has been assigned to command queue already */
        if (!_taskBuffersEnqueued[task.id]){

            /* assign input buffers to queue */
            std::vector<cl::Event> inputEvents = task._assignClInputBuffersToQueue(clCmdQueue);

            /* enqueue OpenCL kernel to command queue */
            std::vector<cl::Event> operationEvent(1);
            clCmdQueue->enqueueNDRangeKernel(*task._clKernel, cl::NullRange, cl::NDRange(1), cl::NDRange(1), &inputEvents, &operationEvent[0]);

            /* assign output buffers to queue */
            std::vector<cl::Event> finishedEvent = task._assignClOutputBuffersToQueue(clCmdQueue, operationEvent);

            /* create callback as lambda and bind to finished event */
            auto callbackHelper = [](cl_event ev, cl_int e_status, void *user_data) {
                DspTask* task_ = (DspTask*) user_data;
                task_->_callback(*task_);
            };
            finishedEvent[0].setCallback(CL_COMPLETE, callbackHelper, &task);

            /* output operation benchmark if debug is enabled */
            if (_debug)
                ocl_event_times(finishedEvent[0], task.getOperationName().c_str());

            _taskBuffersEnqueued[task.id] = true;
        }
        else {
            std::vector<cl::Event> finishedEvent(1);
            clCmdQueue->enqueueNDRangeKernel(*task._clKernel, cl::NullRange, cl::NDRange(1), cl::NDRange(1), 0, &finishedEvent[0]);

            auto callbackHelper = [](cl_event ev, cl_int e_status, void *user_data) {
                DspTask* task_ = (DspTask*) user_data;
                task_->_callback(*task_);
            };
            finishedEvent[0].setCallback(CL_COMPLETE, callbackHelper, &task);

            if (_debug)
                ocl_event_times(finishedEvent[0], task.getOperationName().c_str());
        }
    }
    catch(const cl::Error& err){
        std::cerr << "Error in taskprocessor(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}
