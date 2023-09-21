#include "m1.h"
#include "m3.h"
#include "m4.h"
#include "m3data.h"
#include "m4Help.h"
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
#include <random>
#include <bits/stdc++.h> 
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <string.h>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/bind/bind.hpp>

void find_matrix_path(const int start_idx, const std::set<int>& all_ends, const std::vector<int>& package,
                         const std::vector<int>& depot, std::vector<matrix_info>& matrix_row, const double turn_penalty){

    std::unordered_map<int, m4_node> m4_node_data(intersection_data.size());

    std::priority_queue<m4_pq_node, std::vector<m4_pq_node>, decltype(m4_comparator)> list(m4_comparator);

    std::unordered_map<int, matrix_info> path;

    for(auto& i : all_ends) path[i] = {{}, -1};

    list.push({start_idx, 0});

    int intersection_num = 0;

    while(!list.empty() && intersection_num < all_ends.size()){
        auto& top = list.top();
        int current_id = top.node_id;
        list.pop();

        if(m4_node_data[current_id].visited_check == 1) continue;
        m4_node_data[current_id].visited_check = 1;

        if(all_ends.find(current_id) != all_ends.end()){
            int curr_id = current_id;
            std::vector<int> segs;
            while(curr_id != start_idx){
                segs.insert(segs.begin(), m4_node_data[curr_id].segment_id);
                curr_id = m4_node_data[curr_id].parent_id;
            }
            path[current_id].path_seg = segs;
            path[current_id].time = m4_node_data[current_id].travel_time;
            intersection_num += 1;
        }

        for(int& seg_id : intersection_data[current_id].segments){
            auto& info = street_segments[seg_id];
            int next_id = (info.from == current_id) ? info.to : info.from;
            if(m4_node_data[next_id].visited_check) continue;

             //situtation of oneWay, make sure the direction is valid
            bool one_way = true;
            if(info.oneWay) one_way = (info.from == current_id && info.to == next_id);
            if(!one_way) continue;

             //calculate the previous time, turn cost, and time to adjacent node
            double total_time = street_segment_travel_time[seg_id];
            if(m4_node_data[current_id].segment_id != -1){
                if(info.streetID != street_segments[m4_node_data[current_id].segment_id].streetID) total_time += turn_penalty;
            }
            total_time += m4_node_data[current_id].travel_time;

             //if this node is new or better than prev, update the info
            if(m4_node_data[next_id].travel_time == 0 || total_time < m4_node_data[next_id].travel_time){
                m4_node_data[next_id].travel_time = total_time;
                m4_node_data[next_id].segment_id = seg_id;
                m4_node_data[next_id].parent_id = current_id;

                list.push({next_id, total_time});
            }  
        }
    }

    for(int i = 0; i < matrix_row.size(); ++ i){
        matrix_row[i] = i < package.size() ? path[package[i]] : path[depot[i - package.size()]];
    }
    

}

void load_matrix(std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& package,
                 const std::vector<IntersectionIdx>& depots, const float turn_penalty, const std::set<int>& all_intersections){
    #pragma omp parallel for num_threads(8)
    for(int i = 0; i < matrix.size(); i ++) {
        matrix[i].resize(matrix.size());
        int start = i < package.size() ? package[i] : depots[i - package.size()];
        find_matrix_path(start, all_intersections, package, depots, matrix[i], turn_penalty);
    }
}

void load_depot(const std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& package, std::vector<depot_info>& depot_data){
    #pragma omp parallel for num_threads(8)
    for(int i = 0; i < package.size(); i ++) {
        depot_data[i].id = -1;
        depot_data[i].time = -1;
        for(int depot_id = package.size(); depot_id < matrix.size(); ++ depot_id){
            if(i % 2 == 0){
                if(matrix[depot_id][i].time != -1){
                    if(depot_data[i].time == -1 || matrix[depot_id][i].time < depot_data[i].time){
                        depot_data[i].id = depot_id;
                        depot_data[i].time = matrix[depot_id][i].time;
                    } 
                }
                
            }
            else{
                if(matrix[i][depot_id].time != -1){
                    if(depot_data[i].time == -1 || matrix[i][depot_id].time < depot_data[i].time) {
                        depot_data[i].id = depot_id;
                        depot_data[i].time = matrix[i][depot_id].time;
                    } 
                }
                
            }
        }
    }
}


void greedy_search(int start, const std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& packages,
                                       const std::vector<depot_info>& depot, path_info& path){

    std::set<int> checked;
    path.path_ids.push_back(start);

    while(path.path_ids.size() != packages.size()){
        checked.insert(start);

        int next_id = -1;
        double max_time = bound_time;
        for(int i = 0; i < packages.size(); ++ i){
            if(checked.count(i) || (matrix[start][i].time == -1) || (i % 2 == 1 && !checked.count(i - 1))) continue;
            double current_time = matrix[start][i].time;
            if(next_id == -1 || current_time < max_time){
                next_id = i;
                max_time = current_time;
            }
        }
        if(next_id == -1) return;
        start = next_id;
        path.time += max_time;
        path.path_ids.push_back(start);
    }
    
    if(depot[path.path_ids[0]].time == -1 || depot[path.path_ids.back()].time == -1){
        return;
    } 
    path.legal = true;
    path.time = depot[path.path_ids[0]].time + depot[path.path_ids.back()].time + path.time;
    
}


void make_path(std::vector<CourierSubPath>& result, const std::vector<depot_info>& depot_data, const path_info& test, 
               const std::vector<std::vector<matrix_info>>& matrix, const std::vector<int>& package,
               const std::vector<IntersectionIdx>& depots){
    
    CourierSubPath temp;
    int start = depot_data[test.path_ids[0]].id;
    temp.subpath = matrix[start][test.path_ids[0]].path_seg;
    temp.start_intersection = depots[start - package.size()];
    temp.end_intersection = package[test.path_ids[0]];
    
    result.push_back(temp);

    for(int i = 1; i < package.size(); ++ i){
        if(package[test.path_ids[i - 1]] == package[test.path_ids[i]] && test.path_ids[i - 1] % 2 == test.path_ids[i] % 2) continue;
        temp.subpath = matrix[test.path_ids[i - 1]][test.path_ids[i]].path_seg;
        temp.start_intersection = package[test.path_ids[i - 1]];
        temp.end_intersection = package[test.path_ids[i]];
        
        result.push_back(temp);
    }

    
    int end = depot_data[test.path_ids.back()].id;
    temp.subpath = matrix[test.path_ids[package.size() - 1]][end].path_seg;
    temp.start_intersection = package[test.path_ids[package.size() - 1]];
    temp.end_intersection = depots[end - package.size()];
    
    result.push_back(temp);

}


void SA(const path_info& temp, path_info& result, const std::vector<int>& package, const std::vector<std::vector<matrix_info>>& matrix,
        const std::vector<depot_info>& depot_data, std::chrono::time_point<std::chrono::high_resolution_clock> &time_start){
    std::mt19937 rand_num({std::random_device{}()});
    std::uniform_real_distribution<double> rate_range(0.0, 1.0);

    result = temp;
    path_info current_best = temp;
    path_info test;

    int size = package.size();
    double start_tempture = 300;
    int iterating_length = 4800;
    double delay_coef = 0.999;

    while(start_tempture > 1) {
        int current_length = 0;
        while(current_length < iterating_length) {
            auto time_end = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration<double, std::milli>(time_end - time_start).count();
            if(diff > 49850) {
                if(current_best.time < result.time) result = current_best;
                return;
            }
            path_optimize(result, test);
            test.legal = order_valid_check(test, size) && time_valid_check(test, matrix, depot_data);
            if(!test.legal) continue;
            if(test.time < result.time) {
                result = test;
                if(test.time < current_best.time) current_best = test;
            } 
            else{
                double accept_rate = exp((result.time - test.time) / start_tempture);
                if(accept_rate >= rate_range(rand_num)) result = test;
            }
            current_length ++;
        }
        start_tempture = delay_coef * start_tempture;
    }
    
}

void path_optimize(const path_info& new_path, path_info& result){
    result = new_path;
    switch (rand() % 6){
        case 0:
            swap_path(result.path_ids);
        break;

        case 1:
            reverse_path(result.path_ids);
        break;

        case 2:
            left_shift_path(result.path_ids);
        break;

        case 3:
            right_shift_path(result.path_ids);
        break;

        case 4:
            or_opt_left(result.path_ids);
        break;

        case 5:
            or_opt_right(result.path_ids);
        break;

        case 6:
            three_reverse_path(result.path_ids);
        break;

        default:
        break;
    }
}

void reverse_path(std::vector<int>& path){
    int x, y;
    int first, second;
    while(1){
        first = rand() % (path.size() + 1);
        second = rand() % (path.size() + 1);
        x = fmin(first, second);
        y = fmax(first, second);
        if(x != y){
            reverse(path.begin() + x, path.begin() + y);
            return;
        }
    }
}

void three_reverse_path(std::vector<int>& path){
    int x, y, z;
    double first, second, third;
    std::vector<int> temp(3);
    while(1){
        first = rand() % (path.size() + 1);
        second = rand() % (path.size() + 1);
        third = rand() % (path.size() + 1);
        if(first != second && second != third && first != third) break;
    }   
    temp.push_back(first);
    temp.push_back(second);
    temp.push_back(third);

    std::sort(temp.begin(), temp.end());
    x = temp[1];
    y = temp[2];
    z = temp[3];
    reverse(path.begin() + x, path.begin() + y);
    reverse(path.begin() + y, path.begin() + z);
    return;


}

void swap_path(std::vector<int>& path) {
    int x , y;
    while(1){
        x = rand() % path.size();
        y = rand() % path.size();
        if(x != y){
            auto temp = path[x];
            path[x] = path[y];
            path[y] = temp;
            return;
        }
    }
}

void left_shift_path(std::vector<int>& path){
    int x, y;
    int first, second;
    while(1){
        first = rand() % (path.size());
        second = rand() % (path.size());
        x = fmin(first, second);
        y = fmax(first, second);
        if(x != y){
            auto temp = path[x];
            for(int i = x; i < y; ++ i) {
                path[i] = path[i + 1];
            }
            path[y] = temp;
            return;
        }
    }
}

void right_shift_path(std::vector<int>& path){
    int x, y;
    int first, second;
    while(1){
        first = rand() % (path.size());
        second = rand() % (path.size());
        x = fmin(first, second);
        y = fmax(first, second);
        if(x != y){
            auto temp = path[y];
            for(int i = y; i > x; -- i) {
                path[i] = path[i - 1];
            }
            path[x] = temp;
            return;
        }
    }
}

void or_opt_left(std::vector<int>& path){
    auto temp = path;
    int first = rand() % path.size();
    int second = rand() % path.size();
    int x = fmin(first, second);
    int y = fmax(first, second);
    int break_point = 1 + rand() % 3;

    while(y - x < 5){
        first = rand() % path.size();
        second = rand() % path.size();
        x = fmin(first, second);
        y = fmax(first, second);
    }

    int id = x;
    for(int i = x + break_point; i < y; ++ i, ++ id){
        path[id] = temp[i];
    }
    for(int i = x; i < x + break_point; ++ i, ++ id){
        path[id] = temp[i];
    }
}


void or_opt_right(std::vector<int>& path){
    auto temp = path;
    int first = rand() % path.size();
    int second = rand() % path.size();
    int x = fmin(first, second);
    int y = fmax(first, second);
    int break_point = 1 + rand() % 3;
    while(y - x < 5){
        first = rand() % path.size();
        second = rand() % path.size();
        x = fmin(first, second);
        y = fmax(first, second);
    }

    int id = x;
    for(int i = y - break_point; i < y; ++ i, ++ id){
        path[id] = temp[i];
    }
    for(int i = x; i < y - break_point; ++ i, ++ id){
        path[id] = temp[i];
    }
}


bool order_valid_check(const path_info& test, int size){
   std::vector<bool>check(size / 2);
    for(int i = 0; i < test.path_ids.size(); ++ i){
        auto idx = test.path_ids[i];
        if(idx % 2 == 0) check[idx / 2] = true;
        else if(!check[idx / 2]) return false; 
    }
    return true;
}

bool time_valid_check(path_info& test, const std::vector<std::vector<matrix_info>>& matrix, const std::vector<depot_info>& depot_data){
    test.time = 0;
    for(int i = 0; i < test.path_ids.size() - 1; ++ i){
        int start = test.path_ids[i];
        int end = test.path_ids[i + 1];
        if(matrix[start][end].time == -1) return false;
        test.time += matrix[start][end].time;
    }
    
    if(depot_data[test.path_ids[0]].time == -1 || depot_data[test.path_ids.back()].time == -1) return false;
        
    test.time = test.time + depot_data[test.path_ids[0]].time + depot_data[test.path_ids.back()].time; 
    return true;
}