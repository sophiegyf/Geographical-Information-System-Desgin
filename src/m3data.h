#ifndef M3DATA_H
#define M3DATA_H

#define Ratio 0.49

class pq_node{
public:
    int node_id;
    double travel_time;
    double help_time;
    
    pq_node(){
        node_id = -1;
        travel_time = 0;
        help_time = 0;
    }
    
    pq_node(int id, double tra_time, double hel_time){
        node_id = id;
        travel_time = tra_time;
        help_time = hel_time;
    }
};

inline auto comparator = [](const pq_node& first, const pq_node& second){
    return (first.travel_time + Ratio * first.help_time) > (second.travel_time + Ratio * second.help_time);
};

class node{
public:
    int segment_id;
    int parent_id;
    double travel_time;
    int visited_check; 

    node(){
        segment_id = -1;
        parent_id = -1;
        travel_time = 0;
        visited_check = 0;
    }
};


double help(int& id1, int& id2);


#endif 