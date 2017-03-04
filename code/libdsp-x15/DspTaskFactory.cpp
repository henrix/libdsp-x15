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

#include "DspTaskFactory.h"

DspTaskFactory* DspTaskFactory::_instance = nullptr;

DspTaskFactory& DspTaskFactory::getInstance(){
    if (_instance)
        return *_instance;
    else{
        _instance = new DspTaskFactory();
        return *_instance;
    }
}

FFT_SP* DspTaskFactory::createFFT_SP(unsigned int N, std::function<void(DspTask &task)> callback, TaskProcessor& processor){
    FFT_SP* fft = new FFT_SP(N, callback, processor._clContext, processor._clProgram);
    DspTask* task = fft;
    _tasks.push_back(task);

    return fft;
}

IFFT_SP* DspTaskFactory::createIFFT_SP(unsigned int N, std::function<void(DspTask &task)> callback, TaskProcessor& processor){
    IFFT_SP* ifft = new IFFT_SP(N, callback, processor._clContext, processor._clProgram);
    DspTask* task = ifft;
    _tasks.push_back(task);

    return ifft;
}

FilterBiquadSP* DspTaskFactory::createFilterBiquadSP(unsigned int length, std::function<void(DspTask &task)> callback, TaskProcessor& processor){
    FilterBiquadSP* filter = new FilterBiquadSP(length, callback, processor._clContext, processor._clProgram);
    DspTask* task = filter;
    _tasks.push_back(task);

    return filter;
}

DspTaskFactory::DspTaskFactory()
{

}

DspTaskFactory::~DspTaskFactory()
{
    for (unsigned int i=0; i < _tasks.size(); i++)
        delete _tasks.at(i);
}