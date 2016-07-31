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
    std::unique_ptr<cl::CommandQueue> clCmdQueue;
    std::unique_ptr<cl::Program> clProgram;
    std::map<std::string, std::unique_ptr<cl::Kernel>> clKernels;
    std::map<std::string, std::unique_ptr<cl::Buffer>> clBuffers;
};


API::API(std::function<void(CallbackResponse *clRes)> callback, bool debug)
    : _ptrImpl(new APIImpl(new cl::Context(CL_DEVICE_TYPE_ACCELERATOR))),
    _nFFT(0), _nIFFT(0), _bufSizeFFT(0), _bufSizeIFFT(0), _debug(debug)
{
    _callback = callback; //static

    _opPrepared[ConfigOps::FFT] = false;
    _opPrepared[ConfigOps::IFFT] = false;
    _opPrepared[ConfigOps::FILTER_BIQUAD] = false;
    _opPrepared[ConfigOps::FILTER_FIRCIRC] = false;
    _opPrepared[ConfigOps::FILTER_FIR_CPLX] = false;
    _opPrepared[ConfigOps::FILTER_FIR_GEN] = false;
    _opPrepared[ConfigOps::FILTER_FIR_R2] = false;
    _opPrepared[ConfigOps::FILTER_IIR] = false;
    _opPrepared[ConfigOps::FILTER_IIRLAT] = false;

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

    _ptrImpl->clCmdQueue = std::unique_ptr<cl::CommandQueue>(new cl::CommandQueue(*_ptrImpl->clContext, devices[0], CL_QUEUE_PROFILING_ENABLE));
}
API::~API(){
    if (_opPrepared.at(ConfigOps::FFT)){
        _clean(ConfigOps::FFT);
    }
    if (_opPrepared.at(ConfigOps::IFFT)){
        _clean(ConfigOps::IFFT);
    }
}
void API::setCallback(std::function<void(CallbackResponse *clRes)> callback){
    _callback = callback;
}
float* API::getBufIn(ConfigOps::Ops op){
    switch(op){
        case ConfigOps::FFT:
            return _buffers.at("FFT_X");
        break;
        case ConfigOps::IFFT:
            return _buffers.at("IFFT_X");
        break;
        case ConfigOps::FILTER_BIQUAD:
        break;
        case ConfigOps::FILTER_FIRCIRC:
        break;
        case ConfigOps::FILTER_FIR_CPLX:
        break;
        case ConfigOps::FILTER_FIR_GEN:
        break;
        case ConfigOps::FILTER_FIR_R2:
        break;
        case ConfigOps::FILTER_IIR:
        break;
        case ConfigOps::FILTER_IIRLAT:
        break;
    }
    return NULL;
}
float* API::getBufOut(ConfigOps::Ops op){
    switch(op){
        case ConfigOps::FFT:
            return _buffers.at("FFT_Y");
        break;
        case ConfigOps::IFFT:
            return _buffers.at("IFFT_Y");
        break;
        case ConfigOps::FILTER_BIQUAD:
        break;
        case ConfigOps::FILTER_FIRCIRC:
        break;
        case ConfigOps::FILTER_FIR_CPLX:
        break;
        case ConfigOps::FILTER_FIR_GEN:
        break;
        case ConfigOps::FILTER_FIR_R2:
        break;
        case ConfigOps::FILTER_IIR:
        break;
        case ConfigOps::FILTER_IIRLAT:
        break;
    }
    return NULL;
}
void API::setDebug(const bool debug){
    _debug = debug;
}
void API::prepareFFT(int N, int n_min, int n_max){
    if (_opPrepared.at(ConfigOps::FFT)){
        _clean(ConfigOps::FFT);
    }

    _nFFT = N;
    _bufSizeFFT = sizeof(float) * (2*_nFFT + PAD + PAD);
    /*_kernelConfigs[ConfigOps::FFT] = std::unique_ptr<ConfigOps>(new ConfigOps(ConfigOps::FFT));
    ConfigOps configFFT(ConfigOps::FFT);
    configFFT.setParam<int>("N", N);
    configFFT.setParam<int>("n_min", n_min);
    configFFT.setParam<int>("n_max", n_max);*/

    _buffers["FFT_X"] = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
    _buffers["FFT_W"] = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
    _buffers["FFT_Y"] = (float*) _allocBuffer(sizeof(float)*2*_nFFT);

    _genTwiddles(ConfigOps::FFT, _nFFT, _buffers.at("FFT_W"));

    _ptrImpl->clBuffers["FFT_X"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _buffers.at("FFT_X")));
    _ptrImpl->clBuffers["FFT_W"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _buffers.at("FFT_W")));
    _ptrImpl->clBuffers["FFT_Y"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _buffers.at("FFT_Y")));

    _ptrImpl->clKernels["FFT"] = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_fftSPxSP"));
    _ptrImpl->clKernels.at("FFT")->setArg(0, _nFFT);
    _ptrImpl->clKernels.at("FFT")->setArg(1, *_ptrImpl->clBuffers.at("FFT_X"));
    _ptrImpl->clKernels.at("FFT")->setArg(2, *_ptrImpl->clBuffers.at("FFT_W"));
    _ptrImpl->clKernels.at("FFT")->setArg(3, *_ptrImpl->clBuffers.at("FFT_Y"));
    _ptrImpl->clKernels.at("FFT")->setArg(4, n_min);
    _ptrImpl->clKernels.at("FFT")->setArg(5, n_max);

    _opPrepared[ConfigOps::FFT] = true;
}
void API::prepareIFFT(int N, int n_min, int n_max){
    if (_opPrepared.at(ConfigOps::IFFT)){
        _clean(ConfigOps::IFFT);
    }

    _nIFFT = N;
    _bufSizeIFFT = sizeof(float) * (2*_nIFFT + PAD + PAD);

    _buffers["IFFT_X"] = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
    _buffers["IFFT_W"] = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
    _buffers["IFFT_Y"] = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);

    _genTwiddles(ConfigOps::IFFT, _nIFFT, _buffers.at("IFFT_W"));

    _ptrImpl->clBuffers["IFFT_X"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _buffers.at("IFFT_X")));
    _ptrImpl->clBuffers["IFFT_W"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _buffers.at("IFFT_W")));
    _ptrImpl->clBuffers["IFFT_Y"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _buffers.at("IFFT_Y")));

    _ptrImpl->clKernels["IFFT"] = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_ifftSPxSP"));
    _ptrImpl->clKernels.at("IFFT")->setArg(0, _nIFFT);
    _ptrImpl->clKernels.at("IFFT")->setArg(1, *_ptrImpl->clBuffers.at("IFFT_X"));
    _ptrImpl->clKernels.at("IFFT")->setArg(2, *_ptrImpl->clBuffers.at("IFFT_W"));
    _ptrImpl->clKernels.at("IFFT")->setArg(3, *_ptrImpl->clBuffers.at("IFFT_Y"));
    _ptrImpl->clKernels.at("IFFT")->setArg(4, n_min);
    _ptrImpl->clKernels.at("IFFT")->setArg(5, n_max);

    _opPrepared[ConfigOps::IFFT] = true;
}
void API::prepareFILTER_BIQUAD(std::array<float, 3> b, std::array<float, 2> a, float delay[], int nx){
    if (_opPrepared.at(ConfigOps::FILTER_BIQUAD)){
        _clean(ConfigOps::FILTER_BIQUAD);
    }


}
void API::prepareFILTER_FIRCIRC(int csize, int nh, int ny){

}

void* API::_allocBuffer(size_t size){
    return __malloc_ddr(size);
}
void API::_genTwiddles(ConfigOps::Ops op, int n, float *w){
    int i, j, k;
    const double PI = 3.141592654;

    if (op == ConfigOps::FFT){
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
    }
    else if (op == ConfigOps::IFFT){
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
    }
}

void API::_clean(ConfigOps::Ops op){
    switch(op){
        case ConfigOps::FFT:
        {
            __free_ddr(_buffers.at("FFT_X"));
            __free_ddr(_buffers.at("FFT_W"));
            __free_ddr(_buffers.at("FFT_Y"));
        }
        break;
        case ConfigOps::IFFT:
        {
            __free_ddr(_buffers.at("IFFT_X"));
            __free_ddr(_buffers.at("IFFT_W"));
            __free_ddr(_buffers.at("IFFT_Y"));
        }
        break;
        case ConfigOps::FILTER_BIQUAD:
        {

        }
        break;
        case ConfigOps::FILTER_FIRCIRC:
        {

        }
        break;
        case ConfigOps::FILTER_FIR_CPLX:
        {

        }
        break;
        case ConfigOps::FILTER_FIR_GEN:
        {

        }
        break;
        case ConfigOps::FILTER_FIR_R2:
        {

        }
        break;
        case ConfigOps::FILTER_IIR:
        {

        }
        break;
        case ConfigOps::FILTER_IIRLAT:
        {

        }
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

        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FFT_X"), CL_FALSE, 0, _bufSizeFFT, _buffers.at("FFT_X"), 0, &evs[0]);
        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FFT_W"), CL_FALSE, 0, _bufSizeFFT, _buffers.at("FFT_W"), 0, &evs[1]);
        _ptrImpl->clCmdQueue->enqueueNDRangeKernel(*_ptrImpl->clKernels.at("FFT"), cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->clCmdQueue->enqueueReadBuffer(*_ptrImpl->clBuffers.at("FFT_Y"), CL_TRUE, 0, _bufSizeFFT, _buffers.at("FFT_Y"), &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(ConfigOps::FFT, 2*_nFFT, _buffers.at("FFT_Y"));
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

        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("IFFT_X"), CL_FALSE, 0, _bufSizeIFFT, _buffers.at("IFFT_X"), 0, &evs[0]);
        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("IFFT_W"), CL_FALSE, 0, _bufSizeIFFT, _buffers.at("IFFT_W"), 0, &evs[1]);
        _ptrImpl->clCmdQueue->enqueueNDRangeKernel(*_ptrImpl->clKernels.at("IFFT"), cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->clCmdQueue->enqueueReadBuffer(*_ptrImpl->clBuffers.at("IFFT_Y"), CL_TRUE, 0, _bufSizeIFFT, _buffers.at("IFFT_Y"), &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(ConfigOps::IFFT, 2*_nIFFT, _buffers.at("IFFT_Y"));
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
