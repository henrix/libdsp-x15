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

#include "AudioAPI.hpp"

#define	PAD	0

/*
    Function for generating Specialized sequence of twiddle factors
    (took from TI OpenCL (FFT) example)
*/
void AudioAPI::_twGen(float *w, int n) {
    int i, j, k;
    const double PI = 3.141592654;

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

void AudioAPI::completeFFTEvt(cl_event evt, cl_int type, void *user_data) {

}

AudioAPI::AudioAPI() : _N_fft(0), _N_ifft(0) {
	try {
        _context = new cl::Context(CL_DEVICE_TYPE_ACCELERATOR);
        std::vector<cl::Device> devices = _context->getInfo<CL_CONTEXT_DEVICES>();

        int num;
 		devices[0].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &num);
 		std::cout << "Found " << num << " DSP compute cores." << std::endl;

        std::ifstream t("audiokernel.cl");
        if (!t) { std::cout << "Error Opening Kernel Source file\n"; exit(-1); }

        std::string kSrc((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        cl::Program::Sources source(1, std::make_pair(kSrc.c_str(),kSrc.length()));
        _program = new cl::Program(*_context, source);
        _program->build(devices, "./dsplib.ae66");

        _Q = new cl::CommandQueue(*_context, devices[0], CL_QUEUE_PROFILING_ENABLE);
    }
    catch(cl::Error &err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

AudioAPI::~AudioAPI() {
	delete _bufX;
	delete _bufY;
	delete _bufW;
	delete _program;
	__free_ddr(_w);
}

int AudioAPI::ocl_DSPF_sp_fftSPxSP(int N, float *x,
		float *y, int n_min, int n_max) {

    try{
        if (N != _N_fft) {
            _N_fft = N;
            delete _fftKernel;
            delete _bufX;
            delete _bufY;
            delete _bufW;
            if (_w)
                __free_ddr(_w);

            _bufsize_fft = sizeof(float) * (2*_N_fft + PAD + PAD);
            _w = (float*) __malloc_ddr(sizeof(float)*2*_N_fft);

            //memset (y, 0xA5, sizeof (y));
            _twGen(_w, _N_fft);
            

            _bufX = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufsize_fft, x);
            _bufY = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, _bufsize_fft, y);
            _bufW = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufsize_fft, _w);

            _fftKernel = new cl::Kernel(*_program, "ocl_DSPF_sp_fftSPxSP");
            _fftKernel->setArg(0, _N_fft);
            _fftKernel->setArg(1, *_bufX);
            _fftKernel->setArg(2, *_bufW);
            _fftKernel->setArg(3, *_bufY);
            int rad = 4;
            _fftKernel->setArg(4, rad); //n_min
            _fftKernel->setArg(5, _N_fft); //n_max
        }

        cl::Event ev1, ev2;
        //ev2.setCallback(CL_COMPLETE, completeFFTEvt, NULL);
        std::vector<cl::Event> evs(2);
        _Q->enqueueWriteBuffer(*_bufX, CL_FALSE, 0, _bufsize_fft, x, 0, &evs[0]);
        _Q->enqueueWriteBuffer(*_bufW, CL_FALSE, 0, _bufsize_fft, _w, 0, &evs[1]);
        _Q->enqueueTask(*_fftKernel, &evs, &ev1);
        //ev1.wait();
        _Q->enqueueReadBuffer(*_bufY, CL_TRUE, 0, _bufsize_fft, y, 0, &ev2);
        //ev2.wait();
        ocl_event_times(evs[0], "Write X");
        ocl_event_times(evs[1], "Twiddle");
        ocl_event_times(ev1, "FFT");
        ocl_event_times(ev2, "Read Y");
    }
    catch (cl::Error &err)
    { std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl; }	
}

int AudioAPI::ocl_DSPF_sp_ifftSPxSP(int N, float *x, 
	float *y, int n_min, int n_max) {

}

void AudioAPI::data_callback(int id, int size, float *buf) {

}
