#include "dithering.hpp"
#include "calcfx.hpp"

void Dithering::dither(AsciiFx *ascii_img) { }

void Threshold::dither(AsciiFx *ascii_img) {
    for(int i = 0; i < ascii_img->space.size(); i++) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) {
            pixel = ascii_img->img.pixelColor(j, i); 
            pixel_value = round(0.299*pixel.redQuantum()) + round(0.587*pixel.greenQuantum()) + round(0.114*pixel.blueQuantum()); 

            ascii_img->space[i][j] = CalcFx::remap(pixel_value, 0, 65535, 0, 255); //grayscale_value > 65536/2 ? 1 : 0;
        }
    }
}

void BayerMatrix::dither(AsciiFx *ascii_img) {
        
}


void FloydSteinberg::dither(AsciiFx *ascii_img) {
    for(int i = 0; i < ascii_img->get_height(); i++) {
        for(int j = 0; j < ascii_img->get_width(); j++) {
            pixel = ascii_img->get_img().pixelColor(j, i);
            pixel_value = round(0.299*pixel.redQuantum()) + round(0.587*pixel.greenQuantum()) + round(0.114*pixel.blueQuantum()); 
            ascii_img->space[i][j] = CalcFx::remap(pixel_value, 0, 65535, 0, 255);
        }
    }
        
    for(int i = 0; i < ascii_img->space.size(); i++) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) {
            old_value = ascii_img->space[i][j];
            new_value = old_value >= (WHITE/2) ? WHITE : BLACK;
            error = old_value - new_value;

            if(j < ascii_img->get_width()-1)
                ascii_img->space[i][j+1] += round(error / 16.0 * 7.0);

            if(i < ascii_img->get_height()-1 && j > 0)
                ascii_img->space[i+1][j-1] += round(error / 16.0 * 3.0);

            if(i < ascii_img->get_height()-1)
                ascii_img->space[i+1][j] += round(error / 16.0 * 5.0);

            if(i < ascii_img->get_height()-1 && j < ascii_img->get_width()-1)
                ascii_img->space[i+1][j+1] += round(error / 16.0 * 1.0);

            ascii_img->space[i][j] = CalcFx::clamp(ascii_img->space[i][j], 0, 255);
        }
    }
}