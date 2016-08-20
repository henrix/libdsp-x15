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

#ifndef API_HPP_
#define API_HPP_

#include "CallbackResponse.hpp"
#include <map>
#include <functional>
#include <memory>
#include <array>

/*
    TODO:
    - Create more command queues for multicores
    - Implement filter operations
    - Create GUI for realtime plots
*/

class APIImpl;
/**
 * Class which offers API for DSP operations and helper functions
 * @author Henrik Langer
 * @date 17.09.2016
 */
class API {
public:
	/** Constructs DSP API
	 * @param callback Function pointer to callback
	 * @param debug Flag to enable debug outputs
	 */
    API(std::function<void(CallbackResponse *clbkRes)> callback, bool debug = false);
    ~API();
    /** Sets new callback
     * @param callback Function pointer to callback
     */
    void setCallback(std::function<void(CallbackResponse *clRes)> callback); //Is executed for all operations
    /** Returns pointer to input buffer (x) for DSP operations
     * @param op DSP operation 
     */
    float* getBufIn(CallbackResponse::Ops op);
    /** Returns pointer to output buffer (y) for DSP operations
     * @param op DSP operation
     */
    float* getBufOut(CallbackResponse::Ops op);
    /** Returns status of DSP operation
     * @param op DSP operation
     */
    bool isBusy(CallbackResponse::Ops op);
    /** Enables / disables debug outputs
     * @param debug flag
     */
    void setDebug(const bool debug);

    /*
     * DSP operation init helpers
     */

    /** Prepares FFT operation (e.g. creates and initialises input/output buffers)
     * @param N Length of Fast Fourier Transformation (FFT) in complex samples
     * @param n_min Should be 4 if N can be represented as Power of 4 else, n_min should be 2
     * @param n_max Size of FFT in complex samples
     */
    void prepareFFT(int N, int n_min, int n_max);
    /** Prepares IFFT operation
     * @see prepareFFT(int,int,int)
     */
    void prepareIFFT(int N, int n_min, int n_max);
    /** Prepares biquad filter operation
     * @param nx Number of input/output samples
     */
    void prepareFILTER_BIQUAD(int nx);
    /**  Prepares real FIR operation (not implemented yet)
     */
    void prepareFILTER_FIR_R2(int nh, int nr, float *h);
    /** Prepares IIR operation (not implemented yet)
     */
    void prepareFILTER_IIR();

    /*
     * DSP operation config helpers
     */

    /** Configures biquad filter with normalized filter coefficients
     * @param b Pointer to array with the 3 filter coefficients b0, b1, b2
     * @param a Pointer to array with the 2 filter coefficients a0, a1
     * @param delays Pointer to array with filter delays
     */
    void configFILTER_BIQUAD(float *b, float *a, float *delays);

    /*
     * DSP operations
     */

    /** Enqueues and executes FFT operation on DSPs
     */
    void ocl_DSPF_sp_fftSPxSP();
    /** Enqueues and executes IFFT operation on DSPs
     */
	void ocl_DSPF_sp_ifftSPxSP();
	/** Enqueues and executes biquad filter operation on DSPs
	 */
    void ocl_DSPF_sp_filter_biquad();
    /** Enqueues and executes real FIR
     */
    void ocl_DSPF_sp_fir_r2();
    /** Enqueues and executes IIR
     */
    void ocl_DSPF_sp_iir();
    //float ocl_DSPF_sp_maxval(float *x, int nx);

private:
    void* _allocBuffer(size_t size);
    static void _genTwiddles(CallbackResponse::Ops op, int n, float *w);
    void _clean(CallbackResponse::Ops op);

    std::map<CallbackResponse::Ops, std::unique_ptr<CallbackResponse>> _kernelConfigs;
    std::map<std::string, float*> _buffers;
    std::map<CallbackResponse::Ops, bool> _opPrepared;
    static std::map<CallbackResponse::Ops, bool> _opBusy;

    static std::function<void(CallbackResponse *clRes)> _callback;
    std::unique_ptr<APIImpl> _ptrImpl;
    size_t _nFFT, _nIFFT;
    size_t _nxFILTER_BIQUAD;
    size_t _bufSizeFFT, _bufSizeIFFT;
    bool _debug;

};

#endif //API
