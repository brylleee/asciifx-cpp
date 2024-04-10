#include "dithering.hpp"

void Dithering::dither(AsciiFx *ascii_img) { }

// This doesn't dither, just convert each pixel to its grayscale value (0-255)
// Thresholding is done at convert functions because they each have their own
// color values they're limited at.
void Default::dither(AsciiFx *ascii_img) {
    for(int i = 0; i < ascii_img->space.size(); i++) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) { 
            pixel_value = round((0.299*(int)(ascii_img->img(j, i, 0, 0))) + (0.587*(int)(ascii_img->img(j, i, 0, 1))) + (0.114*(int)(ascii_img->img(j, i, 0, 2)))); 
            ascii_img->space[i][j] = pixel_value; 
        }
    }
}

void BayerMatrix::dither(AsciiFx *ascii_img) {
    for(int i = 0; i < ascii_img->space.size(); i++) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) { 
            pixel_value = round((0.299*(int)(ascii_img->img(j, i, 0, 0))) + (0.587*(int)(ascii_img->img(j, i, 0, 1))) + (0.114*(int)(ascii_img->img(j, i, 0, 2))));
            // Compare the value the current pixel to the bayer matrix value
            ascii_img->space[i][j] = pixel_value > CalcFx::remap(bayer8x8matrix[i % (bayer_size+1)][j % (bayer_size+1)], 0, 64, 0, 255) ? WHITE : BLACK; 
        }
    }
}

void Random::dither(AsciiFx *ascii_img) {
    srand(time(0));
    
    for(int i = 0; i < ascii_img->space.size(); i++) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) { 
            pixel_value = round((0.299*(int)(ascii_img->img(j, i, 0, 0))) + (0.587*(int)(ascii_img->img(j, i, 0, 1))) + (0.114*(int)(ascii_img->img(j, i, 0, 2)))); 
            // Apply a random threshold value
            ascii_img->space[i][j] = pixel_value > rand()%255 ? 255 : 0;
        }
    }
}

void FloydSteinberg::dither(AsciiFx *ascii_img) {
    for(int i = 0, ix = 0; i < ascii_img->space.size(); i++, ix=i-2) {
        for(int j = 0; j < ascii_img->space.at(0).size(); j++) {
            pixel_value = round(0.299*(int)(ascii_img->img(j, i, 0, 0))) + round(0.587*(int)(ascii_img->img(j, i, 0, 1))) + round(0.114*(int)(ascii_img->img(j, i, 0, 2))); 
            ascii_img->space[i][j] = pixel_value;

            if(i < 2) { continue; }

            old_value = ascii_img->space[ix][j];
            new_value = old_value >= (WHITE/2) ? WHITE : BLACK;
            error = old_value - new_value;

            if(j < ascii_img->get_width()-1)
                ascii_img->space[ix][j+1] += round(error / 16.0 * 7.0);

            if(ix < ascii_img->get_height()-1 && j > 0)
                ascii_img->space[ix+1][j-1] += round(error / 16.0 * 3.0);

            if(ix < ascii_img->get_height()-1)
                ascii_img->space[ix+1][j] += round(error / 16.0 * 5.0);

            if(ix < ascii_img->get_height()-1 && j < ascii_img->get_width()-1)
                ascii_img->space[ix+1][j+1] += round(error / 16.0 * 1.0);

            ascii_img->space[ix][j] = CalcFx::clamp(ascii_img->space[ix][j], 0, 255);
        }
    }
}