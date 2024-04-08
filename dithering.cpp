#include "dithering.hpp"
#include "calcfx.hpp"

void Dithering::dither(AsciiFx *ascii_img) { }

void Threshold::dither(AsciiFx *ascii_img) {
    for(int i = 0; i < ascii_img->space.size(); i++) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) { 
            pixel_value = round((0.299*(int)(ascii_img->img(j, i, 0, 0))) + (0.587*(int)(ascii_img->img(j, i, 0, 1))) + (0.114*(int)(ascii_img->img(j, i, 0, 2)))); 
            ascii_img->space[i][j] = pixel_value; //grayscale_value > 65536/2 ? 1 : 0;
        }
    }
}

void BayerMatrix::dither(AsciiFx *ascii_img) {
        
}

void Random::dither(AsciiFx *ascii_img) {

}

void FloydSteinberg::dither(AsciiFx *ascii_img) {
    for(int i = 0; i < ascii_img->get_height(); i++) {
        for(int j = 0; j < ascii_img->get_width(); j++) {
            pixel_value = round(0.299*(int)(ascii_img->img(j, i, 0, 0))) + round(0.587*(int)(ascii_img->img(j, i, 0, 1))) + round(0.114*(int)(ascii_img->img(j, i, 0, 2))); 
            ascii_img->space[i][j] = pixel_value;
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