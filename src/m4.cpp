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
#include <thread>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>

std::vector<CourierSubPath> travelingCourier(
                            const std::vector<DeliveryInf>& deliveries,
                            const std::vector<IntersectionIdx>& depots,
                            const float turn_penalty){
    
    auto time_start = std::chrono::high_resolution_clock::now();
    std::vector<CourierSubPath> result;                            
    std::set<int> all_intersections;
    std::vector<int> package;
    srand(time(0));

    for(int i = 0; i < deliveries.size(); ++ i){
        auto& info = deliveries[i];
        
        package.push_back(info.pickUp);
        package.push_back(info.dropOff);

        all_intersections.insert(info.pickUp);
        all_intersections.insert(info.dropOff);
    }
    for(int i = 0; i < depots.size(); ++ i){
        auto& depot_id = depots[i];
        all_intersections.insert(depot_id);
    }

    // matrix of distance and time from one node to other;
    std::vector<std::vector<matrix_info>> matrix(package.size() + depots.size());
    std::vector<depot_info> depot_data(package.size());

    load_matrix(matrix, package, depots, turn_penalty, all_intersections);
    load_depot(matrix, package, depot_data);

    std::unordered_map<int, path_info> all_solutions(8);

    std::vector<int> valid_id;
    #pragma omp parallel for num_threads(8)
    for(int i = 0; i < 3; i ++) {
        path_info temp;
        int start = rand() % package.size();
        while(start % 2 != 0){
            start = rand() % package.size();
        }
        greedy_search(start, matrix, package, depot_data, temp);
        if(temp.legal && (package.size() > 100)) {
            //std::cout << "processing" << std::endl;
            SA(temp, all_solutions[i], package, matrix, depot_data, time_start); 
        } 
    }
    
    for(int i = 0; i < 3; ++ i){
        auto& path = all_solutions[i];
        if(path.legal) valid_id.push_back(i);
    }
    double current_time = bound_time;
    path_info path;
    if(valid_id.size() != 0){
        for(int& i : valid_id){
            path_info testing = all_solutions[i];
            if(testing.time < current_time){
                path = testing;
                current_time = testing.time;
            }
        }
        make_path(result, depot_data, path, matrix, package, depots);
        return result;
    }
    
    path_info random_path;
    int i = rand() % package.size();
    while(i % 2 != 0){
        i = rand() % package.size();
    }
    greedy_search(i, matrix, package, depot_data, random_path);
    if(random_path.legal){
        make_path(result, depot_data, random_path, matrix, package, depots);
        return result;
    }
    
    return result;
}


