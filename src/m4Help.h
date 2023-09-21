#ifndef M4HELP_H
#define M4HELP_H

#include "m1.h"
#include "m3.h"
#include "m3data.h"
#include "database.h"
#include <iostream>
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

#define bound_time 9999999;

class m4_pq_node{
public:
    int node_id;
    double travel_time;
    
    m4_pq_node(){
        node_id = -1;
        travel_time = 0;
    }
    
    m4_pq_node(int id, double tra_time){
        node_id = id;
        travel_time = tra_time;
    }
};

inline auto m4_comparator = [](const m4_pq_node& first, const m4_pq_node& second){
    return (first.travel_time) > (second.travel_time);
};


class m4_node{
public:
    int segment_id;
    int parent_id;
    double travel_time;
    int visited_check; 

    m4_node(){
        segment_id = -1;
        parent_id = -1;
        travel_time = 0;
        visited_check = 0;
    }
};

struct matrix_info{
    std::vector<int> path_seg;
    double time;
};

struct depot_info{
    int id;
    double time;
};

struct path_info {
    double time = 0;
    bool legal = false;
    std::vector<int> path_ids;
};


void find_matrix_path(const int start_idx, const std::set<int>& all_ends, const std::vector<int>& package,
                      const std::vector<int>& depot, std::vector<matrix_info>& matrix_row, const double turn_penalty);

void load_matrix(std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& package,
                 const std::vector<IntersectionIdx>& depots, const float turn_penalty, const std::set<int>& all_intersections);

void load_depot(const std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& package, std::vector<depot_info>& depot_data);

void greedy_search(int start, const std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& packages,
                   const std::vector<depot_info>& depot, path_info& path); 


void make_path(std::vector<CourierSubPath>& result, const std::vector<depot_info>& depot_data, const path_info& test, 
               const std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& packages,
               const std::vector<IntersectionIdx>& depots);

void SA(const path_info& temp, path_info& resultPath, const std::vector<int>& package, const std::vector<std::vector<matrix_info>>& matrix,
        const std::vector<depot_info>& depot_data, std::chrono::time_point<std::chrono::high_resolution_clock> &time_start);

void path_optimize(const path_info& new_path, path_info& result);

void reverse_path(std::vector<int>& path);
void swap_path(std::vector<int>& path);
void right_shift_path(std::vector<int>& path);
void left_shift_path(std::vector<int>& path);
void three_reverse_path(std::vector<int>& path);
void or_opt_left(std::vector<int>& path);
void or_opt_right(std::vector<int>& path);


bool order_valid_check(const path_info& test, int size);
bool time_valid_check(path_info& test, const std::vector<std::vector<matrix_info>>& matrix, const std::vector<depot_info>& depot_data);

#endif