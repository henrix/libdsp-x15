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
#include "CallbackResponse.hpp"

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

AudioAPI::AudioAPI() : _N_fft(0), _N_ifft(0) {
	try {
        _context = new cl::Context(CL_DEVICE_TYPE_ACCELERATOR);
        std::vector<cl::Device> devices = _context->getInfo<CL_CONTEXT_DEVICES>();

        int num;
 		devices[0].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &num);
 		std::cout << "Found " << num << " DSP compute cores." << std::endl;

        std::ifstream t("audiokernel.cl");
        if (!t) { std::cerr << "Error Opening Kernel Source file\n"; exit(-1); }

        std::string kSrc((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        cl::Program::Sources source(1, std::make_pair(kSrc.c_str(),kSrc.length()));
        _program = new cl::Program(*_context, source);
        _program->build(devices, "./dsplib.ae66");

        _Qfft = new cl::CommandQueue(*_context, devices[0], CL_QUEUE_PROFILING_ENABLE);
        _Qifft = new cl::CommandQueue(*_context, devices[0], CL_QUEUE_PROFILING_ENABLE);
    }
    catch(cl::Error &err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

AudioAPI::~AudioAPI() {
	delete _bufFFTX;
	delete _bufFFTY;
	delete _bufFFTW;
	delete _program;
	__free_ddr(_wFFT);
}

int AudioAPI::ocl_DSPF_sp_fftSPxSP(int N, float *x,
		float *y, int n_min, int n_max,
		void (*callback)(cl_event ev, cl_int e_status, void *user_data)) {

    try{
        if (N != _N_fft) {
            _N_fft = N;
            delete _fftKernel;
            delete _bufFFTX;
            delete _bufFFTY;
            delete _bufFFTW;

            if (_wFFT)
                __free_ddr(_wFFT);

            _bufsize_fft = sizeof(float) * (2*_N_fft + PAD + PAD);
            _wFFT = (float*) __malloc_ddr(sizeof(float)*2*_N_fft);

            _twGen(_wFFT, _N_fft);
            
            _bufFFTX = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufsize_fft, x);
            _bufFFTY = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, _bufsize_fft, y);
            _bufFFTW = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufsize_fft, _wFFT);

            _fftKernel = new cl::Kernel(*_program, "ocl_DSPF_sp_fftSPxSP");
            _fftKernel->setArg(0, _N_fft);
            _fftKernel->setArg(1, *_bufFFTX);
            _fftKernel->setArg(2, *_bufFFTW);
            _fftKernel->setArg(3, *_bufFFTY);
            int rad = 4;
            _fftKernel->setArg(4, rad); //n_min
            _fftKernel->setArg(5, _N_fft); //n_max
        }

        cl::Event ev1;
        std::vector<cl::Event> evs(2);
        std::vector<cl::Event> evss(1);
        _Qfft->enqueueWriteBuffer(*_bufFFTX, CL_FALSE, 0, _bufsize_fft, x, 0, &evs[0]);
        _Qfft->enqueueWriteBuffer(*_bufFFTW, CL_FALSE, 0, _bufsize_fft, _wFFT, 0, &evs[1]);
        _Qfft->enqueueNDRangeKernel(*_fftKernel, cl::NullRange, cl::NDRange(1), cl::NDRange(1), &evs, &evss[0]);
        _Qfft->enqueueReadBuffer(*_bufFFTY, CL_TRUE, 0, _bufsize_fft, y, &evss, &ev1);

        CallbackResponse *clbkRes = new CallbackResponse(FFT, 2*_N_fft, y);
        ev1.setCallback(CL_COMPLETE, callback, clbkRes);

        //ocl_event_times(evs[0], "Write X");
        //ocl_event_times(evs[1], "Twiddle");
        ocl_event_times(ev1, "FFT");
        //ocl_event_times(ev2, "Read Y");

        //TODO: Generate and return ID for referencing task in callback
    }
    catch (cl::Error &err)
    { std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl; }	
}

int AudioAPI::ocl_DSPF_sp_ifftSPxSP(int N, float *x, 
	float *y, int n_min, int n_max,
	void (*callback)(cl_event ev, cl_int e_status, void *user_data)) {

	try{
		if (N != _N_ifft) {
			_N_ifft = N;
	        delete _ifftKernel;
	        delete _bufIFFTX;
	        delete _bufIFFTY;
	        //delete _bufIFFTW;

	        if (_wIFFT)
	            __free_ddr(_wIFFT);

	        _bufsize_ifft = sizeof(float) * (2*_N_ifft + PAD + PAD);
	        _wIFFT = (float*) __malloc_ddr(sizeof(float)*2*_N_ifft);

	        _twGen(_wIFFT, _N_ifft);

	        _bufIFFTX = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufsize_ifft, x);
	        _bufIFFTY = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, _bufsize_ifft, y);
	        _bufIFFTW = new cl::Buffer(*_context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  _bufsize_ifft, _wIFFT);

	        _ifftKernel = new cl::Kernel(*_program, "ocl_DSPF_sp_ifftSPxSP");
	        _ifftKernel->setArg(0, _N_ifft);
	        _ifftKernel->setArg(1, *_bufIFFTX);
	        _ifftKernel->setArg(2, *_bufIFFTW);
	        _ifftKernel->setArg(3, *_bufIFFTY);
	        int rad = 4;
	        _ifftKernel->setArg(4, rad); //n_min
	        _ifftKernel->setArg(5, _N_ifft); //n_max
	    }

	    cl::Event ev1;
	    std::vector<cl::Event> evs(2);
	    std::vector<cl::Event> evss(1);
	    _Qifft->enqueueWriteBuffer(*_bufIFFTX, CL_FALSE, 0, _bufsize_ifft, x, 0, &evs[0]);
	    _Qifft->enqueueWriteBuffer(*_bufIFFTW, CL_FALSE, 0, _bufsize_ifft, _wIFFT, 0, &evs[1]);
	    _Qifft->enqueueNDRangeKernel(*_ifftKernel, cl::NullRange, cl::NDRange(64), cl::NDRange(1), &evs, &evss[0]);
	    _Qifft->enqueueReadBuffer(*_bufIFFTY, CL_TRUE, 0, _bufsize_ifft, y, &evss, &ev1);

	    CallbackResponse *clbkRes = new CallbackResponse(IFFT, 2*_N_ifft, y);
	    ev1.setCallback(CL_COMPLETE, callback, clbkRes);

	    //ocl_event_times(evs[0], "Write X");
	    //ocl_event_times(evs[1], "Twiddle");
	    ocl_event_times(ev1, "IFFT");
	    //ocl_event_times(ev2, "Read Y");

	    //TODO: Generate and return ID for referencing task in callback
	}
	catch (cl::Error &err)
	{ std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl; }
}
