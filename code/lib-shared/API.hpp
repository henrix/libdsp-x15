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
    void prepareFFT(size_t N);
    void prepareIFFT(size_t N);
    void setCallback(std::function<void(CallbackResponse *clRes)> callback); //Is executed for all operations

    /**
     * DSP operations
     */
    void ocl_DSPF_sp_fftSPxSP(float *x, int n_min, int n_max);
	void ocl_DSPF_sp_ifftSPxSP(float *x, int n_min, int n_max);

private:
    void* _allocBuffer(size_t size);
    static void _genTwiddles(CallbackResponse::Ops op, int n, float *w);

    static std::function<void(CallbackResponse *clRes)> _callback;
    std::unique_ptr<APIImpl> _ptrImpl;
    size_t _nFFT, _nIFFT;
    float *_bufXFFT, *_bufYFFT, *_bufWFFT;
    float *_bufXIFFT, *_bufYIFFT, *_bufWIFFT;
    size_t _bufSizeFFT, _bufSizeIFFT;
};

#endif //API
