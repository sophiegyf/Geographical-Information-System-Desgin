# ifndef INTERSECTION_H
# define INTERSECTION_H

# include "LatLon.h"
# include <vector>
# include "StreetsDatabaseAPI.h"

struct intersection{
    
    LatLon position;
    std::string name; 

    int numSegmentConnects;
    std::vector <StreetSegmentIdx> segmentConnects;
    bool highlighted = false;

};
#endif