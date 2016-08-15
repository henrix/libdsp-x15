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
#include "ConfigOps.hpp"
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
class API {
public:
    API(std::function<void(CallbackResponse *clRes)> callback, bool debug = false);
    ~API();
    void setCallback(std::function<void(CallbackResponse *clRes)> callback); //Is executed for all operations
    float* getBufIn(ConfigOps::Ops op);
    float* getBufOut(ConfigOps::Ops op);
    bool isBusy(ConfigOps::Ops op);
    void setDebug(const bool debug);

    /**
     * DSP operation init helpers
     */
    void prepareFFT(int N, int n_min, int n_max);
    void prepareIFFT(int N, int n_min, int n_max);
    void prepareFILTER_BIQUAD(int nx);
    void prepareFILTER_FIR_R2(int nh, int nr, float *h);
    void prepareFILTER_IIR();

    /**
     * DSP operation config helpers
     */
    void configFILTER_BIQUAD(float *b, float *a, float *delays);

    /**
     * DSP operations
     */
    void ocl_DSPF_sp_fftSPxSP();
	void ocl_DSPF_sp_ifftSPxSP();
    void ocl_DSPF_sp_filter_biquad();
    void ocl_foo();
    //float ocl_DSPF_sp_maxval(float *x, int nx);

private:
    void* _allocBuffer(size_t size);
    static void _genTwiddles(ConfigOps::Ops op, int n, float *w);
    void _clean(ConfigOps::Ops op);

    std::map<ConfigOps::Ops, std::unique_ptr<ConfigOps>> _kernelConfigs;
    std::map<std::string, float*> _buffers;
    std::map<ConfigOps::Ops, bool> _opPrepared;
    static std::map<ConfigOps::Ops, bool> _opBusy;

    static std::function<void(CallbackResponse *clRes)> _callback;
    std::unique_ptr<APIImpl> _ptrImpl;
    size_t _nFFT, _nIFFT;
    size_t _nxFILTER_BIQUAD;
    size_t _bufSizeFFT, _bufSizeIFFT;
    bool _debug;

};

#endif //API
