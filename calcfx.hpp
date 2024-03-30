#pragma once

#include <cmath>

class CalcFx {
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
};