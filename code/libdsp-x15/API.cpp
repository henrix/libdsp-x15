/***********************************************************************
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

#define PAD 0
#define __CL_ENABLE_EXCEPTIONS

#include "API.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <CL/cl.hpp>
#include <ocl_util.h>

std::function<void(CallbackResponse *clRes)> API::_callback = NULL;

class APIImpl {
public:
    APIImpl(cl::Context *context) : clContext(context){}

    /* OpenCL stuff */
    std::unique_ptr<cl::Context> clContext;
    std::unique_ptr<cl::CommandQueue> queueFFT, queueIFFT;
    std::unique_ptr<cl::Program> clProgram;
    std::unique_ptr<cl::Kernel> kernelFFT, kernelIFFT;
    std::unique_ptr<cl::Buffer> clBufXFFT, clBufYFFT, clBufWFFT;
    std::unique_ptr<cl::Buffer> clBufXIFFT, clBufYIFFT, clBufWIFFT;
};


API::API(std::function<void(CallbackResponse *clRes)> callback, bool debug)
    : _ptrImpl(new APIImpl(new cl::Context(CL_DEVICE_TYPE_ACCELERATOR))),
    _nFFT(0), _nIFFT(0), _bufSizeFFT(0), _bufSizeIFFT(0),
    _bufXFFT(NULL), _bufYFFT(NULL), _bufXIFFT(NULL), _bufYIFFT(NULL),
    _debug(debug)
{
    _callback = callback;
    std::vector<cl::Device> devices = _ptrImpl->clContext->getInfo<CL_CONTEXT_DEVICES>();
    int num;
    devices[0].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &num);
    std::cout << "Found " << num << " DSP compute cores." << std::endl;

    std::ifstream t("../libdsp-x15/audiokernel.cl");
    if (!t) {
        std::cerr << "Error Opening Kernel Source file\n"; exit(-1);
    }
    std::string kSrc((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    cl::Program::Sources source(1, std::make_pair(kSrc.c_str(),kSrc.length()));
    _ptrImpl->clProgram = std::unique_ptr<cl::Program>(new cl::Program(*_ptrImpl->clContext, source));
    _ptrImpl->clProgram->build(devices, "../libdsp-x15/dsplib.ae66");

    _ptrImpl->queueFFT = std::unique_ptr<cl::CommandQueue>(new cl::CommandQueue(*_ptrImpl->clContext, devices[0], CL_QUEUE_PROFILING_ENABLE));
    _ptrImpl->queueIFFT = std::unique_ptr<cl::CommandQueue>(new cl::CommandQueue(*_ptrImpl->clContext, devices[0], CL_QUEUE_PROFILING_ENABLE));
}
API::~API(){
    if (_bufXFFT)
        __free_ddr(_bufXFFT);
    if (_bufYFFT)
        __free_ddr(_bufYFFT);
    if (_bufWFFT)
        __free_ddr(_bufWFFT);
}
void API::prepareFFT(size_t N, int n_min, int n_max){
    _nFFT = N;
    _bufSizeFFT = sizeof(float) * (2*_nFFT + PAD + PAD);

    /*if (_bufXFFT)
        __free_ddr(_bufXFFT);
    if (_bufYFFT)
        __free_ddr(_bufYFFT);
    if (_bufWFFT)
        __free_ddr(_bufWFFT);*/
    _bufXFFT = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
    _bufYFFT = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
    _bufWFFT = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
    _genTwiddles(CallbackResponse::FFT, _nFFT, _bufWFFT);
    _ptrImpl->clBufXFFT = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _bufXFFT));
    _ptrImpl->clBufYFFT = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _bufYFFT));
    _ptrImpl->clBufWFFT = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _bufWFFT));

    _ptrImpl->kernelFFT = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_fftSPxSP"));
    _ptrImpl->kernelFFT->setArg(0, _nFFT);
    _ptrImpl->kernelFFT->setArg(1, *_ptrImpl->clBufXFFT);
    _ptrImpl->kernelFFT->setArg(2, *_ptrImpl->clBufWFFT);
    _ptrImpl->kernelFFT->setArg(3, *_ptrImpl->clBufYFFT);
    _ptrImpl->kernelFFT->setArg(4, n_min);
    _ptrImpl->kernelFFT->setArg(5, n_max);
}
void API::prepareIFFT(size_t N, int n_min, int n_max){
    _nIFFT = N;
    _bufSizeIFFT = sizeof(float) * (2*_nIFFT + PAD + PAD);

    /*if (_bufXFFT)
        __free_ddr(_bufXFFT);
    if (_bufYFFT)
        __free_ddr(_bufYFFT);
    if (_bufWFFT)
        __free_ddr(_bufWFFT);*/
    _bufXIFFT = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
    _bufYIFFT = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
    _bufWIFFT = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
    _genTwiddles(CallbackResponse::IFFT, _nIFFT, _bufWIFFT);
    _ptrImpl->clBufXIFFT = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _bufXIFFT));
    _ptrImpl->clBufYIFFT = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _bufYIFFT));
    _ptrImpl->clBufWIFFT = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _bufWIFFT));

    _ptrImpl->kernelIFFT = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_ifftSPxSP"));
    _ptrImpl->kernelIFFT->setArg(0, _nIFFT);
    _ptrImpl->kernelIFFT->setArg(1, *_ptrImpl->clBufXIFFT);
    _ptrImpl->kernelIFFT->setArg(2, *_ptrImpl->clBufWIFFT);
    _ptrImpl->kernelIFFT->setArg(3, *_ptrImpl->clBufYIFFT);
    _ptrImpl->kernelIFFT->setArg(4, n_min);
    _ptrImpl->kernelIFFT->setArg(5, n_max);
}
void API::setCallback(std::function<void(CallbackResponse *clRes)> callback){
    _callback = callback;
}
float* API::getBufX(CallbackResponse::Ops op){
    switch(op){
        case CallbackResponse::FFT:
            return _bufXFFT;
        break;
        case CallbackResponse::IFFT:
            return _bufXIFFT;
        break;
    }
}
float* API::getBufY(CallbackResponse::Ops op){
    switch(op){
        case CallbackResponse::FFT:
            return _bufYFFT;
        break;
        case CallbackResponse::IFFT:
            return _bufYIFFT;
        break;
    }
}
void API::setDebug(const bool debug){
    _debug = debug;
}


void* API::_allocBuffer(size_t size){
    return __malloc_ddr(size);
}
void API::_genTwiddles(CallbackResponse::Ops op, int n, float *w){
    int i, j, k;
    const double PI = 3.141592654;

    switch(op){
    case CallbackResponse::FFT:
        for (j = 1, k = 0; j <= n >> 2; j = j << 2)
        {
            for (i = 0; i < n >> 2; i += j)
            {
                w[k]     = (float) sin (2 * PI * i / n);
                w[k + 1] = (float) cos (2 * PI * i / n);
                w[k + 2] = (float) sin (4 * PI * i / n);
                w[k + 3] = (float) cos (4 * PI * i / n);
                w[k + 4] = (float) sin (6 * PI * i / n);
                w[k + 5] = (float) cos (6 * PI * i / n);
                k += 6;
            }
        }
        break;

    case CallbackResponse::IFFT:
        for (j = 1, k = 0; j <= n >> 2; j = j << 2)
        {
            for (i = 0; i < n >> 2; i += j)
            {
                w[k]     = (float) (-1)*sin (2 * PI * i / n);
                w[k + 1] = (float) cos (2 * PI * i / n);
                w[k + 2] = (float) (-1)*sin (4 * PI * i / n);
                w[k + 3] = (float) cos (4 * PI * i / n);
                w[k + 4] = (float) (-1)*sin (6 * PI * i / n);
                w[k + 5] = (float) cos (6 * PI * i / n);
                k += 6;
            }
        }
        break;

    default:
        break;
    }
}


/**
 * DSP operations
 */
void API::ocl_DSPF_sp_fftSPxSP(){
    try{
        cl::Event ev1;
        std::vector<cl::Event> evs(2);
        std::vector<cl::Event> evss(1);

        _ptrImpl->queueFFT->enqueueWriteBuffer(*_ptrImpl->clBufXFFT, CL_FALSE, 0, _bufSizeFFT, _bufXFFT, 0, &evs[0]);
        _ptrImpl->queueFFT->enqueueWriteBuffer(*_ptrImpl->clBufWFFT, CL_FALSE, 0, _bufSizeFFT, _bufWFFT, 0, &evs[1]);
        _ptrImpl->queueFFT->enqueueNDRangeKernel(*_ptrImpl->kernelFFT, cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->queueFFT->enqueueReadBuffer(*_ptrImpl->clBufYFFT, CL_TRUE, 0, _bufSizeFFT, _bufYFFT, &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(CallbackResponse::FFT, 2*_nFFT, _bufYFFT);
        auto lambda = [](cl_event ev, cl_int e_status, void *user_data) {
            CallbackResponse *res = (CallbackResponse*) user_data;
            _callback(res);
        };
        ev1.setCallback(CL_COMPLETE, lambda, clbkRes);

        if (_debug){
            ocl_event_times(evs[0], "Write X");
            ocl_event_times(evs[1], "Twiddle");
            ocl_event_times(evss[0], "FFT");
            ocl_event_times(ev1, "Read Y");
        }
    }
    catch (cl::Error &err)
    {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}
void API::ocl_DSPF_sp_ifftSPxSP(){
    try{
        cl::Event ev1;
        std::vector<cl::Event> evs(2);
        std::vector<cl::Event> evss(1);

        _ptrImpl->queueIFFT->enqueueWriteBuffer(*_ptrImpl->clBufXIFFT, CL_FALSE, 0, _bufSizeIFFT, _bufXIFFT, 0, &evs[0]);
        _ptrImpl->queueIFFT->enqueueWriteBuffer(*_ptrImpl->clBufWIFFT, CL_FALSE, 0, _bufSizeIFFT, _bufWIFFT, 0, &evs[1]);
        _ptrImpl->queueIFFT->enqueueNDRangeKernel(*_ptrImpl->kernelIFFT, cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->queueIFFT->enqueueReadBuffer(*_ptrImpl->clBufYIFFT, CL_TRUE, 0, _bufSizeIFFT, _bufYIFFT, &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(CallbackResponse::IFFT, 2*_nIFFT, _bufYIFFT);
        auto lambda = [](cl_event ev, cl_int e_status, void *user_data) {
            CallbackResponse *res = (CallbackResponse*) user_data;
            _callback(res);
        };
        ev1.setCallback(CL_COMPLETE, lambda, clbkRes);
        
        if (_debug){
            ocl_event_times(evs[0], "Write X");
            ocl_event_times(evs[1], "Twiddle");
            ocl_event_times(evss[0], "IFFT");
            ocl_event_times(ev1, "Read Y");
        }
    }
    catch (cl::Error &err)
    {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}
