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

#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#define DIM 500

#include <SDL/SDL.h>

class Display{
public:
    Display(size_t width, size_t height);
    ~Display();
    /** 
     * draw pixel vector 
     *	length should be equal or multiple of width
     *	pixel range should be between -1.0 and 1.0
     */
    void drawPixels(size_t length, float *pixels);
    void drawLines(size_t length, float *lineends);
private:
	void _putPixel(size_t x, size_t y);

	size_t _width, _height;
    SDL_Surface *_screen;
};

#endif //DISPLAY_HPP_
