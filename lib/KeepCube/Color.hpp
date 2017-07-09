#include <Arduino.h>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    void fromCompound(unsigned long compound) {
        r = compound >> 16;
        g = compound >> 8 & 0xFF;
        b = compound & 0xFF;
    }

    void fromCompound(String compound) {
        unsigned long ul = strtoul(compound.c_str(), NULL, 16);
        r = ul >> 16;
        g = ul >> 8 & 0xFF;
        b = ul & 0xFF;
    }


    uint32_t toCompound() {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
};
