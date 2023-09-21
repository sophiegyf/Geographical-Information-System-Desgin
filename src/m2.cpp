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

#include "m2.h"
#include "ui.h"
#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include <gdk/gdkkeysyms.h>
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

// #include "ezgl/callback.hpp"
// #include "ezgl/camera.hpp"
// #include "ezgl/canvas.hpp"
// #include "ezgl/color.hpp"
// #include "ezgl/control.hpp"

// #include "ezgl/point.hpp"
// #include "ezgl/rectangle.hpp"


#include "m1.h"
#include "m3.h"
#include "ui.h"

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <LatLon.h>

// #include "intersection.h"
// #include "segment.h"
// #include "street.h"
#include "database.h"
#include "zoom.h"
#include "m3data.h"

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


int highlighted_street = 0;
zoom_level zoom_in_factor;
int first_intersection = -1;
int second_intersection = -1;
std::vector<int> multi_intersections;

void drawMap() {
   // Set up the ezgl graphics window and hand control to it, as shown in the 
   // ezgl example program. 
   // This function will be called by both the unit tests (ece297exercise) 
   // and your main() function in main/src/main.cpp.
   // The unit tests always call loadMap() before calling this function
   // and call closeMap() after this function returns.
   ezgl::application::settings settings;
   settings.main_ui_resource = "libstreetmap/resources/main.ui";
   settings.window_identifier = "MainWindow";
   settings.canvas_identifier = "MainCanvas";
   ezgl::application application(settings);

   ezgl::rectangle initial_world({get_x_lon(boundary.min_lon), get_y_lat(boundary.min_lat)}, 
                                 {get_x_lon(boundary.max_lon), get_y_lat(boundary.max_lat)});

   application.add_canvas("MainCanvas", draw_main_canvas, initial_world);

   
   application.run(initial_setup, act_on_mouse_click, act_on_mouse_move, act_on_key_press);
   //application.run(initial_setup, nullptr, nullptr, nullptr);
   zoom_in_factor.level = 0;
}



bool enable_display(ezgl::rectangle rec, double x, double y){
   // return !(rec.top() < rec.bottom() || rec.bottom() > rec.top() || rec.left() > rec.right() || rec.left() < rec.left());
   if(rec.m_first.x < x && rec.m_second.x > x && rec.m_first.y < y && rec.m_second.y > y) return true;
   return false;
}

ezgl::point2d right_position(ezgl::rectangle rec, ezgl::rectangle screen, ezgl::point2d point) {
   double x = point.x;
   double y = point.y;
   double ratio = (rec.m_second.x - rec.m_first.x) / (screen.m_second.x - screen.m_first.x);
   double delta_x = ratio * 30;
   double delta_y = ratio * 30;
   return  {x - delta_x, y + delta_y};
}

//identify one-way roads
bool OneWay(double x_coord, double y_coord) {
   if (x_coord == 0 && y_coord > 0) {
      return true;
   } else if (x_coord > 0) {
      return true;
   } else {
      return false;
   }
}



//function to get current zoom level
int get_zoom_level(ezgl::renderer *g){
   auto rec = g->get_visible_world();
   return -log((rec.m_second.x - rec.m_first.x) * (rec.m_second.y - rec.m_first.y));;
}


std::pair<int, double> find_closest_poi_distance(LatLon current_position){
   int idx = 0;
   double pre_distance = findDistanceBetweenTwoPoints(current_position, getPOIPosition(0));
   //std::cout << pre_distance << " ";
   for(int poi_id = 1; poi_id < POI_data.size(); ++ poi_id){
      double current_distance = findDistanceBetweenTwoPoints(current_position, getPOIPosition(poi_id));
      if(current_distance < pre_distance){
         idx = poi_id;
         pre_distance = current_distance;
      }
   }
   return std::make_pair(idx, pre_distance);
}

void reset_highlight(){
   for(int poi_id = 0; poi_id < POI_data.size(); ++ poi_id){
      POI_data[poi_id].highlighted = false;
   }
   for(int inter_id = 0; inter_id < intersection_data.size(); ++ inter_id){
      intersection_data[inter_id].highlighted = false;
   }
   std::cout << "reset" << std::endl;
}


//Intersection Search inclduing search for partial input
std::vector<int> search_for_intersections(ezgl::application *application, std::string street1, std::string street2) {
   auto first = findStreetIdsFromPartialStreetName(street1);
   auto second = findStreetIdsFromPartialStreetName(street2);
   std::vector<int> inters;
   bool done = false;
   std::stringstream ss;
   if(first.size() == 0){
      ss << "Invalid first input   ";
      application->update_message(ss.str());
      done = true;
   }
   if(second.size() == 0){
      
      ss << "Invalid second input   ";
      application->update_message(ss.str());
      done = true;
   }
   if(done == true){
      std::cout << "No search" << std::endl;
      return inters;
   }
   for(int id1 = 0; id1 < first.size(); ++ id1){
      for(int id2 = 0; id2 < second.size(); ++ id2){
         auto intersections = findIntersectionsOfTwoStreets(first[id1], second[id2]);
         inters.insert(inters.end(), intersections.begin(), intersections.end());
      }
   }
   if(inters.size() == 0){
      ss << "No intersections between 2 streets";
      application->update_message(ss.str());
   }
   remove(inters);
   return inters;
   
}

void searchIntersection(std::string street1, std::string street2, ezgl::application *application) {

   std::vector<int> intersection_ids = search_for_intersections(application, street1, street2);
   std::vector<double> intersection_x;
   std::vector<double> intersection_y;


   if(intersection_ids.size() > 0){
      //std::cout << "find !!!" << std::endl;
      std::cout << "Intersections of 2 streets are following: " << std::endl;
      for(int id = 0; id < intersection_ids.size(); ++ id){
         auto idx = intersection_ids[id];
         std::cout << intersection_data[idx].name << std::endl;
         //if (intersection_data[idx].highlighted = false) {
            intersection_data[idx].highlighted = true;
            if (multi_intersections.size() == 0) {
                  multi_intersections.push_back(idx);
               } else {
                  multi_intersections.push_back(idx);
                  //std::cout << "add" << std::endl;
                  draw_paths(application->get_renderer());
                  ui_spec.direction_sentence = draw_direction(application->get_renderer()).first;
                  ui_spec.directions = draw_direction(application->get_renderer()).second;
               }
               application->refresh_drawing();
         //} 
         // double xpos = intersection_data[idx].x;
         // double ypos = intersection_data[idx].y;
         // double level;
         // ezgl::canvas* cnv = application->get_canvas(application->get_main_canvas_id());
         // adjust_zoom_factor(level, application);
         // ezgl::zoom_in(cnv, {xpos, ypos}, level);
         application->refresh_drawing();
      }

       ezgl::canvas* cnv = application->get_canvas(application->get_main_canvas_id());

       ezgl::rectangle world({get_x_lon(boundary.min_lon), get_y_lat(boundary.min_lat)}, 
                            {get_x_lon(boundary.max_lon), get_y_lat(boundary.max_lat)});
       ezgl::zoom_fit(cnv, world);

      //  double xscale = (get_x_lon(boundary.max_lon) - get_x_lon(boundary.min_lon))/800;
      //  double yscale = (get_y_lat(boundary.max_lat) - get_y_lat(boundary.min_lat))/600;

      //  double x = (intersection_x[0] - get_x_lon(boundary.min_lon)) * xscale;
      //  double y = (intersection_y[0] - get_y_lat(boundary.min_lat)) * yscale;
      //  ezgl::zoom_in(cnv, {x, y}, 9.0);


      application->refresh_drawing();
      std::stringstream ss;
      ss << "Intersection between " << street1 << " & " << street2;
      application->update_message(ss.str());
   }else{
      std::stringstream ss;
      ss << "No intersections found.";
      application->update_message(ss.str());
   }
   return;
}

void turn_on_night_mode(GtkWidget */*searchEntry*/, ezgl::application *application){
   if(night_mode == true) night_mode = false;
   else if (night_mode == false) night_mode = true;
   else night_mode = false;

   application -> refresh_drawing();
}

void adjust_zoom_factor(double &level, ezgl::application *application){
   ezgl::rectangle size = application->get_renderer()->get_visible_world();
   double x1 = size.m_first.x;
   double x2 = size.m_second.x;
   double y1 = size.m_first.y;
   double y2 = size.m_second.y;

   double area = (x2 - x1) * (y2 - y1);

   level = log(area) * (-1);
}


bool in_screen(ezgl::renderer *g, ezgl::point2d& point){
   auto screen_view = g->get_visible_world();
   if(point.x > screen_view.right() || point.x < screen_view.left() || point.y > screen_view.top() || point.y < screen_view.bottom()){
      return false;
   }
   return true;
}

bool in_view(ezgl::renderer *g, std::vector<ezgl::point2d>& point){
   auto world = g->get_visible_world();
   ezgl::rectangle rec = {{world.left() - 100, world.bottom() - 100}, {world.right() + 100, world.top() + 100}};
   for(auto& id : point){
      if(rec.contains(id.x, id.y)){
         return true;
      }
   }
   return false;
}

