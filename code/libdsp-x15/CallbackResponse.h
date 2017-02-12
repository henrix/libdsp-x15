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

#ifndef LIBDSP_X15_CALLBACKRESPONSE_H
#define LIBDSP_X15_CALLBACKRESPONSE_H

#include <cstddef>

/**
 * @brief Class which encapsulates result and settings of specific DSP operation (is parameter of DSP callback)
 * @author Henrik Langer
 * @date 17.09.2016
 */
class CallbackResponse {

public:
    /**
     * Enum for all DSP operations
     */
    enum Ops {
        FFT,
        IFFT,
        FILTER_BIQUAD,
        FILTER_FIR_R2,
        FILTER_IIR
    };
    /** Constructs new CallbackResponse object
     * @param op DSP operation
      * @param dataSize Size of result data (varies from operation to operation (e.g. FFT/IFFT result is complex (double spaced))
      * @param dataPtr Pointer to result data of DSP operation
     */
    CallbackResponse(CallbackResponse::Ops op, unsigned int dataSize, float *dataPtr);
    ~CallbackResponse();
    /** Return type of DSP operation
     */
    CallbackResponse::Ops getOp() const;
    /** Returns size of result data
     */
    size_t getDataSize() const;
    /** Returns pointer to result data
     */
    float* getDataPtr() const;

private:
    CallbackResponse::Ops _op;
    unsigned int _dataSize;
    float *_dataPtr;
};

#endif //LIBDSP_X15_CALLBACKRESPONSE_H
