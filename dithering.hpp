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

class Threshold : public Dithering {
    private: uint8_t pixel_value;
    private: void dither(AsciiFx *ascii_img) override;
};

class Random : public Dithering {
    private: uint8_t pixel_value;
    private: void dither(AsciiFx *ascii_img) override;
};

class BayerMatrix : public Dithering {
    private: void dither(AsciiFx *ascii_img) override;
};

class FloydSteinberg : public Dithering {
    private: uint8_t pixel_value;

    private: uint8_t old_value;
    private: uint8_t new_value;
    private: int error = 0;

    private: void dither(AsciiFx *ascii_img) override;
};