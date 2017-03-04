/***********************************************************************
 * Author: Henrik Langer (henni19790@gmail.com)
 * Copyright: Henrik Langer
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

#ifndef LIBDSP_X15_DSPTASKFACTORY_H
#define LIBDSP_X15_DSPTASKFACTORY_H

/* implemented DSP operations */
#include "FFT_SP.h"
#include "IFFT_SP.h"
#include "FilterBiquadSP.h"

#include <memory>

/**
 * @brief factory (singleton) to construct DspTasks.
 * factory injects required OpenCL runtime objects.
 */
class DspTaskFactory {
public:

    /**
     * returns singleton instance of factory
     * @return
     */
    static DspTaskFactory& getInstance();

    /**
     * creates FFT DSP task
     * @param N block size of FFT
     * @param callback is called after DSP operation has been finished
     * @param processor
     * @return
     */
    FFT_SP* createFFT_SP(unsigned int N, std::function<void(DspTask &task)> callback, TaskProcessor& processor);

    /**
     * creates IFFT DSP task
     * @param N block size of IFFT
     * @param callback is called after DSP operation has been finished
     * @param processor
     * @return
     */
    IFFT_SP* createIFFT_SP(unsigned int N, std::function<void(DspTask &task)> callback, TaskProcessor& processor);

    /**
     * creates biquad filter DSP task
     * @param length length of filter
     * @param callback is called after DSP operation has been finished
     * @param processor
     * @return
     */
    FilterBiquadSP* createFilterBiquadSP(unsigned int length, std::function<void(DspTask &task)> callback, TaskProcessor& processor);

private:
    DspTaskFactory();
    ~DspTaskFactory();

    static DspTaskFactory* _instance;
};


#endif //LIBDSP_X15_DSPTASKFACTORY_H
