#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

namespace Config {
    static int SAMPLES = 1;
    static int BOUNCES = 10;

    static const int BVH_MAX_TRIANGLES_PER_LEAF = 4;

    static uint32_t INIT_WINDOW_WIDTH   = 800;
    static uint32_t INIT_WINDOW_HEIGHT  = 500;
}

#endif