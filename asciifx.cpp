#include <iostream>
#include <map>

#include "CImg.h"
using namespace cimg_library;

#include "asciifx.hpp"
#include "dithering.hpp"
#include "calcfx.hpp"

// TODO: do something about temporary img resizing at convert functions (DRY)
//       better documentation


void AsciiFxConfig::init() {
    setlocale(LC_ALL, "");  
}

// -----------------------------------------------------------------------------------------------------

AsciiFx::AsciiFx(std::string img_path) {
    try {
        this->img.load(img_path.c_str());
    } catch(CImgIOException &e) {
        std::cerr << e.what() << std::endl;
    }

    this->height = img.height();
    this->width = img.width();

    allocate_space(this->height);
}

CImg<uint8_t> AsciiFx::get_img() { return img; }

std::vector<std::vector<uint8_t>> AsciiFx::get_space() { return space; }

size_t AsciiFx::get_width() { return width; }

size_t AsciiFx::get_height() { return height; }

// Allocate space with custom height
void AsciiFx::allocate_space(size_t new_height, size_t new_width) {
    this->free_space();
    new_height = new_height == 0 ? this->height : new_height;

    // Fill space with default values
    for(int i = 0; i < new_height; i++) {  
        this->space.push_back(std::vector<uint8_t>(new_width));
    }
}

// Reset the contents of space
void AsciiFx::free_space() {
    this->space.clear();
}

// -----------------------------------------------------------------------------------------------------

std::vector<std::string> AsciiFx::moon_emojis_convert(Dithering* DitheringAlgorithm, int shrink_nth_times) {
    CImg<uint8_t> img_cpy = img;  // This will hold the original image while it is being resized

    // Converting to moon emojis means that: output width will be /4 while output height is untouched
    // Shrinking the height compensates for it and keeps the original aspect ratio.
    int shrink_height_compensation = 4;
    img.resize((this->width/shrink_nth_times), (this->height/shrink_nth_times)/shrink_height_compensation);

    // Redefine width and height propereties
    this->width = this->width/shrink_nth_times;
    this->height = (this->height/shrink_nth_times)/shrink_height_compensation;

    // Allocate custom height of space
    allocate_space(this->height, this->width);
    DitheringAlgorithm->dither(this);
    std::vector<std::string> result(this->height);

    // After dithering the image, we turn it back to it's original size
    img = img_cpy;
    this->width = img.width();
    this->height = img.height();

    std::map<std::array<int, 4>, std::string> arts = {
        { {0, 0, 0, 0}, "🌑" },
        { {1, 0, 0, 1}, "🌑" },
        { {0, 0, 0, 1}, "🌒" },
        { {0, 0, 1, 0}, "🌒" },
        { {0, 0, 1, 1}, "🌓" },
        { {0, 1, 0, 1}, "🌓" },
        { {0, 1, 1, 1}, "🌔" },
        { {1, 0, 1, 1}, "🌔" },
        { {0, 1, 1, 0}, "🌕" },
        { {1, 1, 1, 1}, "🌕" },
        { {1, 1, 0, 1}, "🌖" },
        { {1, 1, 1, 0}, "🌖" },
        { {1, 1, 0, 0}, "🌗" },
        { {1, 0, 1, 0}, "🌗" },
        { {1, 0, 0, 0}, "🌘" },
        { {0, 1, 0, 0}, "🌘" },
    };

    for(int i = 0; i < this->space.size(); i++) {
        for(int j = 0; j < this->space.at(i).size()-4; j += 4) {
            std::array<int, 4> block = {
                CalcFx::remap(this->space[i][j], 0, 255, 0, 1),
                CalcFx::remap(this->space[i][j + 1], 0, 255, 0, 1), 
                CalcFx::remap(this->space[i][j + 2], 0, 255, 0, 1), 
                CalcFx::remap(this->space[i][j + 3], 0, 255, 0, 1)
            };
            
            auto itr = arts.find(block);
            if(itr != arts.end())
                result[i] += itr->second;
        }
    }

    this->free_space();
    return result;
}


std::vector<std::string> AsciiFx::block_convert(Dithering* DitheringAlgorithm, int shrink_nth_times) {
    CImg<uint8_t> img_cpy = img;  // This will hold the original image while it is being resized
    
    img.resize((this->width/shrink_nth_times), (this->height/shrink_nth_times)/2);

    // Resize original image
    this->width = this->width/shrink_nth_times;
    this->height = this->height/shrink_nth_times/2;

    this->allocate_space(this->height, this->width);
    DitheringAlgorithm->dither(this);
    std::vector<std::string> result(this->height);

    // After dithering the image, we turn it back to it's original size
    img = img_cpy;
    this->width = img.width();
    this->height = img.height();

    std::string blocks[] = {"░", "▒", "▓", "█"};

    for(int i = 0; i < this->space.size(); i++) {
        for(int j = 0; j < this->space.at(i).size(); j++) {
            result[i] += blocks[CalcFx::remap(space[i][j], 0, 255, 0, 3)];
        }
    }

    return result;
}


std::vector<std::wstring> AsciiFx::braille_convert_wide(Dithering* DitheringAlgorithm, int shrink_nth_times) {
    CImg<uint8_t> img_cpy = img;  // This will hold the original image while it is being resized

    // Shrinking the height compensates for it and keeps the original aspect ratio.
    img.resize((this->width/shrink_nth_times), (this->height/shrink_nth_times));

    // Redefine width and height properties
    this->width = this->width/shrink_nth_times;
    this->height = this->height/shrink_nth_times;

    this->allocate_space(this->height, this->width);
    DitheringAlgorithm->dither(this);
    std::vector<std::wstring> result(floor(this->height/4));  // the result only occupies a quarter of the height
    std::wstring line;

    // After dithering the image, we turn it back to it's original size
    img = img_cpy;
    this->width = img.width();
    this->height = img.height();

    // https://kbravh.dev/encoding-qr-codes-in-braille
    int braille_unicode_offset = 0x2800;
    int corresponding_char_offset = 0;

    for(int i = 0, ix = 0; i+4 < this->space.size(); i += 4, ix++) {
        for(int j = 0; j+2 < this->space.at(i).size(); j += 2) {
            corresponding_char_offset +=
                CalcFx::remap(space[i][j], 0, 255, 0, 1)   + (CalcFx::remap(space[i][j+1], 0, 255, 0, 1) << 3) +
                (CalcFx::remap(space[i+1][j], 0, 255, 0, 1) << 1) + (CalcFx::remap(space[i+1][j+1], 0, 255, 0, 1) << 4) +
                (CalcFx::remap(space[i+2][j], 0, 255, 0, 1) << 2) + (CalcFx::remap(space[i+2][j+1], 0, 255, 0, 1) << 5) +
                (CalcFx::remap(space[i+3][j], 0, 255, 0, 1) << 6) + (CalcFx::remap(space[i+3][j+1], 0, 255, 0, 1) << 7);

            // add braille offset + char offset to line
            line += static_cast<wchar_t>(braille_unicode_offset + corresponding_char_offset);
            corresponding_char_offset = 0;
        }

        result[ix] = line;
        line.clear();
    }

    return result;
}


