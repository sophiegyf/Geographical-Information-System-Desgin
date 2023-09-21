#ifndef UI_H
#define UI_H

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "ui.h"
#include "database.h"
#include "zoom.h"
#include "m3data.h"
#include "street.h"
#include "intersection.h"
#include "segment.h"
#include "uiHelp.h"

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

void initial_setup(ezgl::application *app, bool /*new_window*/);

void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);

void act_on_mouse_move(ezgl::application *app, GdkEventButton *event, double x, double y);

void search_closest_intersection_on_click(ezgl::application* app, double x, double y);

void act_on_key_press(ezgl::application *app, GdkEventKey* event, char* key);

void load_map_window(ezgl::application *app);

void switchMap(GtkWidget *searchEntry, ezgl::application *application);

void draw_main_ui(ezgl::renderer *g);

void draw_search_bar(ezgl::renderer *g);

void draw_esc_page(ezgl::renderer *g);

void draw_dropdown(std::string search_input, ezgl::rectangle searchBar, ezgl::renderer *g);

void draw_directions_ui(ezgl::renderer *g);

void draw_settings_page(ezgl::renderer *g);

void draw_help_page(ezgl::renderer *g);

void draw_esc_background(ezgl::renderer *g);

void mouse_highlight(ezgl::renderer *g);

#endif