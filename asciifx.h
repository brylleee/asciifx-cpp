#include <GraphicsMagick/Magick++/Geometry.h>
#include <GraphicsMagick/Magick++/Include.h>
#include <Magick++.h>
#include <iostream>

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
        setlocale(LC_ALL, "en_US.UTF-8");
        Magick::InitializeMagick(nullptr);
    }
}

class AsciiFx;

// Strategy design pattern
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

    public: AsciiFx(std::string img_path) {
        try {
            img.read(img_path);
        } catch(Magick::Exception &e) {
            std::cerr << e.what() << std::endl;
        }

        this->height = img.rows();
        this->width = img.columns();

        // Initial allocation of space
        this->allocate_space(height);
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
        allocate_space(this->height);

        DitheringAlgorithm->dither(this);

        std::vector<std::string> result(height);

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
            for(int j = 0, k = 0; j < this->space.at(i).size(); j += 4, k++) {
                std::array<int, 4> block = { this->space[i][j], this->space[i][j + 1], this->space[i][j + 2], this->space[i][j + 3] };
                auto itr = arts.find(block);
                if(itr != arts.end()) {
                    result[i] += itr->second;
                }
            }
        }

        this->free_space();
        return result;
    }


    public: std::vector<std::string> braille_convert(Dithering* DitheringAlgorithm) {
        std::vector<std::string> result(this->height);
        std::string line = "";

        this->allocate_space(this->height);
        DitheringAlgorithm->dither(this);

        // https://kbravh.dev/encoding-qr-codes-in-braille
        int braille_unicode_offset = 10240;
        int corresponding_char_offset = 0;

        for(int i = 0; i < this->height; i += 4) {
            for(int j = 0; j < this->space.at(i).size(); j += 2) {
                corresponding_char_offset +=
                    (space[i][j] << 1)   + (space[i][j+1] << 4) +
                    (space[i+1][j] << 2) + (space[i+1][j+1] << 5) +
                    (space[i+2][j] << 3) + (space[i+2][j+1] << 6) +
                    (space[i+3][j] << 7) + (space[i+3][j+1] << 8);

                // add braille offset + char offset to line
                // line += ;
            }
        }

        return result;
    }


    // Allocate space with custom height
    public: void allocate_space(size_t new_height = 0) {
        this->free_space();
        new_height = new_height == 0 ? this->height : new_height;

        // Fill space with default values
        for(int i = 0; i < new_height; i++) {  
            this->space.push_back(std::vector<int>());
        }
    }

    // Reset the contents of space
    public: void free_space() {
        this->space.clear();
    }
};


class Threshold : public Dithering {
    private: Magick::Color pixel;
    private: int grayscale_value;
    
    private: void dither(AsciiFx *ascii_img) override {
        ascii_img->img.depth(8);
        for(int i = 0; i < ascii_img->get_height(); i++) {
            for(int j = 0; j < ascii_img->get_width(); j++) {
                pixel = ascii_img->img.pixelColor(j, i); 
                grayscale_value = ((pixel.redQuantum() + pixel.greenQuantum() + pixel.blueQuantum()) / 3); 

                ascii_img->space.at(i).push_back(
                    grayscale_value > 65536/2 ? 1 : 0
                );
            }
        }
    }
};

// ** unfinished **
class FloydSteinberg : public Dithering {
    private: int error = 0;
    private: Magick::Color pixel;
    private: int grayscale_value;

    private: int black = 255;

    private: void dither(AsciiFx *ascii_img) override {
        ascii_img->img.depth(8);  // turn image colors to 8 bits
        
        for(int i = 0; i < ascii_img->get_height(); i++) {
            for(int j = 0; j < ascii_img->get_width(); j++) {
                pixel = ascii_img->get_img().pixelColor(j, i);
                grayscale_value = ((pixel.redQuantum() + pixel.greenQuantum() + pixel.blueQuantum()) / 3);
                // Error diffusion
                if(grayscale_value >= (int)black/2) {  // halfway or above: force black
                    ascii_img->space.at(i).push_back(0); 
                    error += grayscale_value;
                } else { // halfway or below: force white
                    ascii_img->space.at(i).push_back(0);
                    error = error + (grayscale_value - black);
                }                
            }
        }
    }
};
