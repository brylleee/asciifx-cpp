#pragma once

#include "asciifx.hpp"
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
    private: int pixel_value;
    private: void dither(AsciiFx *ascii_img) override;
};

class Random : public Dithering {
    private: void dither(AsciiFx *ascii_img) override;
};

class BayerMatrix : public Dithering {
    private: void dither(AsciiFx *ascii_img) override;
};

class FloydSteinberg : public Dithering {
    private: int pixel_value;

    private: int old_value;
    private: int new_value;
    private: int error = 0;

    private: void dither(AsciiFx *ascii_img) override;
};