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

#include "CallbackResponse.hpp"

CallbackResponse::CallbackResponse() : _op(CallbackResponse::UNDEFINED), _dataSize(0), _dataPtr(0) {

}


CallbackResponse::CallbackResponse(Ops op, unsigned int dataSize, float *dataPtr) {
	_op = op;
	_dataSize = dataSize;
	_dataPtr = dataPtr;
}

CallbackResponse::~CallbackResponse() {

}

CallbackResponse::Ops CallbackResponse::getOp() const {
	return _op;
}

void CallbackResponse::setOp(Ops op){
	_op = op;
}

unsigned int CallbackResponse::getDataSize() const {
	return _dataSize;
}

void CallbackResponse::setDataSize(unsigned int size) {
	_dataSize = size;
}

float* CallbackResponse::getDataPtr() const {
	return _dataPtr;
}

void CallbackResponse::setDataPtr(float *data){
	_dataPtr = data;
}
