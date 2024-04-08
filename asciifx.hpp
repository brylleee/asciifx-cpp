#pragma once

#include <iostream>
#include <cmath>

#include "CImg.h"
using namespace cimg_library;

#include "dithering.hpp"
class Dithering;

/*
 * @namespace AsciiFxConfig 
 * @brief Configuration functions to use within AsciiFxConfig 
 * 
 * This namespace contains functions that work alongside with 'AsciiFxConfig' to properly
 * work with images. This namespace also improves readability for codes that use this header
 */
class AsciiFxConfig {
    /*
     * @class AsciiFx
     * @brief Initializes ImageMagick as well as other stuff. This must be initialized before using any part of the AsciiFx library.
     */
    public: static void init();
};

class AsciiFx {
    // Refers to the image supplied in the constructor
    public: CImg<uint8_t> img;
    public: CImg<uint8_t> get_img();

    // Refers to the 2D vector containing integers of dithered values
    public: std::vector<std::vector<uint8_t>> space;
    public: std::vector<std::vector<uint8_t>> get_space();  // getter returns value

    // Refers to the original width and height of the image supplied in the constructor
    private: size_t width;
    public: size_t get_width();  // getter returns value

    private: size_t height;
    public: size_t get_height();  // getter returns value

    public: AsciiFx(std::string img_path);
    public: std::vector<std::string> moon_emojis_convert(Dithering* DitheringAlgorithm, int shrink_nth_times = 1);
    public: std::vector<std::string> block_convert(Dithering* DitheringAlgorithm, int shrink_nth_times = 1);
    public: std::vector<std::wstring> braille_convert_wide(Dithering* DitheringAlgorithm, int shrink_nth_times = 1);

    public: void allocate_space(size_t new_height = 0, size_t new_width = 0);
    public: void free_space();
};