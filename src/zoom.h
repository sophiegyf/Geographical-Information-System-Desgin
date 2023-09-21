#ifndef ZOOM_LEVEL_H
#define ZOOM_LEVEL_H
#include <string>
#include "StreetsDatabaseAPI.h"

struct zoom_level {
    int level;
    bool zoomed_in;
    bool zoomed_out;
};



#endif