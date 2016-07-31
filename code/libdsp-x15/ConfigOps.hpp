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

#ifndef CONFIGOPS_HPP_
#define CONFIGOPS_HPP_

#include <iostream>
#include <map>
#include <boost/any.hpp>

class ConfigOps {
public:
    enum Ops {
		FFT,
		IFFT,
		FILTER_BIQUAD,
		FILTER_FIRCIRC,
		FILTER_FIR_CPLX,
		FILTER_FIR_GEN,
		FILTER_FIR_R2,
        FILTER_IIR,
        FILTER_IIRLAT
	};

	ConfigOps(Ops op) ;
	Ops getOp() const ;
    template<typename T> T getParam(std::string param_name) const {
        try{
            T val = boost::any_cast<T>(_params.at(param_name));
            return val;
        }
        catch(const std::exception &ec){
            std::cout << "error: " << ec.what() << std::endl;
        }
    }
    template<typename T> void setParam(std::string param_name, T val){
        _params[param_name] = val;
    }


private:
    Ops _op;
    std::map<std::string, boost::any> _params;
};

#endif //CONFIGOPS_HPP_
