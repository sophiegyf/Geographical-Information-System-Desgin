#pragma once
#include <vector>
#include <string>
# include "StreetsDatabaseAPI.h"
#include "segment.h"

struct street{
    StreetIdx id = -1;
    std::string name;
    vector <segment> segments;
    vector <IntersectionIdx> intersections;

    bool highlighted = false;
    bool highway;
    bool primary;
    bool tertiary;
};