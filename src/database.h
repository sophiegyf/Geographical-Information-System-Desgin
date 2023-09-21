#ifndef DATABASE_H
#define DATABASE_H


//#include "m2.h"
#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include "ezgl/application.hpp"
#include "ezgl/callback.hpp"
#include "ezgl/camera.hpp"
#include "ezgl/canvas.hpp"
#include "ezgl/color.hpp"
#include "ezgl/control.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"
#include "ezgl/rectangle.hpp"


#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "zoom.h"
#include <LatLon.h>


#include <vector> 
#include <iterator>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <string>
#include <float.h>




// extern std::vector<intersection> intersectionData;
// extern std::vector<street> streetData;
#define PI 3.141592653589793238
inline bool night_mode = false;
inline std::string file;
//extern zoom_level zoom_in_factor;

extern std::unordered_map<int, std::vector<int>> streetid_and_intersection; //stores intersections of street, get by street id
extern std::multimap<std::string, int> street_names; // store street id, get by street name string
extern std::vector<double> street_segment_length; //stores street segment length, get by street segment id
extern std::vector<double> street_segment_travel_time; // stores street segment travel time, getby street segment id
extern std::unordered_map<int, double> street_length; //stores street length, get by street id
extern std::vector<StreetSegmentInfo> street_segments; //stores street segment info struct, get by street segment id
extern std::vector<const OSMNode *> osm_subway_stations; //stores osm node got subwat station
extern std::vector<const OSMRelation *> osm_subway_lines;

extern std::unordered_map<int, std::vector<int>> street_with_segments; //stores segments of a street

extern std::unordered_map<OSMID, std::unordered_map<std::string, std::string>> osm_data; // by OSMID
extern std::unordered_map<int, std::vector<ezgl::point2d>> feature_curve; // stores all curve point of a feature, get by feature id

extern std::unordered_map<OSMID, const OSMWay*> way_info; // stores osm way info
extern std::unordered_map<OSMID, const OSMNode*> OSM_NODE_HASH; // store all osm node, get by osm id
extern std::vector<const OSMWay*> bus_line; // stores osmway of bus line

extern std::unordered_map<int, std::vector<ezgl::point2d>> bus_curve_point; //stores bus line curve points 
extern std::vector<std::pair<ezgl::point2d, std::string>> subway_station; // stores subway station

extern std::unordered_map<int, std::vector<int>> intersection_node;

extern std::vector<std::unordered_map<int, int>> connect_segment;
extern std::pair<IntersectionIdx, IntersectionIdx> intersections_for_path;
extern std::vector<int> multi_intersections;



enum class Direction {
    East,
    West,
    North,
    South
};

struct Subway{
    ezgl::color color;
    std::vector<std::vector<ezgl::point2d>> points;
};
extern std::vector<Subway> subway_data;

struct feature_bound{
    double max_x;
    double min_x;
    double max_y;
    double min_y;
};

struct feature_info{
    std::string name = "";
    FeatureType type;
    double area;
    std::vector<ezgl::point2d> curve_point;
    feature_bound feature_boundary;
    double x_avg;
    double y_avg;
}; 
extern std::vector<feature_info> feature_data; // stores feature info struct, get by feature id

struct bound{
    double avg_lat = 0;
    double max_lat = -181;
    double min_lat = DBL_MAX;
    double max_lon = -181;
    double min_lon = DBL_MAX;
    double speed = -1;
};
extern bound boundary;

struct POI_info{
    std::string type;
    std::string name;
    LatLon position;
    double x;
    double y;
    OSMID node_id;
    bool highlighted = false;
};
extern std::vector<POI_info> POI_data; // stores POI sturct, get by POI id

struct intersection_info{
    LatLon position;
    double x, y;
    std::string name; 
    std::vector<int> street;
    std::vector<IntersectionIdx> adjacent;
    std::vector <int> segments;
    bool highlighted = false;

};
extern std::vector<intersection_info> intersection_data;

struct segment_info{
    std::string type;
    std::vector<ezgl::point2d> curve_point;
};
extern std::vector<segment_info> street_curve;


void load_POI();
void load_boundary();
void load_feature();
void load_length();
void load_osm();
void load_street_names();
void load_seg_and_intersection();
void remove(std::vector<int> &v);
double latlon_to_x_coord(LatLon point, double avg_lat);
double latlon_to_y_coord(LatLon point);
void convert_string(std::string& str);
double get_x_lon(double lon);
double get_y_lat(double lat);
double get_lon_x(double x);
double get_lat_y(double y);

void load_street_data();
// void load_intersection_data();


void draw_main_canvas(ezgl::renderer *g);
void draw_main_canavs_blank(ezgl::renderer *g);
void draw_features(ezgl::renderer *g);
void draw_POI(ezgl::renderer *g);
void draw_subway(ezgl::renderer *g);
int  get_zoom_level(ezgl::renderer *g);
bool enable_display(ezgl::rectangle rec, double x, double y);
ezgl::point2d right_position(ezgl::rectangle rec, ezgl::rectangle screen, ezgl::point2d point);
void draw_feature_name(ezgl::renderer *g);
// void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);
// void act_on_key_press(ezgl::application *app, GdkEventKey* event, char* key);
std::pair<int, double> find_closest_poi_distance(LatLon current_position);
void reset_highlight();
// void initial_setup(ezgl::application *app, bool /*new_window*/);
std::string map_selection();
void draw_one_street(ezgl::renderer *g, StreetSegmentInfo& seg_info, int seg_id);
void draw_streets(ezgl::renderer *g);
void draw_street_name(ezgl::renderer *g);
void draw_partial_name(ezgl::renderer *g, StreetSegmentInfo& seg, double x_mid, double y_mid, double x_length, double y_length, double angle);
bool in_screen(ezgl::renderer *g, ezgl::point2d& point);
bool in_view(ezgl::renderer *g, std::vector<ezgl::point2d>& point);
void adjust_zoom_factor(double &level, ezgl::application *application);
std::string find_turn(StreetSegmentIdx first, StreetSegmentIdx second);
Direction find_direction_between_points(StreetSegmentIdx first, StreetSegmentIdx second);
Direction find_direction_between_points(LatLon start, LatLon end);
std::string getDirectionString(Direction direction);

bool OneWay(double x_coord, double y_coord);
void draw_highlight_intersection(/*ezgl::rectangle rec,*/ ezgl::renderer *g);
std::vector<int> find_partial_street_name(std::string partial_street_name);
void path(ezgl::renderer* g, std::vector<StreetSegmentIdx> path_segments);
void draw_paths(ezgl::renderer* g);
std::pair<std::vector<std::string>,std::vector<std::string>> draw_direction(ezgl::renderer* g);

// void draw_subway_station(const OSMNode *node, ezgl::renderer *g);
// void draw_subway_routes(std::vector<LatLon> subway_points, ezgl::renderer *g);
void draw_subway_routes(ezgl::renderer *g);
void draw_subway_station(ezgl::renderer *g);
void draw_bus(ezgl::renderer *g);
void load_all_subway_lines ();
void load_all_subway_station ();
void load_bus();
void load_bus_position();
void load_node_info();

std::vector<int> search_for_intersections(ezgl::application *application, std::string street1, std::string street2);
//void searchIntersection(GtkWidget *searchEntry, ezgl::application *application);
void searchIntersection(std::string street1, std::string street2, ezgl::application *application);

// void switchMap(GtkWidget *searchEntry, ezgl::application *application);
// void load_map_window(GtkWidget *searchEntry, ezgl::application *app);
void turn_on_night_mode(GtkWidget *searchEntry, ezgl::application *application);

void test();

#endif