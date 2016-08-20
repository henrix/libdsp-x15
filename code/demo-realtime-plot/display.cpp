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

#include "display.hpp"
#include <iostream>

Display::Display(size_t width, size_t height) : _width(width), _height(height) {

    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    atexit(SDL_Quit);

    _screen = SDL_SetVideoMode(width, height, 8, SDL_HWSURFACE);
    if ( _screen == NULL ) {
        fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",
                        SDL_GetError());
        exit(1);
    }

    std::string title("Amplitude Spectrum");
    SDL_WM_SetCaption(title.c_str(), NULL );
}

Display::~Display() {
    SDL_Quit();
}

void Display::drawPixels(size_t length, float *pixels){
    int width_tmp;
    (length > _width) ? width_tmp = length / _width : width_tmp = 1;

    SDL_FillRect(_screen, NULL, 0x000000);

    if ( SDL_MUSTLOCK(_screen) ){
        if ( SDL_LockSurface(_screen) < 0 ) {
            fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
            return;
        }
    }

    /* offset pixels ([-1 1] -> [0 height]) */
    for (int i=0; i < _width; i++){
        _putPixel(i, (((double)_height/2.0) * pixels[i*width_tmp]) + _height/2);
        //std::cout << "y: " << _height * points[i*width_tmp] << std::endl;
    }

    if ( SDL_MUSTLOCK(_screen) ) 
        SDL_UnlockSurface(_screen);
    
    //SDL_UpdateRect(_screen, _width/2, _height/2, 1, 1); //Update just on pixel
    SDL_UpdateRect(_screen, 0, 0, 0, 0); //Update entire screen
}

void Display::drawLines(size_t length, float *lineends){
    int width_tmp;
    (length > _width) ? width_tmp = length / _width : width_tmp = 1;

    SDL_FillRect(_screen, NULL, 0x000000);

    if ( SDL_MUSTLOCK(_screen) ){
        if ( SDL_LockSurface(_screen) < 0 ) {
            fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
            return;
        }
    }

    /* offset pixels ([-1 1] -> [0 height]) */
    for (int i=0; i < _width; i++){
        float max = (((double)_height/2.0) * lineends[i*width_tmp]) + _height/2;
        for (float j=0; j < max; j+=1.0){
            _putPixel(i, j);
        }
    }

    if ( SDL_MUSTLOCK(_screen) ) 
        SDL_UnlockSurface(_screen);
    
    SDL_UpdateRect(_screen, 0, 0, 0, 0); //Update entire screen
}

void Display::_putPixel(size_t x, size_t y){
    int bpp = _screen->format->BytesPerPixel;
    y = _height - y;
    Uint8 *p = (Uint8 *)_screen->pixels + y * _screen->pitch + x * bpp;
    *p = 0xFF;
}
