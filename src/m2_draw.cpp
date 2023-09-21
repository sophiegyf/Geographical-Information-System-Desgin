
#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"


#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m3data.h"
#include "ui.h"
#include "uiHelp.h"

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <LatLon.h>

#include "database.h"
#include "zoom.h"

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



void draw_main_canvas(ezgl::renderer *g){
   auto start = std::chrono::system_clock::now();
   if(file == "/cad2/ece297s/public/maps/tehran_iran.streets.bin") g->format_font("Noto Kufi Arabic", static_cast<ezgl::font_slant>(0), static_cast<ezgl::font_weight>(0));
   else g->format_font("Noto Sans CJK SC", static_cast<ezgl::font_slant>(0), static_cast<ezgl::font_weight>(0));
   draw_main_canavs_blank(g);
   
   draw_features(g);
   draw_streets(g);
   draw_bus(g);
   draw_subway(g);
   draw_street_name(g);
   draw_feature_name(g);
   draw_paths(g);
   
   draw_POI(g);
   draw_highlight_intersection(g);

   draw_main_ui(g);


   auto end = std::chrono::system_clock::now();
   std::chrono::duration<double> diff = end - start;
   std::cout << "Time used to draw map: " << diff.count() << "seconds. \n";
}

void draw_main_canavs_blank(ezgl::renderer *g){

   // if(!night_mode) g->set_color(167, 225, 249, 255);
   // else g->set_color(31, 53, 44, 255);
   if(!night_mode) g->set_color(232, 232, 232, 255);
   else g->set_color(23, 33, 41);
   auto rec = g->get_visible_world();
   g->fill_rectangle({rec.m_first.x, rec.m_first.y}, {rec.m_second.x, rec.m_second.y});
}


void draw_features(ezgl::renderer *g){
   double coef = pow(1.5, get_zoom_level(g)) / pow(1.5, 10);
   //used for record the printed feature
   std::vector<ezgl::point2d> printed;
   for(int current_feature_id = 0; current_feature_id < getNumFeatures(); ++ current_feature_id){
      //get info of each feature
      auto feature = feature_data[current_feature_id];
      auto bound = feature.feature_boundary;
      auto screen_view = g->get_visible_world();
      if(bound.max_y < screen_view.bottom() || bound.min_y > screen_view.top() || bound.max_x < screen_view.left() || bound.min_x > screen_view.right()) continue;
      auto curve_points = feature.curve_point;
      auto type = feature.type;

         //Define the colour for different features
         switch (type){
         case UNKNOWN:
            g->set_color(ezgl::RED);
            break;

         case PARK:
            if(night_mode == false) g->set_color(180, 227, 170, 255);
            else g->set_color(31, 51, 43);
            break;
         
         case BEACH:
            if(night_mode == false) g->set_color(255, 239, 195, 255);
            else g->set_color(23, 33, 41);
            break;

         case LAKE:
            if(night_mode == false) g->set_color(115, 188, 245, 255);
            else g->set_color(2, 4, 5);
            break;
         
         case RIVER:
            g->set_line_width(45 * coef);
            if(night_mode == false) g->set_color(115, 188, 245, 255);
            else g->set_color(2, 4, 5);
            break;
         
         case ISLAND:
            if(night_mode == false) g->set_color(237, 235, 204, 255);
            else g->set_color(35, 55, 102);
            break;
         
         case BUILDING:
            if(get_zoom_level(g) <= 7){
               continue;
            }
            if(night_mode == false) g->set_color(170, 181, 191, 255);
            else g->set_color(32, 58, 71);
            break;
         
         case GREENSPACE:
            if(night_mode == false) g->set_color(165, 207, 148, 255);
            else g->set_color(31, 51, 43);
            break;
         
         case GOLFCOURSE:
            if(night_mode == false) g->set_color(165, 207, 148, 255);
            else g->set_color(31, 51, 43);
            break;

         case STREAM:
            g->set_line_width(25 * coef);
            if(night_mode == false) g->set_color(115, 188, 245, 255);
            else g->set_color(2, 4, 5);
            break;
         
         case GLACIER:
            g->set_color(181, 226, 244, 255);
            break;
         
         default:
            continue;
            //g->set_color(255, 255, 255, 255);
            break;
         }

         if(curve_points.size() > 1 && curve_points[0] == curve_points[curve_points.size() - 1]){
            g->fill_poly(curve_points);
            
         }
         else{
            for(int idx = 1; idx < curve_points.size(); ++ idx){
               g->draw_line(curve_points[idx - 1], curve_points[idx]);
            }
         }

   }
   
}





void draw_feature_name(ezgl::renderer *g){
   if(file == "/cad2/ece297s/public/maps/tehran_iran.streets.bin") g->format_font("Noto Kufi Arabic", static_cast<ezgl::font_slant>(0), static_cast<ezgl::font_weight>(0));
   else g->format_font("Noto Sans CJK SC", static_cast<ezgl::font_slant>(0), static_cast<ezgl::font_weight>(0));
   g->set_font_size(10);
   g->set_color(ezgl::BLACK);  
   g->set_text_rotation(0);
   auto rec = g->get_visible_world();
   double x = rec.m_second.x - rec.m_first.x;
   double y = rec.m_second.y - rec.m_first.y;
   double xy = sqrt(pow(x, 2) + pow(y, 2)) / 5;
   double distance = 0;
   double avg_x;
   double avg_y;
   //used for record the printed feature
   std::vector<ezgl::point2d> printed;

   //traverse each feature
   for(int feature_id = 0; feature_id < getNumFeatures(); ++ feature_id){
      auto feature = feature_data[feature_id];
      if(feature.name == "<noname>") continue; 
      //display the feature name when zoom to a fit level
      if((get_zoom_level(g) >= 3 && feature.area >= 30000) || (get_zoom_level(g) > 9)){

         avg_x = feature_data[feature_id].x_avg;
         avg_y = feature_data[feature_id].y_avg;
         ezgl::point2d avg = {avg_x, avg_y};
         //if it's out of the screen
         if(!in_screen(g, avg)) continue;

         //set colour for each feature
         //if(feature.name == "<noname>") continue;     
         if(feature.type == PARK || feature.type == GREENSPACE) g->set_color(24, 128, 56, 255);
         else if(feature.type == BUILDING) g->set_color(83, 87, 92, 255);
         else if(feature.type == LAKE) g->set_color(77, 129, 198, 255);
         else if(feature.type == ISLAND) g->set_color(129, 145, 167, 255);
         else if(feature.type == BEACH) g->set_color(161, 252, 3, 255);
         else continue;
         bool too_close = false;
      
         //compare the distance of each feature
         //return if they are too close
         if(printed.size() > 0){
            for(int print_id = 0; print_id < printed.size(); ++ print_id){
               auto temp = printed[print_id];
               double x_length = pow(temp.x - avg_x, 2);
               double y_length = pow(temp.y - avg_y, 2);
               distance = sqrt(x_length + y_length);
               
               if(distance < xy) {
                  too_close = true;
                  break;
                  
               }
            }
         }
         //mark the drawn feature, when new feature comes compare their distance
         if(too_close == false){
            g->draw_text({avg_x, avg_y}, feature.name, sqrt(pow(avg_x, 2) + pow(avg_y, 2)), 100);
            printed.push_back({avg_x, avg_y});
         }
         
      }
     
      
   }

}


void draw_streets(ezgl::renderer *g){
   double zoom_level = get_zoom_level(g);
   for(int segment_id = 0; segment_id < getNumStreetSegments(); ++ segment_id){
      //traverse each segments
      auto segment_info = getStreetSegmentInfo(segment_id);
      auto street_type = street_curve[segment_id].type;
      if(!in_view(g, street_curve[segment_id].curve_point)) continue;

      if(street_type == "primary" || street_type == "motorway" || street_type == "motorway_link"){
         if(!night_mode)g->set_color(255, 242, 175, 255);
         else g->set_color(51, 109, 138);
         if (zoom_level <= 5) g->set_line_width(6);
         else if (zoom_level <= 7) g->set_line_width(7);
         else if (zoom_level <= 9) g->set_line_width(10);
         else if (zoom_level <= 11) g->set_line_width(12);
         else if (zoom_level <= 13) g->set_line_width(17);
         else g->set_line_width(26);
         draw_one_street(g, segment_info, segment_id);
         
      }
      else if(street_type == "secondary" || street_type == "secondary_link"){
         //draw_one_street(g, segment_info, segment_id);
         if(!night_mode)g->set_color(216, 199, 240, 255);
         else g->set_color(68, 95, 115);
         //g->set_color(180, 175, 255, 255);
         if (zoom_level <= 3) g->set_line_width(0);
         else if (zoom_level <= 5) g->set_line_width(3);
         else if (zoom_level <= 9) g->set_line_width(5);
         else if (zoom_level <= 11) g->set_line_width(9);
         else if (zoom_level <= 13) g->set_line_width(12);
         else g->set_line_width(14);
         draw_one_street(g, segment_info, segment_id);
      }
      else if(street_type == "tertiary" || street_type == "tertiary_link"){
         if(!night_mode)g->set_color(134, 184, 164, 255);
         else g->set_color(68, 95, 115);
         //g->set_color(180, 175, 255, 255);
         if (zoom_level <= 7) continue;
         // else if (zoom_level <= 5) g->set_line_width(0);
         else if (zoom_level <= 9) g->set_line_width(3);
         // else if (zoom_level <= 11) g->set_line_width(7);
         // else if (zoom_level <= 13) g->set_line_width(10);
         else g->set_line_width(10);
         draw_one_street(g, segment_info, segment_id);
      }
      else if(street_type == "residential"){
         if(!night_mode)g->set_color(134, 166, 184, 255);
         else g->set_color(68, 95, 115);
         if (zoom_level <= 7) continue;
         else if (zoom_level <= 9) g->set_line_width(3);
         else g->set_line_width(10);
         draw_one_street(g, segment_info, segment_id);
      }
      else{
         if(!night_mode)g->set_color(255, 255, 255, 255);
         else g->set_color(68, 95, 115);
         if (zoom_level <= 7) continue;
         else if (zoom_level <= 9) g->set_line_width(3);
         else g->set_line_width(10);
         draw_one_street(g, segment_info, segment_id);
      }
      
   }
}

void draw_one_street(ezgl::renderer *g, StreetSegmentInfo& seg_info, int seg_id){
   double x_from = intersection_data[seg_info.from].x;
   double y_from = intersection_data[seg_info.from].y;
   double x_to = intersection_data[seg_info.to].x;
   double y_to = intersection_data[seg_info.to].y;
   auto curve_point = street_curve[seg_id].curve_point;
   for(int num = 0; num < curve_point.size() - 1; ++ num){
      //if(!in_screen(g, curve_point[num]) && !in_screen(g, curve_point[num + 1])) continue;
      g->draw_line(curve_point[num], curve_point[num + 1]);
      if(num == 0){
         g->draw_line({x_from, y_from}, curve_point[num]);
      }
      if(num == curve_point.size() - 2){
         g->draw_line(curve_point[num + 1], {x_to, y_to});
      }
   }

}


void draw_street_name(ezgl::renderer *g){
   if(get_zoom_level(g) <= 7) return;
   double ang;
   for(int seg_id = 0; seg_id < getNumStreetSegments(); ++ seg_id){
      auto curve_point = street_curve[seg_id].curve_point;
       if(!in_view(g, curve_point)) continue;
      auto seg_info = getStreetSegmentInfo(seg_id);
      if(getStreetName(seg_info.streetID) == "<unknown>") continue;
      for(int num = 0; num < curve_point.size() - 1; ++ num){
         double x_from = curve_point[num].x;
         double y_from = curve_point[num].y;
         double x_to = curve_point[num + 1].x;
         double y_to = curve_point[num + 1].y;
         ezgl::point2d pos_fr = {x_from, y_from};
         ezgl::point2d pos_to = {x_to, y_to};
         if(!in_screen(g, pos_fr) && !in_screen(g, pos_to)) continue;
         double xx_length = x_to - x_from;
         double yy_length = y_to - y_from;
         double x_m = 0.5 * (x_from + x_to);
         double y_m = 0.5 * (y_from + y_to);
         ezgl::point2d pos = {x_m, y_m};
         if(!in_screen(g, pos)) continue;
         double rad = yy_length / xx_length;
         if(xx_length == 0) ang = 90;
         else ang = atan(rad) * 180 / PI;
         draw_partial_name(g, seg_info, x_m, y_m, xx_length, yy_length, ang); 
      }
      
   }
}

void draw_partial_name(ezgl::renderer *g, StreetSegmentInfo& seg, double x_mid, double y_mid, double x_length, double y_length, double angle){
   g->set_color(85, 85, 85, 255);
   //std::cout << angle << " ";
   //g->set_text_rotation(angle);
   //if(angle < 0) angle = 360 + angle;
   g->set_font_size(10);
   // rotate with angle we get
   g->set_text_rotation(angle);
   std::string name = getStreetName(seg.streetID);
   if(seg.oneWay){
      //check directions
      if(x_length > 0 || y_length > 0){
         g->draw_text({x_mid, y_mid}, "--->" + name + "--->", 1.15 * sqrt(pow(x_length, 2) + pow(y_length, 2)), 50);
      }
      else{
         g->draw_text({x_mid, y_mid}, "<---" + name + "<---", 1.15 * sqrt(pow(x_length, 2) + pow(y_length, 2)), 50);
      }
   }
   else{
      //std:: cout << sqrt(pow(x_length, 2) + pow(y_length, 2)) << std::endl;
      g->draw_text({x_mid, y_mid}, name, 1.15 * sqrt(pow(x_length, 2) + pow(y_length, 2)), 50);
      //g->draw_text({x_mid, y_mid}, name);
   }
}




void draw_subway_routes(ezgl::renderer *g) {
   if (get_zoom_level(g) <= 6) return;
   g->set_line_width(5);
   for(int idx = 0; idx < subway_data.size(); ++ idx){
      auto subway_points = subway_data[idx].points;
      // if(!in_view(g ,subway_points)) continue;
      for (auto point = 0; point < subway_points.size(); point++) {
         if(!in_view(g, subway_points[point])) continue;
         for(int tag = 0; tag < subway_points[point].size() - 1; ++ tag){
            auto from = subway_points[point][tag];
            auto to = subway_points[point][tag + 1];
            g->set_color(subway_data[idx].color);
            g->draw_line(from, to);
         }
        
      }
   }
}

void draw_subway_station(ezgl::renderer *g) {
   if(get_zoom_level(g) <= 6) return;
   for(int idx = 0; idx < subway_station.size(); ++ idx){
      if(!in_screen(g, subway_station[idx].first)) continue;

      if(!night_mode)g->set_color(232, 44, 78);
      else g->set_color(158, 158, 163);
      g->fill_arc(subway_station[idx].first, 0.00003, 0, 360);
      g->set_font_size(10);
      g->draw_text({subway_station[idx].first.x - 0.0001, subway_station[idx].first.y - 0.0001}, subway_station[idx].second, 0.002, 100);

   }
}

void draw_subway(ezgl::renderer *g) {
   if(get_zoom_level(g) <= 7) return;
   draw_subway_routes(g);
   draw_subway_station(g);
}

void draw_bus(ezgl::renderer *g){
   if(get_zoom_level(g) < 8) return;
   for(int seg_id = 0; seg_id < bus_line.size(); ++ seg_id){
      if(!in_view(g, bus_curve_point[seg_id])) return;

      for(int idx = 0; idx < bus_curve_point[seg_id].size() - 1; ++ idx){
         auto from = bus_curve_point[seg_id][idx];
         auto to = bus_curve_point[seg_id][idx + 1];
         //std::cout << "1 " << from.x << " " << from.y << " " << to.x << " " << to.y << std::endl;
 
      if(!in_screen(g, from) && !in_screen(g, to)) continue;
         if(!night_mode)g->set_color(232, 162, 187);
         else g->set_color(79, 65, 107);
         g->set_line_width(3);
         g->draw_line(from, to);
      }

   }
   

}


void draw_highlight_intersection(ezgl::renderer *g) {
   if(get_zoom_level(g) < 7) return;

   for(int inter_id = 0; inter_id < intersection_data.size(); ++ inter_id){
      if(intersection_data[inter_id].highlighted == true){
         g->set_color(ezgl::RED);
         g->fill_arc({intersection_data[inter_id].x, intersection_data[inter_id].y}, 0.00004, 0, 360);
      }
   }
}

void draw_POI(ezgl::renderer *g){
   if(get_zoom_level(g) <= 10) return;
   //load png for POI
   //ezgl::surface *general = ezgl::renderer::load_png("libstreetmap/resources/general.png");
   ezgl::surface *fastfood = ezgl::renderer::load_png("libstreetmap/resources/fastfood.png");
   ezgl::surface *parking = ezgl::renderer::load_png("libstreetmap/resources/parking.png");
   ezgl::surface *hosipital = ezgl::renderer::load_png("libstreetmap/resources/hosipital.png");
   ezgl::surface *library = ezgl::renderer::load_png("libstreetmap/resources/library.png");
   ezgl::surface *airport = ezgl::renderer::load_png("libstreetmap/resources/airport.png");
   ezgl::surface *cafe = ezgl::renderer::load_png("libstreetmap/resources/cafe.png");
   ezgl::surface *bank = ezgl::renderer::load_png("libstreetmap/resources/bank.png");
   ezgl::surface *bus_station = ezgl::renderer::load_png("libstreetmap/resources/busstation.png");
   ezgl::surface *restaurant = ezgl::renderer::load_png("libstreetmap/resources/nightclub.png");
   ezgl::surface *university = ezgl::renderer::load_png("libstreetmap/resources/university.png");

   //set font info
   if(file == "/cad2/ece297s/public/maps/tehran_iran.streets.bin") g->format_font("Noto Kufi Arabic", static_cast<ezgl::font_slant>(0), static_cast<ezgl::font_weight>(0));
   else g->format_font("Noto Sans CJK SC", static_cast<ezgl::font_slant>(0), static_cast<ezgl::font_weight>(0));
   g->set_font_size(9); 
   g->set_text_rotation(0);
   auto world = g->get_visible_world();
   //g->set_coordinate_system(ezgl::SCREEN);
   double x = world.m_second.x - world.m_first.x;
   double y = world.m_second.y - world.m_first.y;
   double xy = sqrt(pow(x, 2) + pow(y, 2) / 5);
   //double xy = pow(7 * pow(1.5, 15 - get_zoom_level(g)), 2);
   std::vector<ezgl::point2d> printed;
   //only when we have zoom level at 10 or more
   
   //traverse each POI
   for(int poi_id = 0; poi_id < POI_data.size(); ++ poi_id){
      auto poi_info = POI_data[poi_id];
      double x_coord = poi_info.x;
      double y_coord = poi_info.y;
      if(x_coord > world.right() || x_coord < world.left() || y_coord > world.top() || y_coord < world.bottom()) continue;
      // ezgl::point2d coord = {x_coord, y_coord};
      // if(!in_screen(g, coord)) continue;
      std::string poi_name = POI_data[poi_id].name;

      bool too_close = false;
      if(printed.size() > 0){
         for(int print_id = 0; print_id < printed.size(); ++ print_id){
            auto temp = printed[print_id];
            double x_length = pow(temp.x - x_coord, 2);
            double y_length = pow(temp.y - y_coord, 2);
            double distance = sqrt(x_length + y_length);

            //compare the distance to what we have drawn
            if(distance < xy) {                  
               too_close = true;
               break;
               
            }
         }
      }
      if(too_close == false){
         ezgl::point2d position = {x_coord, y_coord};
         g->set_color(252, 152, 3, 255);
         
         //draw different type of poi
      
         if(poi_info.type == "fastfood"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(fastfood, position);
         }
         else if(poi_info.type == "parking"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(parking, position);
         }
         else if(poi_info.type == "hosipital"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(hosipital, position);
         }
         else if(poi_info.type == "library"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(library, position);
         }
         else if(poi_info.type == "airport"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(airport, position);
         }
         else if(poi_info.type == "cafe"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(cafe, position);
         }
         else if(poi_info.type == "bank"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(bank, position);
         }
         else if(poi_info.type == "bus_station"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(bus_station, position);
         }
         else if(poi_info.type == "restaurant"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(restaurant, position);
         }
         else if(poi_info.type == "university"){
            g->draw_text({x_coord, y_coord}, poi_name, sqrt(pow(x_coord, 2) + pow(y_coord, 2)), 100);
            g->draw_surface(university, position);
         }
         else{
            continue;
         }
         //mark the drawn POI, when new POI comes compare their distance
         printed.push_back({x_coord, y_coord});
      }

   
   }

   //free the surface
   //ezgl::renderer::free_surface(general);
   ezgl::renderer::free_surface(fastfood);
   ezgl::renderer::free_surface(parking);
   ezgl::renderer::free_surface(hosipital);
   ezgl::renderer::free_surface(library);
   ezgl::renderer::free_surface(airport);
   ezgl::renderer::free_surface(cafe);
   ezgl::renderer::free_surface(bank);
   ezgl::renderer::free_surface(bus_station);
   ezgl::renderer::free_surface(restaurant);
   ezgl::renderer::free_surface(university);
}

//draw path segments 
void path(ezgl::renderer* g, std::vector<StreetSegmentIdx> path_segments) {
    for (auto segment : path_segments) {
        g->set_color(ezgl::RED);
        g->set_line_width(5);
        StreetSegmentInfo info = getStreetSegmentInfo(segment);
        draw_one_street(g, info, segment);
    }
}

void draw_paths(ezgl::renderer* g) {
   

   if (multi_intersections.size() == 0) return;
   
   auto start = ezgl::renderer::load_png("libstreetmap/resources/general.png");
   auto end = ezgl::renderer::load_png("libstreetmap/resources/general.png");

   if (multi_intersections.size() == 1){ 
      g->draw_surface(end, {intersection_data[multi_intersections[0]].x, intersection_data[multi_intersections[0]].y});
      ezgl::renderer::free_surface(start);
      ezgl::renderer::free_surface(end);
      return;
   }
   
   std::vector<int>intersections_found;
   for (int i = 0; i < multi_intersections.size()-1; i++) {
      intersections_found = findPathBetweenIntersections({multi_intersections[i], multi_intersections[i+1]}, 15);
      path(g, intersections_found);
   }
   for (int i = 0; i < multi_intersections.size(); i++) {
      int inter_id = multi_intersections[i];
      if (i == 0) {
         g->draw_text({intersection_data[inter_id].x+0.0001, intersection_data[inter_id].y+0.0001}, "start", 5, 100);
         g->draw_surface(start, {intersection_data[inter_id].x, intersection_data[inter_id].y});
      } else if (i == multi_intersections.size()-1) {
         g->draw_text({intersection_data[inter_id].x+0.0001, intersection_data[inter_id].y+0.0001}, "end", 5, 100);
         g->draw_surface(end, {intersection_data[inter_id].x, intersection_data[inter_id].y});
      } else {
         g->draw_text({intersection_data[inter_id].x+0.0001, intersection_data[inter_id].y+0.0001}, std::to_string(i), 5, 100);
      }
      
      //draw_direction(g);
   }
   draw_street_name(g);

   ezgl::renderer::free_surface(start);
   ezgl::renderer::free_surface(end);
}

std::pair<std::vector<std::string>,std::vector<std::string>> draw_direction(ezgl::renderer* /*g*/) {
   std::pair <std::vector<std::string>,std::vector<std::string>> direc_info;
   ui_spec.direction_sentence.clear();
   ui_spec.directions.clear();
   if (multi_intersections.size() > 1) {
      double distance = 0.0;
      double time;
      auto intersections_found = findPathBetweenIntersections({multi_intersections[0], multi_intersections[1]}, 15);
      Direction direction;
      std::string turn;
      double segment_distance;

      ////////////////////////////////////////ui purposes
      // ui_spec.direction_sentence.clear();
      // ui_spec.directions.clear();
      std::stringstream ss;
      ss << "Starting at " << intersection_data[multi_intersections[0]].name << ".";
      ui_spec.direction_sentence.push_back(ss.str());
      ui_spec.directions.push_back("start");
      ss.str(std::string());

      if (multi_intersections.size() == 2) {
         if (intersections_found.size() == 1) {
            direction = find_direction_between_points(intersection_data[multi_intersections[0]].position, intersection_data[multi_intersections[1]].position);   
            distance = street_segment_length[intersections_found[0]];

            ////////////////////////////////ui purposes
            ui_spec.directions.push_back(getDirectionString(direction));
            ss << "Heading " << getDirectionString(direction) << " for " << (int) distance << " meters.";
            ui_spec.direction_sentence.push_back(ss.str());
            ss.str(std::string());
         } else {
            direction = find_direction_between_points(intersections_found[0], intersections_found[1]);   
            distance = street_segment_length[intersections_found[0]];

            ///////////////////////////////////ui purposes
            ui_spec.directions.push_back(getDirectionString(direction));
            ss << "Go " << getDirectionString(direction) << " for " << (int) distance << " meters.";
            ui_spec.direction_sentence.push_back(ss.str());
            ss.str(std::string());

            for (auto segment = 0; segment < intersections_found.size()-1; segment++) {
               turn = find_turn(intersections_found[segment], intersections_found[segment+1]);
               segment_distance = street_segment_length[intersections_found[segment]];
               distance += segment_distance;

               /////////////////////////////////////////ui purpose
               ui_spec.directions.push_back(turn);

               if (turn == "straight"){
                  ss << "Go Straight for " << (int) segment_distance << " meters.";
                  ui_spec.direction_sentence.push_back(ss.str());
                  ss.str(std::string());
               }
               else{
                  ss << "Turn " << turn << " and go straight for " << (int) segment_distance << " meters.";
                  ui_spec.direction_sentence.push_back(ss.str());
                  ss.str(std::string());
               }
            }
   
         }
         ss << "Arrive at " << intersection_data[multi_intersections[1]].name;
         ui_spec.directions.push_back("arrive");
         ui_spec.direction_sentence.push_back(ss.str());
         ss.str(std::string());
      } else {
         
         for (int i = 0; i < multi_intersections.size() - 1; i++) {
            intersections_found = findPathBetweenIntersections({multi_intersections[i], multi_intersections[i+1]}, 15);
            if (intersections_found.size() == 1) {
               direction = find_direction_between_points(intersection_data[multi_intersections[i]].position, intersection_data[multi_intersections[i+1]].position);   
               distance = street_segment_length[intersections_found[0]];

               /////////////////////////////////ui purpose
               ui_spec.directions.push_back(getDirectionString(direction));
               ss << "Go " << getDirectionString(direction) << " for " << (int) distance << " meters.";
               ui_spec.direction_sentence.push_back(ss.str());
               ss.str(std::string());
            } else {
               direction = find_direction_between_points(intersections_found[0], intersections_found[1]);   
               distance = street_segment_length[intersections_found[0]];

               ////////////////////////////////ui purpose
               ui_spec.directions.push_back(getDirectionString(direction));
               ss << "Go " << getDirectionString(direction) << " for " << (int) distance << " meters.";
               ui_spec.direction_sentence.push_back(ss.str());
               ss.str(std::string());
               for (auto segment = 0; segment < intersections_found.size()-1; segment++) {
                  turn = find_turn(intersections_found[segment], intersections_found[segment+1]);
                  segment_distance = street_segment_length[intersections_found[segment]];
                  distance += segment_distance;

                  ///////////////////////////////////ui purpose
                  ui_spec.directions.push_back(turn);

                  if (turn == "straight"){
                     ss << "Go Straight for " << (int) segment_distance << " meters.";
                     ui_spec.direction_sentence.push_back(ss.str());
                     ss.str(std::string());
                  } 
                  else{
                     ss << "Turn " << turn << " and go straight for " << (int) segment_distance << " meters.";
                     ui_spec.direction_sentence.push_back(ss.str());
                     ss.str(std::string());
                  }
               }
            }
            ss << "Arrive at " << intersection_data[multi_intersections[i+1]].name;
            ui_spec.directions.push_back("arrive");
            ui_spec.direction_sentence.push_back(ss.str());
            ss.str(std::string());
         }
      }

      for (int i = 0; i < ui_spec.direction_sentence.size(); i ++){
         std::cout << ui_spec.direction_sentence[i] << std::endl;
      }


      time = computePathTravelTime(multi_intersections, 15);
      std::cout << "Total distance traveled: " << distance << " meters." << std::endl;
      std::cout << "Total time taking: " << time << "seconds." << std::endl;

      
      direc_info.first = ui_spec.direction_sentence;
      direc_info.second = ui_spec.directions;
      
   } 
   return direc_info;
}


std::string getDirectionString(Direction direction) {
    switch (direction) {
        case Direction::East:
            return "east";
        case Direction::West:
            return "west";
        case Direction::North:
            return "north";
        case Direction::South:
            return "south";
        default:
            return "";
    }
}

Direction find_direction_between_points(LatLon start, LatLon end) {
   double dx = get_x_lon(end.longitude()) - get_x_lon(start.longitude());
   double dy = get_y_lat(end.latitude()) - get_y_lat(start.latitude());
   double angle = std::atan2(dy, dx) * 180.0 / M_PI;

   if (angle >= -45 && angle < 45) {
      return Direction::East;
   }
   else if (angle >= -135 && angle < -45) {
      return Direction::South;
   }
    else if (angle >= 45 && angle < 135) {
        return Direction::North;
    }
    else {
        return Direction::West;
    }
}

Direction find_direction_between_points(StreetSegmentIdx first, StreetSegmentIdx second) {
   int a, c;
   auto first_info = getStreetSegmentInfo(first);
   auto second_info = getStreetSegmentInfo(second);

   if (first_info.from == second_info.from) {
      a = first_info.to;
      c = first_info.from;
   } else if (first_info.from == second_info.to) {
      a = first_info.to;
      c = first_info.from;
   } else if (first_info.to == second_info.to) {
      a = first_info.from;
      c = first_info.to;
   } else {
      a = first_info.from;
      c = first_info.to;
   }


   double dx, dy;
   dx = intersection_data[c].x - intersection_data[a].x;
   dy = intersection_data[c].y - intersection_data[a].y;

   double angle = std::atan2(dy, dx) * 180.0 / M_PI;

   if (angle >= -45 && angle < 45) {
      return Direction::East;
   }
   else if (angle >= -135 && angle < -45) {
      return Direction::South;
   }
    else if (angle >= 45 && angle < 135) {
        return Direction::North;
    }
    else {
        return Direction::West;
    }
}

std::string find_turn(StreetSegmentIdx first, StreetSegmentIdx second) {
   int a, b, c;
   auto first_info = getStreetSegmentInfo(first);
   auto second_info = getStreetSegmentInfo(second);
   if (first_info.streetID == second_info.streetID) return "straight";

   if (first_info.from == second_info.from) {
      a = first_info.to;
      b = second_info.to;
      c = first_info.from;
   } else if (first_info.from == second_info.to) {
      a = first_info.to;
      b = second_info.from;
      c = first_info.from;
   } else if (first_info.to == second_info.to) {
      a = first_info.from;
      b = second_info.from;
      c = first_info.to;
   } else {
      a = first_info.from;
      b = second_info.to;
      c = first_info.to;
   }

   double bearing;
   double dx1, dy1, dx2, dy2;
   dx1 = intersection_data[c].x - intersection_data[a].x;
   dy1 = intersection_data[c].y - intersection_data[a].y;
   dx2 = intersection_data[b].x - intersection_data[c].x;
   dy2 = intersection_data[b].y - intersection_data[c].y;
   bearing = dx1*dy2 - dy1*dx2;

   if (bearing > 0) return "left";
   else return "right";
}
