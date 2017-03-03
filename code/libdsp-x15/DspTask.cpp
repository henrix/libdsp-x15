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

#include "DspTask.h"

unsigned long DspTask::_id_counter = 0;

DspTask::DspTask(unsigned long id, OPERATION operation, std::function<void(DspTask &task)> callback,
                 std::shared_ptr<cl::Context> clContext, std::shared_ptr<cl::Program> clProgram) :
        id(id), operation(operation), _callback(callback), _clContext(clContext), _clProgram(clProgram)
{

}

unsigned long DspTask::_createId() {
    return _id_counter++;
}
void* DspTask::_mallocBuffer(std::size_t size){
    return __malloc_ddr(size);
};
void DspTask::_freeBuffer(void *ptr){
    __free_ddr(ptr);
}