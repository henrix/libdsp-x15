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
#include <functional>
#include <memory>

class APIImpl;
class API {
public:
    API(std::function<void(CallbackResponse *clRes)> callback);
    ~API();
    void prepareFFT(size_t N, int n_min, int n_max);
    void prepareIFFT(size_t N, int n_min, int n_max);
    void setCallback(std::function<void(CallbackResponse *clRes)> callback); //Is executed for all operations
    float* getBufX(CallbackResponse::Ops op);
    float* getBufY(CallbackResponse::Ops op);

    /**
     * DSP operations
     */
    void ocl_DSPF_sp_fftSPxSP();
	void ocl_DSPF_sp_ifftSPxSP();

private:
    void* _allocBuffer(size_t size);
    static void _genTwiddles(CallbackResponse::Ops op, int n, float *w);

    static std::function<void(CallbackResponse *clRes)> _callback;
    std::unique_ptr<APIImpl> _ptrImpl;
    size_t _nFFT, _nIFFT;
    size_t _bufSizeFFT, _bufSizeIFFT;
    float *_bufXFFT, *_bufYFFT, *_bufWFFT;
    float *_bufXIFFT, *_bufYIFFT, *_bufWIFFT;
};

#endif //API
