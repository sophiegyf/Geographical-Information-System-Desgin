#pragma once 
#include <vector>
#include "LatLon.h"
# include "StreetsDatabaseAPI.h"
using namespace std;

struct segment{
    OSMID wayOSMID;
    int streetID;
    int from, to;
    int numCurvePoints;
    float speedLimit;
    bool oneWay;
    

    double length;
    double travelTime;

    std::vector <LatLon> curvePoints;
    std::vector <pair<double,double>> curvePointsXY; 
};