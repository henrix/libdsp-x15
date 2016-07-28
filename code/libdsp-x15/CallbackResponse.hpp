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

#ifndef CALLBACKRESPONSE_HPP_
#define CALLBACKRESPONSE_HPP_

#include <cstddef>

class CallbackResponse {

public:
    enum Ops {
		FFT,
		IFFT,
		FIR,
		IIR,
		CONV,
		UNDEFINED
	};
	CallbackResponse();
	CallbackResponse(Ops op, unsigned int dataSize, float *dataPtr);
	~CallbackResponse();
	CallbackResponse::Ops getOp() const;
	void setOp(Ops op);
	size_t getDataSize() const;
	void setDataSize(size_t size);
	size_t getN() const;
	float* getDataPtr() const;
	void setDataPtr(float *data);

private:
    Ops _op;
	unsigned int _dataSize;
	float *_dataPtr;
};

#endif //CALLBACK_RESPONSE_HPP_
