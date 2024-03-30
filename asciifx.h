#include <GraphicsMagick/Magick++/Geometry.h>
#include <GraphicsMagick/Magick++/Include.h>
#include <Magick++.h>

#include <iostream>
#include <cmath>

// TODO: do something about temporary img resizing at convert functions (DRY)
//       unaccounted values under moon emojis convert
//       random dithering
//       move clamp and remap to a different class
//       better documentation

/**
 * @namespace AsciiFxConfig 
 * @brief Configuration functions to use within AsciiFxConfig 
 * 
 * This namespace contains functions that work alongside with 'AsciiFxConfig' to properly
 * work with images. This namespace also improves readability for codes that use this header
*/
namespace AsciiFxConfig {
    /**
     * @brief Initializes ImageMagick as well as other stuff. This must be initialized before using any part of the AsciiFx library.
    */
    inline void init() {
        setlocale(LC_ALL, "");  
        Magick::InitializeMagick(nullptr);
    }
}

class AsciiFx;

/**
 * @class Dithering
 * @brief Parent class of various dithering algorithms
 * Fills up the 'space' vector in an 'AsciiFx' object with dithered pixel values
*/
class Dithering {
    private: virtual void dither(AsciiFx *img) = 0;
    friend class AsciiFx;
};

/**
 * @class AsciiFx
 * @brief Contains the algorithms for converting dithered images to different ascii arts 
*/
class AsciiFx {
    // Refers to the image supplied in the constructor
    public: Magick::Image img;
    public: Magick::Image get_img() { return img; };  // getter returns reference

    // Refers to the 2D vector containing integers of dithered values
    public: std::vector<std::vector<int>> space;
    public: std::vector<std::vector<int>> get_space() { return space; };  // getter returns value

    // Refers to the original width and height of the image supplied in the constructor
    private: size_t width;
    public: size_t get_width() { return width; };  // getter returns value

    private: size_t height;
    public: size_t get_height() { return height; };  // getter returns value

    // Remaps a value from a bigger range of values to a smaller range
    public: static int remap(int x, int xmin, int xmax, int ymin, int ymax) {
        return round(((float)(x - xmin)/(float)(xmax - xmin)) * (ymax - ymin) + ymin);
    }

    // Constraint a value with minimum and maximum range
    public: static int clamp(int x, int xmin, int xmax) {
        if(x > xmax) return xmax;
        if(x < xmin) return xmin;
        return x;
    } 

    public: AsciiFx(std::string img_path) {
        try {
            img.read(img_path);
        } catch(Magick::Exception &e) {
            std::cerr << e.what() << std::endl;
        }

        this->height = img.rows();
        this->width = img.columns();

        allocate_space(this->height);
    }


    public: std::vector<std::string> moon_emojis_convert(Dithering* DitheringAlgorithm, int shrink_nth_times = 1) {
        Magick::Image img_cpy = img;  // This will hold the original image while it is being resized

        // Converting to moon emojis means that: output width will be /4 while output height is untouched
        // Shrinking the height compensates for it and keeps the original aspect ratio.
        int shrink_height_compensation = 4;
        Magick::Geometry size((this->width/shrink_nth_times), (this->height/shrink_nth_times)/shrink_height_compensation);
        size.aspect(true);
        size.fillArea(true);
        
        // Resize original image
        img.resize(size);  
        this->width = this->width/shrink_nth_times;
        this->height = (this->height/shrink_nth_times)/shrink_height_compensation;

        // Allocate custom height of space
        allocate_space(this->height, this->width);
        DitheringAlgorithm->dither(this);
        std::vector<std::string> result(this->height);

        // After dithering the image, we turn it back to it's original size
        img = img_cpy;
        this->width = img.columns();
        this->height = img.rows();

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
            { {1, 1, 0, 1}, "🌖" },
            { {1, 1, 1, 1}, "🌕" },
            { {1, 1, 1, 0}, "🌖" },
            { {1, 1, 0, 0}, "🌗" },
            { {1, 0, 1, 0}, "🌗" },
            { {1, 0, 0, 0}, "🌘" },
            { {0, 1, 0, 0}, "🌘" },
        };

        for(int i = 0; i < this->space.size(); i++) {
            for(int j = 0; j < this->space.at(i).size(); j += 4) {
                std::array<int, 4> block = {
                    remap(this->space[i][j], 0, 255, 0, 1),
                    remap(this->space[i][j + 1], 0, 255, 0, 1), 
                    remap(this->space[i][j + 2], 0, 255, 0, 1), 
                    remap(this->space[i][j + 3], 0, 255, 0, 1)
                };
                
                auto itr = arts.find(block);
                if(itr != arts.end())
                    result[i] += itr->second;
            }
        }

        this->free_space();
        return result;
    }


    public: std::vector<std::string> block_convert(Dithering* DitheringAlgorithm, int shrink_nth_times = 1) {
        Magick::Image img_cpy = img;  // This will hold the original image while it is being resized

        // Shrinking the height compensates for it and keeps the original aspect ratio.
        Magick::Geometry size((this->width/shrink_nth_times), (this->height/shrink_nth_times)/2);
        size.aspect(true);
        size.fillArea(true);

        // Resize original image
        img.resize(size);  
        this->width = this->width/shrink_nth_times;
        this->height = this->height/shrink_nth_times/2;

        this->allocate_space(this->height, this->width);
        DitheringAlgorithm->dither(this);
        std::vector<std::string> result(this->height);

        // After dithering the image, we turn it back to it's original size
        img = img_cpy;
        this->width = img.columns();
        this->height = img.rows();

        std::string blocks[] = {"░", "▒", "▓", "█"};

        for(int i = 0; i < this->space.size(); i++) {
            for(int j = 0; j < this->space.at(i).size(); j++) {
                result[i] += blocks[remap(space[i][j], 0, 255, 0, 3)];
            }
        }

        return result;
    }


    public: std::vector<std::wstring> braille_convert_wide(Dithering* DitheringAlgorithm, int shrink_nth_times = 1) {
        Magick::Image img_cpy = img;  // This will hold the original image while it is being resized

        // Shrinking the height compensates for it and keeps the original aspect ratio.
        Magick::Geometry size((this->width/shrink_nth_times), (this->height/shrink_nth_times));
        size.aspect(true);
        size.fillArea(true);

        // Resize original image
        img.resize(size);  
        this->width = this->width/shrink_nth_times;
        this->height = this->height/shrink_nth_times;

        this->allocate_space(this->height, this->width);
        DitheringAlgorithm->dither(this);
        std::vector<std::wstring> result(floor(this->height/4));  // the result only occupies a quarter of the height
        std::wstring line;

        // After dithering the image, we turn it back to it's original size
        img = img_cpy;
        this->width = img.columns();
        this->height = img.rows();

        // https://kbravh.dev/encoding-qr-codes-in-braille
        int braille_unicode_offset = 0x2800;
        int corresponding_char_offset = 0;

        for(int i = 0, ix = 0; i+4 < this->space.size(); i += 4, ix++) {
            for(int j = 0; j+2 < this->space.at(i).size(); j += 2) {
                corresponding_char_offset +=
                    remap(space[i][j], 0, 255, 0, 1)   + (remap(space[i][j+1], 0, 255, 0, 1) << 3) +
                    (remap(space[i+1][j], 0, 255, 0, 1) << 1) + (remap(space[i+1][j+1], 0, 255, 0, 1) << 4) +
                    (remap(space[i+2][j], 0, 255, 0, 1) << 2) + (remap(space[i+2][j+1], 0, 255, 0, 1) << 5) +
                    (remap(space[i+3][j], 0, 255, 0, 1) << 6) + (remap(space[i+3][j+1], 0, 255, 0, 1) << 7);

                // add braille offset + char offset to line
                line += static_cast<wchar_t>(braille_unicode_offset + corresponding_char_offset);
                corresponding_char_offset = 0;
            }

           result[ix] = line;
           line.clear();
        }

        return result;
    }


    // Allocate space with custom height
    public: void allocate_space(size_t new_height = 0, size_t new_width = 0) {
        this->free_space();
        new_height = new_height == 0 ? this->height : new_height;

        // Fill space with default values
        for(int i = 0; i < new_height; i++) {  
            this->space.push_back(std::vector<int>(new_width));
        }
    }

    // Reset the contents of space
    public: void free_space() {
        this->space.clear();
    }
};

class Threshold : public Dithering {
    private: Magick::Color pixel;
    private: int pixel_value;
    
    private: void dither(AsciiFx *ascii_img) override {
        for(int i = 0; i < ascii_img->space.size(); i++) {
            for(int j = 0; j < ascii_img->space.at(0).size(); j++) {
                pixel = ascii_img->img.pixelColor(j, i); 
                pixel_value = round(0.299*pixel.redQuantum()) + round(0.587*pixel.greenQuantum()) + round(0.114*pixel.blueQuantum()); 

                ascii_img->space[i][j] = AsciiFx::remap(pixel_value, 0, 65535, 0, 255); //grayscale_value > 65536/2 ? 1 : 0;
            }
        }
    }
};

class BayerMatrix : public Dithering {
    private: Magick::Color pixel;

    private: void dither(AsciiFx *ascii_img) override {
        
    }
};

class FloydSteinberg : public Dithering {
    private: Magick::Color pixel;
    private: int pixel_value = 0;

    private: const int white = 255;
    private: const int black = 0;

    private: int old_value;
    private: int new_value;
    private: int error = 0;

    private: void dither(AsciiFx *ascii_img) override {
        for(int i = 0; i < ascii_img->get_height(); i++) {
            for(int j = 0; j < ascii_img->get_width(); j++) {
                pixel = ascii_img->get_img().pixelColor(j, i);
                pixel_value = round(0.299*pixel.redQuantum()) + round(0.587*pixel.greenQuantum()) + round(0.114*pixel.blueQuantum()); 
                ascii_img->space[i][j] = AsciiFx::remap(pixel_value, 0, 65535, 0, 255);
            }
        }
        
        for(int i = 0; i < ascii_img->space.size(); i++) {
            for(int j = 0; j < ascii_img->space.at(0).size(); j++) {
                old_value = ascii_img->space[i][j];
                new_value = old_value >= (white/2) ? white : black;
                error = old_value - new_value;

                if(j < ascii_img->get_width()-1)
                    ascii_img->space[i][j+1] += round(error / 16.0 * 7.0);

                if(i < ascii_img->get_height()-1 && j > 0)
                    ascii_img->space[i+1][j-1] += round(error / 16.0 * 3.0);

                if(i < ascii_img->get_height()-1)
                    ascii_img->space[i+1][j] += round(error / 16.0 * 5.0);

                if(i < ascii_img->get_height()-1 && j < ascii_img->get_width()-1)
                    ascii_img->space[i+1][j+1] += round(error / 16.0 * 1.0);

                ascii_img->space[i][j] = AsciiFx::clamp(ascii_img->space[i][j], 0, 255);
            }
        }
    }
};