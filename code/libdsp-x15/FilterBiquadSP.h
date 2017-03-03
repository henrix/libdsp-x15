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

#ifndef LIBDSP_X15_FILTERBIQUADSP_H
#define LIBDSP_X15_FILTERBIQUADSP_H

#include "DspTask.h"

class FilterBiquadSP : public DspTask {
    friend class DspTaskFactory;
private:
    FilterBiquadSP(unsigned int N, std::function<void(DspTask &task)> callback, std::shared_ptr<cl::Context> clContext,
                   std::shared_ptr<cl::Program> clProgram) : DspTask(DspTask::_createId(), DspTask::FILTER_BIQUAD, callback, clContext, clProgram) {}
    ~FilterBiquadSP(){}
};

#endif //LIBDSP_X15_FILTERBIQUADSP_H
