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

#include "library.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <CL/cl.hpp>
#include <ocl_util.h>

std::function<void(CallbackResponse *clbkRes)> API::_callback = NULL;
/* Flags to indicate status of DSP operations */
std::map<CallbackResponse::Ops, bool> API::_opBusy =
        {
                {CallbackResponse::FFT, false},
                {CallbackResponse::IFFT, false},
                {CallbackResponse::FILTER_BIQUAD, false},
                {CallbackResponse::FILTER_FIR_R2, false},
                {CallbackResponse::FILTER_IIR, false}
        };

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


API::API(std::function<void(CallbackResponse *clbkRes)> callback, bool debug)
        : _ptrImpl(new APIImpl(new cl::Context(CL_DEVICE_TYPE_ACCELERATOR))),
          _nFFT(0), _nIFFT(0), _bufSizeFFT(0), _bufSizeIFFT(0), _debug(debug)
{
    _callback = callback; //static

    _opPrepared[CallbackResponse::FFT] = false;
    _opPrepared[CallbackResponse::IFFT] = false;
    _opPrepared[CallbackResponse::FILTER_BIQUAD] = false;
    _opPrepared[CallbackResponse::FILTER_FIR_R2] = false;
    _opPrepared[CallbackResponse::FILTER_IIR] = false;

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
    if (_opPrepared.at(CallbackResponse::FFT)){
        _clean(CallbackResponse::FFT);
    }
    if (_opPrepared.at(CallbackResponse::IFFT)){
        _clean(CallbackResponse::IFFT);
    }
}
void API::setCallback(std::function<void(CallbackResponse *clRes)> callback){
    _callback = callback;
}
float* API::getBufIn(CallbackResponse::Ops op){
    switch(op){
        case CallbackResponse::FFT:
            return _buffers.at("FFT_X");
            break;
        case CallbackResponse::IFFT:
            return _buffers.at("IFFT_X");
            break;
        case CallbackResponse::FILTER_BIQUAD:
            return _buffers.at("FILTER_BIQUAD_X");
            break;
        case CallbackResponse::FILTER_FIR_R2:
            break;
        case CallbackResponse::FILTER_IIR:
            break;
    }
    return NULL;
}
float* API::getBufOut(CallbackResponse::Ops op){
    switch(op){
        case CallbackResponse::FFT:
            return _buffers.at("FFT_Y");
            break;
        case CallbackResponse::IFFT:
            return _buffers.at("IFFT_Y");
            break;
        case CallbackResponse::FILTER_BIQUAD:
            return _buffers.at("FILTER_BIQUAD_Y");
            break;
        case CallbackResponse::FILTER_FIR_R2:
            break;
        case CallbackResponse::FILTER_IIR:
            break;
    }
    return NULL;
}
bool API::isBusy(CallbackResponse::Ops op){
    switch(op){
        case CallbackResponse::FFT:
            return _opBusy.at(CallbackResponse::FFT);
            break;
        case CallbackResponse::IFFT:
            return _opBusy.at(CallbackResponse::IFFT);
            break;
        case CallbackResponse::FILTER_BIQUAD:
            return _opBusy.at(CallbackResponse::FILTER_BIQUAD);
            break;
        case CallbackResponse::FILTER_FIR_R2:
            return _opBusy.at(CallbackResponse::FILTER_FIR_R2);
            break;
        case CallbackResponse::FILTER_IIR:
            return _opBusy.at(CallbackResponse::FILTER_IIR);
            break;
    }
    return false;
}
void API::setDebug(const bool debug){
    _debug = debug;
}
void API::prepareFFT(int N, int n_min, int n_max){
    try{
        if (_opPrepared.at(CallbackResponse::FFT)){
            _clean(CallbackResponse::FFT);
        }

        _nFFT = N;
        _bufSizeFFT = sizeof(float) * (2*_nFFT + PAD + PAD);

        _buffers["FFT_X"] = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
        _buffers["FFT_W"] = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
        _buffers["FFT_Y"] = (float*) _allocBuffer(sizeof(float)*2*_nFFT);
        _brevFFT = (unsigned char*) _allocBuffer(sizeof(unsigned char)*64);

        _genTwiddles(CallbackResponse::FFT, _nFFT, _buffers.at("FFT_W"));
        _initBrev(_brevFFT);

        _ptrImpl->clBuffers["FFT_X"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _buffers.at("FFT_X")));
        _ptrImpl->clBuffers["FFT_W"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeFFT, _buffers.at("FFT_W")));
        _ptrImpl->clBuffers["FFT_Y"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,  _bufSizeFFT, _buffers.at("FFT_Y")));
        _ptrImpl->clBuffers["FFT_BREV"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  64*sizeof(unsigned char), _brevFFT));

        _ptrImpl->clKernels["FFT"] = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_fftSPxSP"));
        _ptrImpl->clKernels.at("FFT")->setArg(0, _nFFT);
        _ptrImpl->clKernels.at("FFT")->setArg(1, *_ptrImpl->clBuffers.at("FFT_X"));
        _ptrImpl->clKernels.at("FFT")->setArg(2, *_ptrImpl->clBuffers.at("FFT_W"));
        _ptrImpl->clKernels.at("FFT")->setArg(3, *_ptrImpl->clBuffers.at("FFT_Y"));
        _ptrImpl->clKernels.at("FFT")->setArg(4, n_min);
        _ptrImpl->clKernels.at("FFT")->setArg(5, n_max);
        //_ptrImpl->clKernels.at("FFT")->setArg(6, *_ptrImpl->clBuffers.at("FFT_BREV"));

        _opPrepared[CallbackResponse::FFT] = true;
    }
    catch(cl::Error &err){
        std::cout << "OpenCL error in prepareFFT: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    catch(const std::exception &err){
        std::cout << "Error in prepareFFT: " << err.what() << std::endl;
    }
}
void API::prepareIFFT(int N, int n_min, int n_max){
    try{
        if (_opPrepared.at(CallbackResponse::IFFT)){
            _clean(CallbackResponse::IFFT);
        }

        _nIFFT = N;
        _bufSizeIFFT = sizeof(float) * (2*_nIFFT + PAD + PAD);

        _buffers["IFFT_X"] = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
        _buffers["IFFT_W"] = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);
        _buffers["IFFT_Y"] = (float*) _allocBuffer(sizeof(float)*2*_nIFFT);

        _genTwiddles(CallbackResponse::IFFT, _nIFFT, _buffers.at("IFFT_W"));

        _ptrImpl->clBuffers["IFFT_X"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _buffers.at("IFFT_X")));
        _ptrImpl->clBuffers["IFFT_W"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufSizeIFFT, _buffers.at("IFFT_W")));
        _ptrImpl->clBuffers["IFFT_Y"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,  _bufSizeIFFT, _buffers.at("IFFT_Y")));

        _ptrImpl->clKernels["IFFT"] = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_ifftSPxSP"));
        _ptrImpl->clKernels.at("IFFT")->setArg(0, _nIFFT);
        _ptrImpl->clKernels.at("IFFT")->setArg(1, *_ptrImpl->clBuffers.at("IFFT_X"));
        _ptrImpl->clKernels.at("IFFT")->setArg(2, *_ptrImpl->clBuffers.at("IFFT_W"));
        _ptrImpl->clKernels.at("IFFT")->setArg(3, *_ptrImpl->clBuffers.at("IFFT_Y"));
        _ptrImpl->clKernels.at("IFFT")->setArg(4, n_min);
        _ptrImpl->clKernels.at("IFFT")->setArg(5, n_max);

        _opPrepared[CallbackResponse::IFFT] = true;
    }
    catch(cl::Error &err){
        std::cout << "OpenCL error in prepareIFFT: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    catch(const std::exception &err){
        std::cout << "Error in prepareIFFT: " << err.what() << std::endl;
    }
}
void API::prepareFILTER_BIQUAD(int nx){
    try{
        if (_opPrepared.at(CallbackResponse::FILTER_BIQUAD)){
            _clean(CallbackResponse::FILTER_BIQUAD);
        }

        _nxFILTER_BIQUAD = nx;

        _buffers["FILTER_BIQUAD_X"] = (float*) _allocBuffer(sizeof(float)*nx);
        _buffers["FILTER_BIQUAD_B"] = (float*) _allocBuffer(sizeof(float)*3);
        _buffers["FILTER_BIQUAD_A"] = (float*) _allocBuffer(sizeof(float)*2);
        _buffers["FILTER_BIQUAD_Y"] = (float*) _allocBuffer(sizeof(float)*nx);

        _ptrImpl->clBuffers["FILTER_BIQUAD_X"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, nx, _buffers.at("FILTER_BIQUAD_X")));
        _ptrImpl->clBuffers["FILTER_BIQUAD_B"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 3, _buffers.at("FILTER_BIQUAD_B")));
        _ptrImpl->clBuffers["FILTER_BIQUAD_A"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 2, _buffers.at("FILTER_BIQUAD_A")));
        _ptrImpl->clBuffers["FILTER_BIQUAD_Y"] = std::unique_ptr<cl::Buffer>(new cl::Buffer(*_ptrImpl->clContext, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, nx, _buffers.at("FILTER_BIQUAD_Y")));

        _ptrImpl->clKernels["FILTER_BIQUAD"] = std::unique_ptr<cl::Kernel>(new cl::Kernel(*_ptrImpl->clProgram, "ocl_DSPF_sp_biquad"));
        _ptrImpl->clKernels.at("FILTER_BIQUAD")->setArg(0, *_ptrImpl->clBuffers.at("FILTER_BIQUAD_X"));
        _ptrImpl->clKernels.at("FILTER_BIQUAD")->setArg(1, *_ptrImpl->clBuffers.at("FILTER_BIQUAD_B"));
        _ptrImpl->clKernels.at("FILTER_BIQUAD")->setArg(2, *_ptrImpl->clBuffers.at("FILTER_BIQUAD_A"));
        //_ptrImpl->clKernels.at("FILTER_BIQUAD")->setArg(3, *_ptrImpl->clBuffers.at("FILTER_BIQUAD_B")); //filter delays
        _ptrImpl->clKernels.at("FILTER_BIQUAD")->setArg(4, *_ptrImpl->clBuffers.at("FILTER_BIQUAD_Y"));
        _ptrImpl->clKernels.at("FILTER_BIQUAD")->setArg(5, nx);

        _opPrepared[CallbackResponse::FILTER_BIQUAD] = true;
    }
    catch(cl::Error &err){
        std::cout << "OpenCL error in prepareFILTER_BIQUAD: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    catch(const std::exception &err){
        std::cout << "Error in prepareFILTER_BIQUAD: " << err.what() << std::endl;
    }
}
void API::prepareFILTER_FIR_R2(int nh, int nr, float *h){

}
void API::prepareFILTER_IIR(){

}

void API::configFILTER_BIQUAD(float *b, float *a, float *delays){
    for (int i=0; i < 3; i++)
        _buffers.at("FILTER_BIQUAD_B")[i] = b[i];
    for (int i=0; i < 2; i++)
        _buffers.at("FILTER_BIQUAD_A")[i] = a[i];
}

void* API::_allocBuffer(size_t size){
    return __malloc_ddr(size);
}
void API::_genTwiddles(CallbackResponse::Ops op, int n, float *w){

    int i, j, k;
    double x_t, y_t, theta1, theta2, theta3;
    const double PI = 3.141592654;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {
            theta1 = 2 * PI * i / n;
            x_t = cos (theta1);
            y_t = sin (theta1);
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t;

            theta2 = 4 * PI * i / n;
            x_t = cos (theta2);
            y_t = sin (theta2);
            w[k + 2] = (float) x_t;
            w[k + 3] = (float) y_t;

            theta3 = 6 * PI * i / n;
            x_t = cos (theta3);
            y_t = sin (theta3);
            w[k + 4] = (float) x_t;
            w[k + 5] = (float) y_t;
            k += 6;
        }
    }
}

void API::_initBrev(unsigned char *brev){
    unsigned char tmp[64] = {
            0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
            0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
            0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
            0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
            0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
            0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
            0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
            0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
    };
    for (int i=0; i < 64; i++)
        brev[i] = tmp[i];
}

void API::_clean(CallbackResponse::Ops op){
    switch(op){
        case CallbackResponse::FFT:
        {
            __free_ddr(_buffers.at("FFT_X"));
            __free_ddr(_buffers.at("FFT_W"));
            __free_ddr(_buffers.at("FFT_Y"));
            __free_ddr(_brevFFT);
        }
            break;
        case CallbackResponse::IFFT:
        {
            __free_ddr(_buffers.at("IFFT_X"));
            __free_ddr(_buffers.at("IFFT_W"));
            __free_ddr(_buffers.at("IFFT_Y"));
        }
            break;
        case CallbackResponse::FILTER_BIQUAD:
        {
            __free_ddr(_buffers.at("FILTER_BIQUAD_X"));
            __free_ddr(_buffers.at("FILTER_BIQUAD_B"));
            __free_ddr(_buffers.at("FILTER_BIQUAD_A"));
            __free_ddr(_buffers.at("FILTER_BIQUAD_Y"));
        }
            break;
        case CallbackResponse::FILTER_FIR_R2:
        {

        }
            break;
        case CallbackResponse::FILTER_IIR:
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
        if (!_opPrepared.at(CallbackResponse::FFT)){
            std::cerr << "FFT operation not prepared yet! Stopping DSP operation." << std::endl;
            return;
        }
        _opBusy[CallbackResponse::FFT] = true;

        cl::Event ev1;
        std::vector<cl::Event> evs(3);
        std::vector<cl::Event> evss(1);

        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FFT_X"), CL_FALSE, 0, _bufSizeFFT, _buffers.at("FFT_X"), 0, &evs[0]);
        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FFT_W"), CL_FALSE, 0, _bufSizeFFT, _buffers.at("FFT_W"), 0, &evs[1]);
        //_ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FFT_BREV"), CL_FALSE, 0, sizeof(unsigned char) * 64, _brevFFT, 0, &evs[2]);
        _ptrImpl->clCmdQueue->enqueueNDRangeKernel(*_ptrImpl->clKernels.at("FFT"), cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->clCmdQueue->enqueueReadBuffer(*_ptrImpl->clBuffers.at("FFT_Y"), CL_TRUE, 0, _bufSizeFFT, _buffers.at("FFT_Y"), &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(CallbackResponse::FFT, 2*_nFFT, _buffers.at("FFT_Y"));
        auto lambda = [](cl_event ev, cl_int e_status, void *user_data) {
            CallbackResponse *res = (CallbackResponse*) user_data;
            _opBusy[CallbackResponse::FFT] = false;
            _callback(res);

        };
        ev1.setCallback(CL_COMPLETE, lambda, clbkRes);

        if (_debug){
            ocl_event_times(evs[0], "Write X");
            ocl_event_times(evs[1], "Twiddle");
            //ocl_event_times(evs[2], "Brev");
            ocl_event_times(evss[0], "FFT");
            ocl_event_times(ev1, "Read Y");
        }
    }
    catch (cl::Error &err)
    {
        std::cerr << "Error in fft(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}
void API::ocl_DSPF_sp_ifftSPxSP(){
    try{
        if (!_opPrepared.at(CallbackResponse::IFFT)){
            std::cerr << "IFFT operation not prepared yet! Stopping DSP operation." << std::endl;
            return;
        }
        _opBusy[CallbackResponse::IFFT] = true;

        cl::Event ev1;
        std::vector<cl::Event> evs(2);
        std::vector<cl::Event> evss(1);

        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("IFFT_X"), CL_FALSE, 0, _bufSizeIFFT, _buffers.at("IFFT_X"), 0, &evs[0]);
        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("IFFT_W"), CL_FALSE, 0, _bufSizeIFFT, _buffers.at("IFFT_W"), 0, &evs[1]);
        _ptrImpl->clCmdQueue->enqueueNDRangeKernel(*_ptrImpl->clKernels.at("IFFT"), cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->clCmdQueue->enqueueReadBuffer(*_ptrImpl->clBuffers.at("IFFT_Y"), CL_TRUE, 0, _bufSizeIFFT, _buffers.at("IFFT_Y"), &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(CallbackResponse::IFFT, 2*_nIFFT, _buffers.at("IFFT_Y"));
        auto lambda = [](cl_event ev, cl_int e_status, void *user_data) {
            CallbackResponse *res = (CallbackResponse*) user_data;
            _opBusy[CallbackResponse::IFFT] = false;
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
        std::cerr << "Error in ifft(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}
void API::ocl_DSPF_sp_filter_biquad(){
    try{
        if (!_opPrepared.at(CallbackResponse::FILTER_BIQUAD)){
            std::cerr << "Filter biquad not prepared yet! Stopping DSP operation." << std::endl;
            return;
        }
        _opBusy[CallbackResponse::FILTER_BIQUAD] = true;

        cl::Event ev1;
        std::vector<cl::Event> evs(3);
        std::vector<cl::Event> evss(1);

        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FILTER_BIQUAD_X"), CL_FALSE, 0, _nxFILTER_BIQUAD, _buffers.at("FILTER_BIQUAD_X"), 0, &evs[0]);
        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FILTER_BIQUAD_B"), CL_FALSE, 0, 3, _buffers.at("FILTER_BIQUAD_B"), 0, &evs[1]);
        _ptrImpl->clCmdQueue->enqueueWriteBuffer(*_ptrImpl->clBuffers.at("FILTER_BIQUAD_A"), CL_FALSE, 0, 2, _buffers.at("FILTER_BIQUAD_A"), 0, &evs[2]);
        _ptrImpl->clCmdQueue->enqueueNDRangeKernel(*_ptrImpl->clKernels.at("FILTER_BIQUAD"), cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _ptrImpl->clCmdQueue->enqueueReadBuffer(*_ptrImpl->clBuffers.at("FILTER_BIQUAD_Y"), CL_TRUE, 0, _nxFILTER_BIQUAD, _buffers.at("FILTER_BIQUAD_Y"), &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(CallbackResponse::FILTER_BIQUAD, _nxFILTER_BIQUAD, _buffers.at("FILTER_BIQUAD_Y"));
        auto lambda = [](cl_event ev, cl_int e_status, void *user_data) {
            CallbackResponse *res = (CallbackResponse*) user_data;
            _opBusy[CallbackResponse::FILTER_BIQUAD] = false;
            _callback(res);
        };
        ev1.setCallback(CL_COMPLETE, lambda, clbkRes);

        if (_debug){
            ocl_event_times(evs[0], "Write X");
            ocl_event_times(evs[1], "Write B");
            ocl_event_times(evs[2], "Write A");
            ocl_event_times(evss[0], "FILTER_BIQUAD");
            ocl_event_times(ev1, "Read Y");
        }
    }
    catch(cl::Error &err){
        std::cerr << "OpenCL error in filter_biquad(): " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    catch(const std::exception &err){
        std::cerr << "Error in filter_biquad(): " << err.what() << std::endl;
    }
}
