#pragma once

#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

class AsciiFx;
class Dithering;

#include "asciifx.hpp"

#include <string>
#include "dithering.hpp"
#include "calcfx.hpp"

#include <locale>
#include <codecvt>

class Converter {
    public: virtual std::vector<std::string> convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times = 1) = 0;
};

class MoonEmojis : public Converter {  
    public: std::vector<std::string> convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times = 1) override;
};

class Blocks : public Converter {
    public: std::vector<std::string> convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times = 1) override;
};

class Braille : public Converter {
    public: std::vector<std::string> convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times = 1) override;
};