/* 
 * Copyright 2023 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <vector> 
#include <iterator>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <string>
#include <cctype>
#include <cmath>
#include <tuple>
#include <float.h>
#include <cfloat>
#include<bits/stdc++.h>
#include <boost/algorithm/string.hpp>

#include "m1.h"
#include "m3.h"
#include "m4.h"
#include "m4Help.h"
#include "m3data.h"
#include "ui.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <LatLon.h>


/* 
 * Implemented by Kyna Wu
 * declares struct including necessary for future access
 * details see each header file
 */

#include "intersection.h"
// #include "segment.h"
// #include "street.h"
#include "database.h"





//Global variable
/* 
 * Implemented by all team members
 * street_names: a vector of street names paired with corresponding street id
 * intersectionData: vector of intersection type, includes all intersection and corresponding information
 * streetData: vector of street type, includes all information including segment and intersection information
 * osm_data: stores osm id with key and data
 * street_with_its_segment: store street id with its segments id
 * street_segment_length: store street segment length
 * street_segmen_travel_time: store speed limit of each street
 * streetid_and_intersection: stores street ids and intersections for eacb
 */


// std::vector<intersection> intersectionData;
// std::vector<street> streetData;

std::unordered_map<int, std::vector<int>> street_with_segments; //stores vector of segments of a street, get by street_id;
std::vector<intersection_info> intersection_data; //stores intersection_info struct, get by by intersection_id
std::unordered_map<int, std::vector<int>> streetid_and_intersection; //stores intersections of street, get by street id
std::multimap<std::string, int> street_names; // store street id, get by street name string
std::vector<double> street_segment_length; //stores street segment length, get by street segment id
std::vector<double> street_segment_travel_time; // stores street segment travel time, getby street segment id
std::unordered_map<int, double> street_length; //stores street length, get by street id
std::vector<StreetSegmentInfo> street_segments; //stores street segment info struct, get by street segment id
std::vector<feature_info> feature_data; //stores feature info struct, get by feature id
std::unordered_map<OSMID, std::unordered_map<std::string, std::string>> osm_data; // by OSMID
std::vector<POI_info> POI_data; // stores POI struct, get by POI id
std::unordered_map<int, std::vector<ezgl::point2d>> feature_curve;
std::vector<const OSMRelation *> osm_subway_lines;
std::vector<Subway> subway_data;
std::unordered_map<OSMID, const OSMNode*> OSM_NODE_HASH;
std::unordered_map<OSMID, const OSMWay *> way_info;
std::vector<const OSMWay*> bus_line;
std::unordered_map<int, std::vector<ezgl::point2d>> bus_curve_point;
std::vector<std::pair<ezgl::point2d, std::string>> subway_station;
std::vector<segment_info> street_curve;
std::unordered_map<int, std::vector<int>> intersection_node;
//std::unordered_map<int, node> node_data;
std::vector<std::unordered_map<int, int>> connect_segment;
std::pair<IntersectionIdx, IntersectionIdx> intersections_for_path;

//std::unordered_map<int, m4_node> m4_node_data;
bound boundary;

//Helper functions declaration
/* 
 * load information from database to global variables declared above
 */
void load_POI(){
    for(int poi_id = 0; poi_id < getNumPointsOfInterest(); ++ poi_id){
        POI_info temp;
        temp.position = getPOIPosition(poi_id);
        temp.x = get_x_lon(temp.position.longitude());
        temp.y = get_y_lat(temp.position.latitude());
        temp.node_id = getPOIOSMNodeID(poi_id);
        POI_data.push_back(temp);
        POI_data[poi_id].type = getPOIType(poi_id);
        POI_data[poi_id].name = getPOIName(poi_id);
    }

}
void load_boundary(){
    //night_mode = false;
    double max_lat = getIntersectionPosition(0).latitude();
    double min_lat = max_lat;
    double max_lon = getIntersectionPosition(0).longitude();
    double min_lon = max_lon;
    std::unordered_map<int, LatLon> intersects;
    for (int id = 0; id < getNumIntersections(); id ++) {
        intersects[id] = getIntersectionPosition(id);
        max_lat = std::max(max_lat, intersects[id].latitude());
        min_lat = std::min(min_lat, intersects[id].latitude());
        max_lon = std::max(max_lon, intersects[id].longitude());
        min_lon = std::min(min_lon, intersects[id].longitude());
    }
    boundary.max_lat = max_lat;
    boundary.min_lat = min_lat;
    boundary.max_lon = max_lon;
    boundary.min_lon = min_lon;
    boundary.avg_lat = (max_lat + min_lat) / 2;

}
//implemented by Kyna Wu
//loads all information to street vector, including segment information and intersections
//Segment information: OSMID wayOSMID, streetID, from, to, numCurvePoints, speedLimit, oneWay, length, travelTime, curvePoints;
//Also including intersections on each streets
void load_seg_and_intersection(){
    std::vector<std::set<int>> street_intersection_set;
    street_intersection_set.resize(getNumStreets());
    //streetid_and_intersection.reserve(getNumStreets());
    for(int intersection_id = 0; intersection_id < getNumIntersections(); ++ intersection_id){
        for(int segnum = 0; segnum < getNumIntersectionStreetSegment(intersection_id); ++ segnum){
            int seg_idx = getIntersectionStreetSegment(intersection_id, segnum);
            StreetSegmentInfo seg_info = getStreetSegmentInfo(seg_idx);         
            //streetid_and_intersection[seg_info.streetID].push_back(intersection_id);
            street_intersection_set[seg_info.streetID].insert(intersection_id);
        }
    }
    //for(int i = 0; i < getNumStreets(); ++ i){
    //    remove(streetid_and_intersection[i]);
    //}
    for (size_t s = 0; s < street_intersection_set.size(); s++) {
        std::vector<int> intersections;
        intersections.assign(street_intersection_set[s].begin(), street_intersection_set[s].end());
        streetid_and_intersection.insert(std::make_pair(s, intersections));
    }
    // streetid_and_intersection.reserve(getNumStreets());
    // for(int intersection_id = 0; intersection_id < getNumIntersections(); ++ intersection_id){
    //     for(int segnum = 0; segnum < getNumIntersectionStreetSegment(intersection_id); ++ segnum){
    //         int seg_idx = getIntersectionStreetSegment(intersection_id, segnum);
    //         StreetSegmentInfo seg_info = getStreetSegmentInfo(seg_idx);         
    //         streetid_and_intersection[seg_info.streetID].push_back(intersection_id);
    //         //street_intersection_set[seg_info.streetID].insert(intersection_id);
    //     }
    // }
    // for(int i = 0; i < streetid_and_intersection.size(); ++ i){
    //     remove(streetid_and_intersection[i]);
    // }
    
}

//Implement by jiahao
//This function provides the hashmap with osm id with its tag as pair
void load_osm(){
    //store osm node id with its corresponding index
    osm_data.reserve(getNumberOfNodes());
    for(int osm_idx = 0; osm_idx < getNumberOfNodes(); ++ osm_idx){
        const OSMNode* e = getNodeByIndex(osm_idx);
        OSMID osm_id = e->id();
        for(int i = 0; i < getTagCount(e); ++ i){
            osm_data[osm_id].insert(getTagPair(e,i));
        }
        //OSM_NODE_HASH.insert(std::make_pair(osm_id, e));
    }

    for(int osm_idx = 0; osm_idx < getNumberOfWays(); ++ osm_idx){
        auto e = getWayByIndex(osm_idx);
        OSMID osm_id = e->id();
        way_info[osm_id] = e;
    }

}

//Implement by jiahao
//This load street function provides a map with street name as key, street id as data(can use name to find id)
void load_street_names(){
    for(int street_id = 0; street_id < getNumStreets(); ++ street_id){
        std::string street_name = getStreetName(street_id);
        convert_string(street_name);
        // streets temp;
        // street_info.push_back(temp);
        // street_info[street_id].name = street_name;
        street_names.insert(std::make_pair(street_name,street_id));
    }
}

//Implemented by Kyna
//this loads streetData, including segments in on street
void load_street_data(){
    street_with_segments.reserve(getNumStreets());
    connect_segment.resize(getNumIntersections());
    //intersection_data.reserve(getNumIntersections());
    for(int inter = 0; inter < getNumIntersections(); ++ inter){
        intersection_info temp;
        intersection_data.push_back(temp);
        intersection_data[inter].name = getIntersectionName(inter);    
        intersection_data[inter].position = getIntersectionPosition(inter);
        intersection_data[inter].x = get_x_lon(intersection_data[inter].position.longitude());
        intersection_data[inter].y = get_y_lat(intersection_data[inter].position.latitude());

    }
    
    for(int intersection_id = 0; intersection_id < getNumIntersections(); ++ intersection_id){
        //std::cout << "from ";
        for(int segment_intersection_id = 0; segment_intersection_id < getNumIntersectionStreetSegment(intersection_id); ++ segment_intersection_id){
            int segment_id = getIntersectionStreetSegment(intersection_id, segment_intersection_id);
            //std::cout << segment_id << " ";
            intersection_data[intersection_id].segments.push_back(segment_id);
            auto segment_info = getStreetSegmentInfo(segment_id);
            street_with_segments[segment_info.streetID].push_back(segment_id);
            intersection_data[intersection_id].street.push_back(segment_info.streetID);      
            
            if(segment_info.from == intersection_id){

                intersection_data[intersection_id].adjacent.push_back(segment_info.to);
            }
            else if(segment_info.to == intersection_id && !segment_info.oneWay){

                intersection_data[intersection_id].adjacent.push_back(segment_info.from);
            }
        }     
    }
    for(int inter_id = 0; inter_id < getNumIntersections(); ++ inter_id){
        remove(intersection_data[inter_id].adjacent);
    }
}






//Implement by Sophie
//This load street function maps the value of street segment length into the vector according to the index 
void load_length() {
    street_segment_length.reserve(getNumStreetSegments());
    street_segment_travel_time.reserve(getNumStreetSegments());
    street_length.reserve(getNumStreets());

    double total_speed = 0;
    for (int street_segment_id = 0; street_segment_id < getNumStreetSegments(); street_segment_id++) {
        //StreetSegmentInfo street_segment_info = getStreetSegmentInfo(street_idx);
        segment_info temp;
        street_curve.push_back(temp);   ////

        StreetSegmentInfo street_segment_info = getStreetSegmentInfo(street_segment_id);

        total_speed += street_segment_info.speedLimit;
        //if(street_segment_info.speedLimit > boundary.speed) boundary.speed = street_segment_info.speedLimit;
        
        street_segments.push_back(street_segment_info);

        auto way_id = street_segment_info.wayOSMID;
        auto way = way_info[way_id];
        for(int count = 0; count < getTagCount(way); ++ count){
            auto tag = getTagPair(way, count);
            if(tag.first == "highway"){
                street_curve[street_segment_id].type = tag.second;
                break;
            }
        }

        int curve_point_num = street_segment_info.numCurvePoints;
        IntersectionIdx from = street_segment_info.from;
        IntersectionIdx to = street_segment_info.to;

        LatLon first_point = getIntersectionPosition(from);
        double point_x = get_x_lon(first_point.longitude());
        double point_y = get_y_lat(first_point.latitude());
        street_curve[street_segment_id].curve_point.push_back({point_x, point_y});




        LatLon second_point;
        double find_street_segment_length = 0;
        //if street is a straight line with no curve points 
        for(int curve_num = 0; curve_num < curve_point_num; ++ curve_num){
            second_point = getStreetSegmentCurvePoint(street_segment_id, curve_num);

            double x = get_x_lon(second_point.longitude());
            double y = get_y_lat(second_point.latitude());
            street_curve[street_segment_id].curve_point.push_back({x, y});///

            find_street_segment_length += findDistanceBetweenTwoPoints(first_point, second_point);
            first_point = second_point;

            point_x = get_x_lon(first_point.longitude());
            point_y = get_y_lat(first_point.latitude());
            street_curve[street_segment_id].curve_point.push_back({point_x, point_y});
        }
        second_point = getIntersectionPosition(to);
        point_x = get_x_lon(first_point.longitude());
        point_y = get_y_lat(first_point.latitude());
        street_curve[street_segment_id].curve_point.push_back({point_x, point_y});
        find_street_segment_length += findDistanceBetweenTwoPoints(first_point, second_point);
        double speed = street_segment_info.speedLimit;
        
        // street_info[street_segment_info.streetID].seg_info[street_segment_id].street_segment_length = find_street_segment_length;
        // street_info[street_segment_info.streetID].seg_info[street_segment_id].street_segment_travel_time = find_street_segment_length / speed;
        //street_info[street_segment_info.streetID].street_length += street_info[street_segment_info.streetID].seg_info[street_segment_id].street_segment_length;
        street_segment_length.push_back(find_street_segment_length);
        street_segment_travel_time.push_back(find_street_segment_length / speed);
        street_length[street_segment_info.streetID] += find_street_segment_length;
    }
    boundary.speed = total_speed / getNumStreetSegments();

   
}

void load_feature(){
    feature_data.reserve(getNumFeatures());
    feature_curve.reserve(getNumFeatures());
    // std::unordered_map<std::vector<std::pair<double, double>>> feature_curve;
    for(int feature_id = 0; feature_id < getNumFeatures(); ++ feature_id){
        double area = 0.0;
        double total_lat = 0.0;
        double avg_lat = 0.0;
        double lat_first = getFeaturePoint(feature_id, 0).latitude();
        double lon_first = getFeaturePoint(feature_id, 0).longitude();
        double lat_second = getFeaturePoint(feature_id, getNumFeaturePoints(feature_id) - 1).latitude();
        double lon_second = getFeaturePoint(feature_id, getNumFeaturePoints(feature_id) - 1).longitude();

        feature_info temp;
        temp.type = getFeatureType(feature_id);
        feature_data.push_back(temp);
        feature_data[feature_id].name = getFeatureName(feature_id);

        auto nums_point = getNumFeaturePoints(feature_id);
        
        std::vector<LatLon> coord;
        
        double curr_x = 0;
        double curr_y = 0;

        double x_total = 0;
        double y_total = 0;
        //shoelace formula to calculate the area
        for(int point = 0; point < nums_point; ++ point){
            //total_lat += getFeaturePoint(feature_id, point).latitude();
            
            auto position = getFeaturePoint(feature_id, point);
            total_lat += position.latitude();
            curr_x = get_x_lon(position.longitude());
            curr_y = get_y_lat(position.latitude());
            feature_data[feature_id].curve_point.push_back({curr_x, curr_y});
            // feature_curve[feature_id].push_back({curr_x, curr_y});
            x_total += curr_x;
            y_total += curr_y;

            if(curr_x > feature_data[feature_id].feature_boundary.max_x) feature_data[feature_id].feature_boundary.max_x = curr_x;
            if(curr_x < feature_data[feature_id].feature_boundary.min_x) feature_data[feature_id].feature_boundary.min_x = curr_x;
            if(curr_y > feature_data[feature_id].feature_boundary.max_y) feature_data[feature_id].feature_boundary.max_y = curr_y;
            if(curr_y < feature_data[feature_id].feature_boundary.min_y) feature_data[feature_id].feature_boundary.min_y = curr_y;

            // coord.push_back(getFeaturePoint(feature_id, point));

        }
        avg_lat = total_lat / nums_point;
        feature_data[feature_id].x_avg = x_total / nums_point;
        feature_data[feature_id].y_avg = y_total / nums_point;
        //Return 0 if it is not a closed polygon
        if(lat_first != lat_second || lon_first != lon_second){
            area = 0;
            feature_data[feature_id].area = area;
            continue;
        }
        for(int current = 0; current < getNumFeaturePoints(feature_id); ++ current){
            int next = (current + 1) % getNumFeaturePoints(feature_id);
            double x_coord = kDegreeToRadian * getFeaturePoint(feature_id,current).longitude() * cos(kDegreeToRadian * avg_lat);
            double y_coord = kDegreeToRadian * getFeaturePoint(feature_id,current).latitude();
            double x_next = kDegreeToRadian * getFeaturePoint(feature_id,next).longitude() * cos(kDegreeToRadian * avg_lat);
            double y_next = kDegreeToRadian * getFeaturePoint(feature_id,next).latitude();
            //feature_data[feature_id].curve_point.push_back({x_coord, y_coord});
            area += (x_coord * y_next - y_coord * x_next) * pow(kEarthRadiusInMeters, 2);
           
        }

        if(area > 0){
            area = 0.5 * area;
        } else {
            area =  -0.5 * area; 
        }
        feature_data[feature_id].area = area;
    }
    
}

//std::vector<std::pair<ezgl::point2d, std::string>> subway_station;
void load_all_subway_station () {
    // std::unordered_map<TypedOSMID, const OSMNode*> OSM_NODE_HASH;
   //std::vector<const OSMNode *> osm_subway_stations;
   std::string name;
   for (int idx = 0; idx < getNumberOfNodes(); idx++) {
      const OSMNode* currNode = getNodeByIndex(idx);
      OSM_NODE_HASH.insert(std::make_pair(currNode->id(), currNode));

      for (auto count = 0; count < getTagCount(currNode); count++){
            std::pair<std::string, std::string> tagPair = getTagPair(currNode, count);
            if (tagPair.first == "station" && tagPair.second == "subway") {
                for (auto id = 0; id < getTagCount(currNode); id++) {
                    std::pair<std::string, std::string> get_tag;
                    get_tag = getTagPair(currNode, id);
                    if (get_tag.first == "name") {
                        name = get_tag.second;
                        break;
                }
            }

            double x = get_x_lon((getNodeCoords(currNode)).longitude());
            double y = get_y_lat((getNodeCoords(currNode)).latitude());
            ezgl::point2d pos = {x, y};
            subway_station.push_back(std::make_pair(pos, name));
            
            break;
        }
      }
    }

}

void load_all_subway_lines () {
   //std::vector<const OSMRelation *> osm_subway_lines;
    for (int idx = 0; idx < getNumberOfRelations(); idx++) {
      const OSMRelation *currRel = getRelationByIndex(idx);
      for (auto count = 0; count < getTagCount(currRel); count++) {
         std::pair<std::string, std::string> tagPair = getTagPair(currRel, count);
         if (tagPair.first == "route" && tagPair.second == "subway") {
            osm_subway_lines.push_back(currRel);
            break;
         }
      }
    }

    std::unordered_map<OSMID, const OSMWay*> OSM_WAY_HASH;
    for (int num = 0; num < getNumberOfWays(); num++) {
        const OSMWay *curr_way = getWayByIndex(num);
        OSM_WAY_HASH.insert(std::make_pair(curr_way->id(), curr_way));
    }

   //find subway color
    for (size_t idx = 0; idx < osm_subway_lines.size(); idx++ ) {
        Subway line;         
        const OSMRelation *subway = osm_subway_lines[idx];
        for (auto count = 0; count < getTagCount(subway); count++ ) {
            std::pair<std::string, std::string> tag_pair = getTagPair(subway, count);
            if (tag_pair.first == "colour") {
                if (tag_pair.second == "yellow") {
                    line.color = ezgl::YELLOW;
                } else if (tag_pair.second == "green") {
                    line.color = ezgl::DARK_GREEN;
                } else if (tag_pair.second == "purple") {
                    line.color = ezgl::PURPLE;
                } else if (tag_pair.second == "red") {
                    line.color = ezgl::RED;
                } else if (tag_pair.second == "blue") {
                    line.color = ezgl::BLUE;
                } else if (tag_pair.second == "navy") {
                    line.color = ezgl::BLUE;
                } else if (tag_pair.second[0] == '#') {
                    char red = std::stoi(std::string({tag_pair.second[1], tag_pair.second[2]}), 0, 16);
                    char green = std::stoi(std::string({tag_pair.second[3], tag_pair.second[5]}), 0, 16);
                    char blue = std::stoi(std::string({tag_pair.second[5], tag_pair.second[6]}), 0, 16);
                    line.color = ezgl::color(red, green, blue);
                } else {
                    line.color = ezgl::BLACK;
                }
                break;
            }
        }           
        std::vector<TypedOSMID> route_member = getRelationMembers(subway);
        std::vector<std::string> route_member_roles = getRelationMemberRoles(subway);      
        for(auto member = 0; member < route_member.size(); member++) {
            if (route_member[member].type() == TypedOSMID::Way && route_member_roles[member] != "platform") {
                const OSMWay *curr_way = OSM_WAY_HASH[route_member[member]];
                std::vector<ezgl::point2d> track_points;
                std::vector<OSMID> node_points = getWayMembers(curr_way);            
                for (auto node = 0; node < node_points.size(); node++) {
                    const OSMNode *curr_node = OSM_NODE_HASH[node_points[node]];
                    auto pos = getNodeCoords(curr_node);
                    double x = get_x_lon(pos.longitude());
                    double y = get_y_lat(pos.latitude());
                    track_points.push_back({x, y});
                }
                line.points.push_back(track_points);
            }
        }
        subway_data.push_back(line);
    }
}

void load_bus(){
    std::string colour;
    //std::string name;
    for(int id = 0; id < getNumberOfRelations(); ++ id){
        auto relation = getRelationByIndex(id);
        for(int tag_id = 0; tag_id < getTagCount(relation); ++ tag_id){
            auto pair = getTagPair(relation, tag_id);
            if(pair.first == "route" && (pair.second == "bus" || pair.second == "streetcar")){
                // for(int tid = 0; tid < getTagCount(relation); ++tid){
                //     auto pair_next = getTagPair(relation, tid);
                //     if(pair_next.first == "colour") colour = pair_next.second;
                // }
                auto members = getRelationMembers(relation);
                for(int r_id = 0; r_id < members.size(); ++ r_id){
                    auto info = members[r_id];
                    if(info.type() == TypedOSMID::Way){
                        auto way = way_info.find(info)->second;
                        bus_line.push_back(way);
                    }
                }
                break;
            }
        }
    }
}


void load_bus_position(){
    for(int seg_id = 0; seg_id < bus_line.size(); ++ seg_id){
        auto way = bus_line[seg_id];
        auto members = getWayMembers(way);
        for(int num = 0; num < members.size(); ++ num){
            auto way_first = OSM_NODE_HASH.find(members[num])->second;
            ezgl::point2d from = {get_x_lon(getNodeCoords(way_first).longitude()), get_y_lat(getNodeCoords(way_first).latitude())};
            bus_curve_point[seg_id].push_back(from);
        }
   }
}


void load_node_info(){
    //node_data.reserve(intersection_data.size());
    //m4_node_data.reserve(intersection_data.size());
}





double get_x_lon(double lon){
    return lon * (cos(boundary.avg_lat * kDegreeToRadian));
}

double get_y_lat(double lat){
    return lat;
}
double get_lon_x(double x){
    return x / (cos(boundary.avg_lat * kDegreeToRadian));
}
double get_lat_y(double y){
    return y;
}


//Implement by jiahao
//This function is used to check the range
template<typename Type>
bool Range_Check(Type num, Type lowerBound, Type upperBound){
    if (num < lowerBound || num > upperBound) return false;
    return true;
}
//Implement by jiahao
//This function is used to check if the target is repeat
template<typename Type>
bool Repeat(std::vector<Type> element, Type target){
    for(int i = 0; i < element.size(); ++ i){
        if(element[i] == target) return false;
    }
    return true;
}

//Implement by jiahao
//This function removes the space and conver string to lowercase
void convert_string(std::string& str){
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

//Implement by jiahao
//This function remove the same element in a vector
void remove(std::vector<int> &v){
    auto end = v.end();
    for (auto it = v.begin(); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }
 
    v.erase(end, v.end());
}

//Implement by jiahao
//This function convert laton to x, y coordinate
double latlon_to_x_coord(LatLon point, double lat_avg){
    return point.longitude() * cos(lat_avg) * kDegreeToRadian * kEarthRadiusInMeters;
   // double x_coord = kDegreeToRadian * getFeaturePoint(feature_id,current).longitude() * cos(kDegreeToRadian * avg_lat);
}
double latlon_to_y_coord(LatLon point){
    return point.latitude() * kDegreeToRadian * kEarthRadiusInMeters;
}


// loadMap will be called with the name of the file that stores the "layer-2"
// map data accessed through StreetsDatabaseAPI: the street and intersection 
// data that is higher-level than the raw OSM data). 
bool loadMap(std::string map_streets_database_filename) {
   
    std::string newFileName = map_streets_database_filename;
    std::string newTail = ".osm.bin";
    std::string oldTail = ".streets.bin";
    
    newFileName.erase(newFileName.size() - oldTail.length(), oldTail.length());
    newFileName += newTail;
    
    
    bool load_successful = loadStreetsDatabaseBIN(map_streets_database_filename) && loadOSMDatabaseBIN(newFileName); //Indicates whether the map has loaded 
                                  //successfully

    std::cout << "loadMap: " << map_streets_database_filename << std::endl;

    if(!load_successful){
        return false;
    }

    // Load your map related data structures here
    //call all loading functions
    load_boundary();
    load_feature();
    load_osm();
    load_street_names();
    load_length();
    load_seg_and_intersection();
    load_POI();
    load_street_data();
    load_all_subway_station();
    load_all_subway_lines();
    load_bus();
    load_bus_position();
    load_node_info();
    //load_successful = true; //Make sure this is updated to reflect whether
                            //loading the map succeeded or failed
    

    return load_successful;
}




void closeMap() {
    //Clean-up your map related data structures here
    street_with_segments.clear();
    intersection_data.clear();
    street_names.clear();
    osm_data.clear();
    street_segment_length.clear();
    street_segment_travel_time.clear();
    street_length.clear();
    streetid_and_intersection.clear();
    feature_data.clear();
    street_segments.clear();
    POI_data.clear();
    osm_subway_lines.clear();
    subway_data.clear();
    way_info.clear();
    feature_curve.clear();
    street_curve.clear();
    bus_line.clear();
    OSM_NODE_HASH.clear();
    bus_curve_point.clear();
    subway_station.clear();
    //node_data.clear();
    //m4_node_data.clear();
    intersection_node.clear();
    connect_segment.clear();
    multi_intersections.clear();
    
    closeOSMDatabase();
    closeStreetDatabase();
    
   
}

//Implement by sophie
//This function calculates distance between two points 
double findDistanceBetweenTwoPoints(LatLon point_1, LatLon point_2) {
    double first_latitde = point_1.latitude();
    double second_latitude = point_2.latitude();
    // acording to the formula given in handout 
    double lat_avg = (first_latitde + second_latitude)/2 * kDegreeToRadian; 
    double x1 = latlon_to_x_coord(point_1, lat_avg);
    double x2 = latlon_to_x_coord(point_2, lat_avg);
    double y1 = latlon_to_y_coord(point_1);
    double y2 = latlon_to_y_coord(point_2);
    
    //return kEarthRadiusInMeters * sqrt(pow(y2 - y1,2)+pow(x2 - x1,2)); 
    return sqrt(pow(y2 - y1,2)+pow(x2 - x1,2)); 
}

//Implement by sophie
//This function calculates the length of the street segment
double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    //check if street segment id exits
    if(!Range_Check(street_segment_id, 0, getNumStreetSegments())) return 0.0;
    //int street_id = etStreetSegmentInfo(street_segment_id).streetID;

    //return street_info[getStreetSegmentInfo(street_segment_id).streetID].seg_info[street_segment_id].street_segment_length;
    return street_segment_length[street_segment_id];
}


//Implement by sophie
//This function calculates the time required to travel through the street segment
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id) {
    // check if street segment id exits
    if (!Range_Check(street_segment_id, 0, getNumStreetSegments()-1)) {
        return 0.0;
    }
    return street_segment_travel_time[street_segment_id];
    //return street_info[getStreetSegmentInfo(street_segment_id).streetID].seg_info[street_segment_id].street_segment_travel_time;
}


//Implement by Kyna 
// Returns all intersections reachable by traveling down one street segment 
std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id){
    std::vector<IntersectionIdx> adjacent_intersections;
    // if(Range_Check(intersection_id, 0, getNumIntersections() - 1)){
    adjacent_intersections = intersection_data[intersection_id].adjacent;
    // }
    return adjacent_intersections;

}

//Implemented by Kyna
//Returns closest intersection from position provided in lat lon form
IntersectionIdx findClosestIntersection(LatLon my_position){
    int temp, index = 0;
    int closestDistance = findDistanceBetweenTwoPoints(getIntersectionPosition(0), my_position); 
    
    for(int i = 1; i < getNumIntersections(); i++){ //traverse through all intersections by index
        temp = findDistanceBetweenTwoPoints(getIntersectionPosition(i), my_position); //find distance between given point and the intersection
        if(temp < closestDistance){
            closestDistance = temp;
            index = i; //keep the index unless closer distance found
        }
    }
    return index;
}

//Implemented by Kyna
// Returns the street segments that connect to the given intersection 
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id){
    std::vector<StreetSegmentIdx> segments_intersection;
    segments_intersection = intersection_data[intersection_id].segments;
    return segments_intersection;
}

//Implemented by Kyna
// Returns all intersections along the given street.
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    std::vector<IntersectionIdx> intersection_of_street;
    intersection_of_street = streetid_and_intersection[street_id];
    return intersection_of_street;
}

//Implemented by Kyna 
// Return all intersection ids at which the two given streets intersect
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(StreetIdx street_id1, StreetIdx street_id2){
    std::vector<IntersectionIdx> result;
    if (!Range_Check(street_id1, 0, getNumStreets() - 1) || !Range_Check(street_id2, 0, getNumStreets() - 1)){
        return result;
    }
    
    auto intersections_1 = findIntersectionsOfStreet(street_id1);
    auto intersection_2 = findIntersectionsOfStreet(street_id2);
    sort(intersections_1.begin(), intersections_1.end());
    sort(intersection_2.begin(), intersection_2.end());
    for(auto it = intersections_1.begin(); it != intersections_1.end(); ++ it){
        if(std::find(intersection_2.begin(), intersection_2.end(), *it) != intersection_2.end()){
            result.push_back(*it);
        }
    }
    return result;
}

std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){
    std::vector<StreetIdx> possibleStreets;
    if(street_prefix.empty()) return possibleStreets;
    
    //remove the space and transform it to lowercap
    convert_string(street_prefix);

    auto current = street_names.lower_bound(street_prefix);
    auto last = street_names.upper_bound(street_prefix);

    if (current == street_names.end()) return possibleStreets;

    do{
        if (current->first.compare(0, street_prefix.size(), street_prefix) == 0){
            possibleStreets.push_back(current->second);
        } else break;
        current++;
    } while (current != last && current != street_names.end()); 
    
    //remove(possibleStreets);
    return possibleStreets;
}

double findStreetLength(StreetIdx street_id){
    //check if street_id is valid
    if(!Range_Check(street_id, 0, getNumStreets())) return 0.0;
    //double length = 0.0;

    // length = street_info[street_id].street_length;
    // //length = street_length[street_id];
    return street_length[street_id];
}

POIIdx findClosestPOI(LatLon my_position, std::string POItype){

    double PreDistance = 0.0;
    double CurrentDistance = 0.0;
    int PastIdx = 0;

    //locate the first POI with given type
    for(PastIdx = 0; PastIdx < getNumPointsOfInterest(); ++ PastIdx){
        if(getPOIType(PastIdx) != POItype) continue;
        PreDistance = findDistanceBetweenTwoPoints(my_position, getPOIPosition(PastIdx)); 
        break;
    }
    //Traverse the remains, update if we find a shorter one
    for(int CurrentIdx = PastIdx + 1; CurrentIdx < getNumPointsOfInterest(); ++ CurrentIdx){
        if(getPOIType(CurrentIdx) != POItype) continue;
        CurrentDistance = findDistanceBetweenTwoPoints(my_position, getPOIPosition(CurrentIdx)); 
        if(CurrentDistance < PreDistance){
            PreDistance = CurrentDistance;
            PastIdx = CurrentIdx;
        }
    }

    return PastIdx;
}

double findFeatureArea(FeatureIdx feature_id){
    double area = 0.0;
    if(!Range_Check(feature_id, 0, getNumFeatures())) return area;
    area = feature_data[feature_id].area;
    return area;

}

std::string getOSMNodeTagValue (OSMID OSMid, std::string key){
    std::string TagValue = "";
    //get the osm pair(key and data) form the osmid
    auto OSM_pair = osm_data[OSMid];
    //check if the key is exist, if it is, return the data
    if(OSM_pair.find(key) != OSM_pair.end()){
        TagValue = OSM_pair[key];
    } 
    return TagValue;
}


