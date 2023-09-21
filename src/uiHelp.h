#ifndef UIHELP_H
#define UIHELP_H

// #include "m1.h"
// #include "m2.h"
// #include "m3.h"
// #include "ui.h"
// #include "database.h"
// #include "zoom.h"
// #include "m3data.h"
// #include "street.h"
// #include "intersection.h"
// #include "segment.h"

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
#include <math.h>
#include<bits/stdc++.h>
#include <boost/algorithm/string.hpp>

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <LatLon.h>

#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include <gdk/gdkkeysyms.h>
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"


struct {
    ezgl::rectangle search_bar1 = {{10,40}, {275,10}};
    ezgl::rectangle search_bar2 = {{10,75}, {275,45}};

    ezgl::rectangle search_button = {{248,72}, {275,45}};

    string search_entry1;
    string search_entry2;

    bool search1_activated;
    bool search2_activated;


    std::vector <StreetIdx> drop_down_search_result;
    std::vector <ezgl::rectangle> drop_down_boxes;


    bool directions_activated;
    int direction_start_index;
    int next_line;
    ezgl::rectangle direction_box = {{20,115},{370, 85}};
    std::vector <ezgl::rectangle> direction_drop_down_boxes;
    std::vector <std::string> direction_sentence;
    std::vector <std::string> directions;


    bool esc_page_activated;
    ezgl::point2d mouse_highlight_location;

    ezgl::rectangle esc_load_map_button;
    ezgl::rectangle esc_settings_button;
    ezgl::rectangle esc_help_button;

    ezgl::point2d load_map_text_location;
    ezgl::point2d settings_text_location;
    ezgl::point2d help_text_location;

    bool esc_load_map_highlight;
    bool esc_setting_highlight;
    bool esc_help_highlight;


    bool help_activated;
    int help_page = 1;


    bool setting_activated;
    ezgl::rectangle night_mode_bar;

    
    
}ui_spec;


#endif