#include "converter.hpp"

std::vector<std::string> MoonEmojis::convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times) {
    CImg<uint8_t> img_cpy = asciifx->img;  // This will hold the original image while it is being resized

    // Converting to moon emojis means that: output width will be /4 while output height is untouched
    // Shrinking the height compensates for it and keeps the original aspect ratio.
    int shrink_height_compensation = 4;
    asciifx->img.resize((asciifx->width/shrink_nth_times), (asciifx->height/shrink_nth_times)/shrink_height_compensation);

    // Redefine width and height propereties
    asciifx->width = asciifx->width/shrink_nth_times;
    asciifx->height = (asciifx->height/shrink_nth_times)/shrink_height_compensation;

    // Allocate custom height of space
    asciifx->allocate_space(asciifx->height, asciifx->width);
    DitheringAlgorithm->dither(asciifx);
    std::vector<std::string> result(asciifx->height);

    // After dithering the image, we turn it back to it's original size
    asciifx->img = img_cpy;
    asciifx->width = asciifx->img.width();
    asciifx->height = asciifx->img.height();

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

    for(int i = 0; i < asciifx->space.size(); i++) {
        for(int j = 0; j < asciifx->space.at(i).size()-4; j += 4) {
            std::array<int, 4> block = {
                CalcFx::remap(asciifx->space[i][j], 0, 255, 0, 1),
                CalcFx::remap(asciifx->space[i][j + 1], 0, 255, 0, 1), 
                CalcFx::remap(asciifx->space[i][j + 2], 0, 255, 0, 1), 
                CalcFx::remap(asciifx->space[i][j + 3], 0, 255, 0, 1)
            };
            
            auto itr = arts.find(block);
            if(itr != arts.end())
                result[i] += itr->second;
        }
    }

    asciifx->free_space();
    return result;
}

std::vector<std::string> Blocks::convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times) {
    CImg<uint8_t> img_cpy = asciifx->img;  // This will hold the original image while it is being resized
    
    asciifx->img.resize((asciifx->width/shrink_nth_times), (asciifx->height/shrink_nth_times)/2);

    // Resize original image
    asciifx->width = asciifx->width/shrink_nth_times;
    asciifx->height = asciifx->height/shrink_nth_times/2;

    asciifx->allocate_space(asciifx->height, asciifx->width);
    DitheringAlgorithm->dither(asciifx);
    std::vector<std::string> result(asciifx->height);

    // After dithering the image, we turn it back to it's original size
    asciifx->img = img_cpy;
    asciifx->width = asciifx->img.width();
    asciifx->height = asciifx->img.height();

    std::string blocks[] = {"░", "▒", "▓", "█"};

    for(int i = 0; i < asciifx->space.size(); i++) {
        for(int j = 0; j < asciifx->space.at(i).size(); j++) {
            result[i] += blocks[CalcFx::remap(asciifx->space[i][j], 0, 255, 0, 3)];
        }
    }

    return result;
}

std::vector<std::string> Braille::convert(AsciiFx* asciifx, Dithering* DitheringAlgorithm, int shrink_nth_times) {
    CImg<uint8_t> img_cpy = asciifx->img;  // This will hold the original image while it is being resized

    // Shrinking the height compensates for it and keeps the original aspect ratio.
    asciifx->img.resize((asciifx->width/shrink_nth_times), (asciifx->height/shrink_nth_times));

    // Redefine width and height properties
    asciifx->width = asciifx->width/shrink_nth_times;
    asciifx->height = asciifx->height/shrink_nth_times;

    asciifx->allocate_space(asciifx->height, asciifx->width);
    DitheringAlgorithm->dither(asciifx);
    std::vector<std::string> result(floor(asciifx->height/4));  // the result only occupies a quarter of the height
    std::wstring line;

    // After dithering the image, we turn it back to it's original size
    asciifx->img = img_cpy;
    asciifx->width = asciifx->img.width();
    asciifx->height = asciifx->img.height();

    // https://kbravh.dev/encoding-qr-codes-in-braille
    int braille_unicode_offset = 0x2800;
    int corresponding_char_offset = 0;

    for(int i = 0, ix = 0; i+4 < asciifx->space.size(); i += 4, ix++) {
        for(int j = 0; j+2 < asciifx->space.at(i).size(); j += 2) {
            corresponding_char_offset +=
                CalcFx::remap(asciifx->space[i][j], 0, 255, 0, 1)   + (CalcFx::remap(asciifx->space[i][j+1], 0, 255, 0, 1) << 3) +
                (CalcFx::remap(asciifx->space[i+1][j], 0, 255, 0, 1) << 1) + (CalcFx::remap(asciifx->space[i+1][j+1], 0, 255, 0, 1) << 4) +
                (CalcFx::remap(asciifx->space[i+2][j], 0, 255, 0, 1) << 2) + (CalcFx::remap(asciifx->space[i+2][j+1], 0, 255, 0, 1) << 5) +
                (CalcFx::remap(asciifx->space[i+3][j], 0, 255, 0, 1) << 6) + (CalcFx::remap(asciifx->space[i+3][j+1], 0, 255, 0, 1) << 7);

            // add braille offset + char offset to line
            line += static_cast<wchar_t>(braille_unicode_offset + corresponding_char_offset);
            corresponding_char_offset = 0;
        }

        // wstring to string; magic shi I found at: https://coliru.stacked-crooked.com/a/9a0db54094421729
        // https://cplusplus.com/forum/general/243349/
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        result[ix] = converter.to_bytes(line);
        line.clear();
    }

    return result;
}