#include "m1.h"
#include "m3.h"
#include "m3data.h"
#include "database.h"

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <queue>

// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given turn_penalty (in seconds) per turn implied by the path.  If there is
// no turn, then there is no penalty. Note that whenever the street id changes
// (e.g. going from Bloor Street West to Bloor Street East) we have a turn.
double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, const double turn_penalty){
    double total_time = 0;
    double partial_time = 0;

    //if the path is invalid eg: empty 
    if(path.size() == 0) return total_time;

    partial_time = street_segment_travel_time[path[0]];
    total_time = partial_time;
    //if the path is single
    if(path.size() == 1) return partial_time;

    //if path are multiple
    for(int id = 1; id < path.size(); ++ id){
        partial_time = street_segment_travel_time[path[id]];
        total_time += partial_time;
        if(street_segments[path[id]].streetID != street_segments[path[id - 1]].streetID){
            total_time += turn_penalty;
        }
       
    }
    return total_time;
}

// Returns a path (route) between the start intersection (intersect_id.first)
// and the destination intersection (intersect_id.second), if one exists.
// This routine should return the shortest path
// between the given intersections, where the time penalty to turn right or
// left is given by turn_penalty (in seconds).  If no path exists, this routine
// returns an empty (size == 0) vector.  If more than one path exists, the path
// with the shortest travel time is returned. The path is returned as a vector
// of street segment ids; traversing these street segments, in the returned
// order, would take one from the start to the destination intersection.
std::vector<StreetSegmentIdx> findPathBetweenIntersections(const std::pair<IntersectionIdx, IntersectionIdx> intersect_ids,
                                                           const double turn_penalty){
    std::vector<StreetSegmentIdx> path;

    //if start = end
    if(intersect_ids.first == intersect_ids.second) return path;

    int start = intersect_ids.first;
    int end = intersect_ids.second;
    //if out of the range
    if(start < 0 || start > intersection_data.size() || end < 0 || end > intersection_data.size()) return path;
   
    //clear the data of node_data
    std::unordered_map<int, node> node_data(intersection_data.size());

    //priority queue to do the process with increasing order (time)
    std::priority_queue<pq_node, std::vector<pq_node>, decltype(comparator)> list(comparator);

    //push in the start point
    list.push({start, 0, 0});
    node_data[start].travel_time = 0;

    while(!list.empty()){
        //get the top's info
        auto& top = list.top();
        int current_id = top.node_id;
        //pop it out
        list.pop();

        //if we reach the end point, store path backwards
        if(current_id == end){
            while(current_id != start){
                auto id = node_data[current_id].segment_id;
                path.insert(path.begin(), id);
                current_id = node_data[current_id].parent_id;
            }
            return path;
        }

        //if the node has been traversed before
        if(node_data[current_id].visited_check == 1) continue;
        node_data[current_id].visited_check = 1;

        //evaluate the time to each adjacent node
        for(int& seg_id : intersection_data[current_id].segments){
            auto& info = street_segments[seg_id];
            int next_id = (info.from == current_id) ? info.to : info.from;
            if(node_data[next_id].visited_check) continue;

            //situtation of oneWay, make sure the direction is valid
            bool one_way = true;
            if(info.oneWay) one_way = (info.from == current_id && info.to == next_id);
            if(!one_way) continue;

            //calculate the previous time, turn cost, and time to adjacent node
            double total_time = street_segment_travel_time[seg_id];
            if(node_data[current_id].segment_id != -1){
                if(info.streetID != street_segments[node_data[current_id].segment_id].streetID) total_time += turn_penalty;
            }
            total_time += node_data[current_id].travel_time;

            //if this node is new or better than prev, update the info
            if(node_data[next_id].travel_time == 0 || total_time < node_data[next_id].travel_time){
                node_data[next_id].travel_time = total_time;
                node_data[next_id].segment_id = seg_id;
                node_data[next_id].parent_id = current_id;

                //Heuristic function to help speed up the search time;
                double help_time = help(next_id, end);
                list.push({next_id, total_time, help_time});
            }  
        }
    }
    return path;
}
     
//function used to calculate the absolute distance from point to point
double help(int& id1, int& id2){
    auto p1 = intersection_data[id1].position;
    auto p2 = intersection_data[id2].position;
    double distance = findDistanceBetweenTwoPoints(p1, p2);
    return distance / boundary.speed;
}
