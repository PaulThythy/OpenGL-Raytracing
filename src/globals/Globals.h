#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

namespace Config {
    static int SAMPLES = 5;
    static int BOUNCES = 5;

    static const int BVH_MAX_TRIANGLES_PER_LEAF = 8;

    static bool USE_BVH = true;

    static uint32_t INIT_WINDOW_WIDTH   = 800;
    static uint32_t INIT_WINDOW_HEIGHT  = 500;
}

#endif