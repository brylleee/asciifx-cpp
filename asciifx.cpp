#include "asciifx.hpp"

void AsciiFxConfig::init() {
    setlocale(LC_ALL, "");  
}

// -----------------------------------------------------------------------------------------------------

AsciiFx::AsciiFx(std::string img_path) {
    this->img.load(img_path.c_str());

    this->height = img.height();
    this->width = img.width();

    allocate_space(this->height);
}

std::vector<std::string> AsciiFx::convert(Converter* ConverterMethod, Dithering* DitheringAlgorithm, int shrink_nth_times) {
    return ConverterMethod->convert(this, DitheringAlgorithm, shrink_nth_times);
}

CImg<uint8_t> AsciiFx::get_img() { return img; }

std::vector<std::vector<uint8_t>> AsciiFx::get_space() { return space; }

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