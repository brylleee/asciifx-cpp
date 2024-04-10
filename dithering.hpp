#pragma once

#include "calcfx.hpp"
#include "asciifx.hpp"

#include <cstdlib>
#include <ctime>

class AsciiFx;

const int BLACK = 0;
const int WHITE = 255;

/**
 * @class Dithering
 * @brief Parent class of various dithering algorithms
 * Fills up the 'space' vector in an 'AsciiFx' object with dithered pixel values
*/
class Dithering {
    private: virtual void dither(AsciiFx *img) = 0;
    friend class AsciiFx;
};

class Default : public Dithering {
    private: uint8_t pixel_value;
    private: void dither(AsciiFx *ascii_img) override;
};

class Random : public Dithering {
    private: uint8_t pixel_value;
    private: void dither(AsciiFx *ascii_img) override;
};

class BayerMatrix : public Dithering {
    private: uint8_t pixel_value;
    private: void dither(AsciiFx *ascii_img) override;

    private: int bayer_size = 8;
    private: int bayer8x8matrix[8][8] = {
        { 0, 32,  8, 40,  2, 34, 10, 42  },
        { 48, 16, 56, 24, 50, 18, 58, 26 },
        { 12, 44,  4, 36, 14, 46,  6, 38 },
        { 60, 28, 52, 20, 62, 30, 54, 22 },
        {  3, 35, 11, 43,  1, 33,  9, 41 },
        { 51, 19, 59, 27, 49, 17, 57, 25 },
        { 15, 47,  7, 39, 13, 45,  5, 37 },
        { 63, 31, 55, 23, 61, 29, 53, 21 }
    };
};

class FloydSteinberg : public Dithering {
    private: uint8_t pixel_value;

    private: uint8_t old_value;
    private: uint8_t new_value;
    private: int error = 0;

    private: void dither(AsciiFx *ascii_img) override;
};