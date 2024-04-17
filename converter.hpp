#pragma once

class AsciiFx;
class Dithering;

#include "asciifx.hpp"

#include "dithering.hpp"
#include "calcfx.hpp"

#include <string>
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