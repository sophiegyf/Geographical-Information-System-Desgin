#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "database.h"
#include "zoom.h"
#include "m3data.h"
#include "street.h"
#include "intersection.h"
#include "segment.h"
#include "ui.h"
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
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <LatLon.h>

#include <gtk/gtk.h>
#include <gtk/gtkcomboboxtext.h>
#include <gdk/gdkkeysyms.h>
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void initial_setup(ezgl::application *app, bool /*new_window*/){
   std::stringstream ss;
   ss << "WELCOME TO OUR MAP";
   app->update_message(ss.str());

}

void search_closest_intersection_on_click(ezgl::application* app, double x, double y){
    //std::cout << "Mouse clicked at (" << x << "," << y << ") "<< std::endl;
   //app->update_message("position now");
   LatLon pos = LatLon(get_lat_y(y), get_lon_x(x));
   int inter_id = findClosestIntersection(pos);
   //std::pair<IntersectionIdx, IntersectionIdx> starting_end_points = intersections_for_path;

   //calculate the distance of poi and intersection
   double inter_distance = findDistanceBetweenTwoPoints(pos, intersection_data[inter_id].position);
   auto poi = find_closest_poi_distance(pos);
   int poi_id = poi.first;
   double poi_distance = poi.second;
   //std::cout << inter_distance << " " << poi_distance << std::endl;
   std::stringstream ss;


    //determine highlight interseciton or poi base on their distance and they should close to the poi or intersection; 
   if(poi_distance <= inter_distance && poi_distance <= 20){
      if(POI_data[poi_id].highlighted == false){
         POI_data[poi_id].highlighted = true;
         ss << "POI: " << POI_data[poi_id].name;
         app->update_message(ss.str());
         app->refresh_drawing();
         //std::cout << "poi lighted" << std::endl;
      } else{
         POI_data[poi_id].highlighted = false;
         ss << "";
         app->update_message(ss.str());
         app->refresh_drawing();
         //std::cout << "poi not lighted" << std::endl;
      }
   }else if(inter_distance < poi_distance && inter_distance <= 20){
      //app->update_message(intersectionData[inter_id].name);
            
      //std::cout << intersectionData[inter_id].name << " f  ";

      //click to update the highlight status
      if(intersection_data[inter_id].highlighted == false){
         intersection_data[inter_id].highlighted = true;
         ss << "Intersection: " << intersection_data[inter_id].name;
         app->update_message(ss.str());
         //app->refresh_drawing();
         if (multi_intersections.size() == 0) {
            multi_intersections.push_back(inter_id);
         } else {
            multi_intersections.push_back(inter_id);
            //std::cout << "add" << std::endl;
            draw_paths(app->get_renderer());
            ui_spec.direction_sentence = draw_direction(app->get_renderer()).first;
            ui_spec.directions = draw_direction(app->get_renderer()).second;
         }
         
         // ss << "Intersection: " << intersection_data[inter_id].name;
         // app->update_message(ss.str());
         // app->refresh_drawing();
         //std::cout << "inter lighted" << std::endl;
      } else{
         intersection_data[inter_id].highlighted = false;
         ss << "Remove Intersection: " << intersection_data[inter_id].name;
         app->update_message(ss.str());
         //app->refresh_drawing();
         auto found_intersection = std::find(multi_intersections.begin(), multi_intersections.end(), inter_id);
         //draw_streets(app->get_renderer());
         multi_intersections.erase(found_intersection);
         //std::sort(multi_intersections.begin(), multi_intersections.end());
         draw_paths(app->get_renderer());
         auto direct_info = draw_direction(app->get_renderer());
         ui_spec.direction_sentence = direct_info.first;
         ui_spec.directions = direct_info.second;
         // ss << "";
         // app->update_message(ss.str());
         // app->refresh_drawing();
         //draw_highlight_intersection(g);
         //std::cout << "inter not lighted" << std::endl;   
      }

      draw_main_ui(app->get_renderer());
      app->refresh_drawing();
   } 

}


void load_map_window(ezgl::application *app){
   GtkWidget *map_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title (GTK_WINDOW (map_window), "Load");
   
   //set buttons
   GtkWidget *toronto, *bei_jing, *cairo, *cape_town, *golden_horseshoe, *hamilton, *hong_kong,
   *iceland, *interlaken, *kyiv, *london, *new_delhi, *new_york, *sant_helena, *rio_de_janeiro,
   *singapore, *sydney, *tehran, *tokyo;

   toronto = gtk_button_new_with_label("Toronto");
   bei_jing = gtk_button_new_with_label("Beijing");
   cairo = gtk_button_new_with_label("Cairo");
   cape_town = gtk_button_new_with_label("Cape Town");
   golden_horseshoe = gtk_button_new_with_label("Golden Horseshoe");
   hamilton = gtk_button_new_with_label("Hamilton");
   hong_kong = gtk_button_new_with_label("Hong Kong");
   iceland = gtk_button_new_with_label("Iceland");
   interlaken = gtk_button_new_with_label("Interlaken");
   kyiv = gtk_button_new_with_label("Kyiv");
   london = gtk_button_new_with_label("London");
   new_delhi = gtk_button_new_with_label("New Delhi");
   new_york = gtk_button_new_with_label("New York");
   sant_helena = gtk_button_new_with_label("Saint Helena");
   rio_de_janeiro = gtk_button_new_with_label("Rio De Janeiro");
   singapore = gtk_button_new_with_label("Singapore");
   sydney = gtk_button_new_with_label("Sydney");
   tehran = gtk_button_new_with_label("Tehran");
   tokyo = gtk_button_new_with_label("Tokyo");
   
   
   GtkWidget* grid = gtk_grid_new();
   gtk_grid_attach((GtkGrid*)grid, toronto, 0,0,1,1);
   gtk_grid_attach((GtkGrid*)grid, bei_jing, 0,1,1,1);
   gtk_grid_attach((GtkGrid*)grid, cairo, 0,2,1,1);
   gtk_grid_attach((GtkGrid*)grid, cape_town, 0,3,1,1);
   gtk_grid_attach((GtkGrid*)grid, golden_horseshoe, 0,4,1,1);
   gtk_grid_attach((GtkGrid*)grid, hamilton, 0,5,1,1);
   gtk_grid_attach((GtkGrid*)grid, hong_kong, 0,6,1,1);

   gtk_grid_attach((GtkGrid*)grid, iceland, 1,0,1,1);
   gtk_grid_attach((GtkGrid*)grid, interlaken, 1,1,1,1);
   gtk_grid_attach((GtkGrid*)grid, kyiv, 1,2,1,1);
   gtk_grid_attach((GtkGrid*)grid, london, 1,3,1,1);
   gtk_grid_attach((GtkGrid*)grid, new_delhi, 1,4,1,1);
   gtk_grid_attach((GtkGrid*)grid, new_york, 1,5,1,1);
   gtk_grid_attach((GtkGrid*)grid, sant_helena, 1,6,1,1);

   gtk_grid_attach((GtkGrid*)grid, rio_de_janeiro, 2,0,1,1);
   gtk_grid_attach((GtkGrid*)grid, singapore, 2,1,1,1);
   gtk_grid_attach((GtkGrid*)grid, sydney, 2,2,1,1);
   gtk_grid_attach((GtkGrid*)grid, tehran, 2,3,1,1);
   gtk_grid_attach((GtkGrid*)grid, tokyo, 2,4,1,1);
   
   gtk_container_add (GTK_CONTAINER (map_window), grid);
   
   gtk_widget_show_all (map_window); 
   g_signal_connect(toronto, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(bei_jing, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(cairo, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(cape_town, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(golden_horseshoe, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(hamilton, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(hong_kong, "clicked", G_CALLBACK(switchMap), app);
   
   g_signal_connect(iceland, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(interlaken, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(kyiv, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(london, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(new_delhi, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(new_york, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(sant_helena, "clicked", G_CALLBACK(switchMap), app);
   
   g_signal_connect(rio_de_janeiro, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(singapore, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(sydney, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(tehran, "clicked", G_CALLBACK(switchMap), app);
   g_signal_connect(tokyo, "clicked", G_CALLBACK(switchMap), app);
}

void switchMap(GtkWidget *searchEntry, ezgl::application *application){
   std::string map_name = gtk_button_get_label((GtkButton*)searchEntry);
   std::string map_path;
   if(map_name == "Toronto") map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";
   else if(map_name == "Beijing") map_path = "/cad2/ece297s/public/maps/beijing_china.streets.bin";
   else if(map_name == "Cairo") map_path = "/cad2/ece297s/public/maps/cairo_egypt.streets.bin";
   else if(map_name == "Cape Town") map_path = "/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin";
   else if(map_name == "Golden Horseshoe") map_path = "/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin";
   else if(map_name == "Hamilton") map_path = "/cad2/ece297s/public/maps/hamilton_canada.streets.bin";
   else if(map_name == "Hong Kong") map_path = "/cad2/ece297s/public/maps/hong-kong_china.streets.bin";
   else if(map_name == "Iceland") map_path = "/cad2/ece297s/public/maps/iceland.streets.bin";
   else if(map_name == "Interlaken") map_path = "/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin";
   else if(map_name == "Kyiv") map_path = "/cad2/ece297s/public/maps/kyiv_ukraine.streets.bin";
   else if(map_name == "London") map_path = "/cad2/ece297s/public/maps/london_england.streets.bin";
   else if(map_name == "New Delhi") map_path = "/cad2/ece297s/public/maps/new-delhi_india.streets.bin";
   else if(map_name == "New York") map_path = "/cad2/ece297s/public/maps/new-york_usa.streets.bin";
   else if(map_name == "Saint Helena") map_path = "/cad2/ece297s/public/maps/saint-helena.streets.bin";
   else if(map_name == "Rio De Janeiro") map_path = "/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin";
   else if(map_name == "Singapore") map_path = "/cad2/ece297s/public/maps/singapore.streets.bin";
   else if(map_name == "Sydney") map_path = "/cad2/ece297s/public/maps/sydney_australia.streets.bin";
   else if(map_name == "Tehran") map_path = "/cad2/ece297s/public/maps/tehran_iran.streets.bin";
   else if(map_name == "Tokyo") map_path = "/cad2/ece297s/public/maps/tokyo_japan.streets.bin";
   
   closeMap();
   
   ezgl::canvas* cnv = application->get_canvas(application->get_main_canvas_id());
   auto start = std::chrono::system_clock::now();
   loadMap(map_path); 
   auto end = std::chrono::system_clock::now();
   std::chrono::duration<double> diff = end - start;
   std::cout << "Time used to open map: " << diff.count() << "seconds. \n";
   
   ezgl::rectangle new_world({get_x_lon(boundary.min_lon), get_y_lat(boundary.min_lat)},
                             {get_x_lon(boundary.max_lon), get_y_lat(boundary.max_lat)});
   application->change_canvas_world_coordinates("MainCanvas", new_world);
   
   cnv->redraw();
   application->refresh_drawing();
}

void draw_main_ui(ezgl::renderer *g){
   g->set_coordinate_system(ezgl::SCREEN);
    
   g->set_horiz_justification(static_cast<ezgl::justification>(1));
   g->set_vert_justification(static_cast<ezgl::justification>(3));
   g->set_text_rotation(0);

   
   if (ui_spec.esc_page_activated){
      draw_esc_page(g); 
      mouse_highlight(g);
      return;
   }

   if (ui_spec.setting_activated){
      draw_settings_page(g);
      mouse_highlight(g);
      return;
   }

   if (ui_spec.help_activated){
      draw_help_page(g);
      mouse_highlight(g);
      return;
   }

   draw_search_bar(g);
   if (ui_spec.search1_activated){
      draw_dropdown(ui_spec.search_entry1, ui_spec.search_bar1, g);
   }else if (ui_spec.search2_activated){
      draw_dropdown(ui_spec.search_entry2, ui_spec.search_bar2, g);
   }

   if (multi_intersections.size() > 1){
      draw_directions_ui(g);
   }
   

   g->set_coordinate_system(ezgl::WORLD);
    
   g->set_horiz_justification(static_cast<ezgl::justification>(0));
   g->set_vert_justification(static_cast<ezgl::justification>(0));
}

void draw_search_bar(ezgl::renderer *g){
    g->set_color(ezgl::WHITE);
    g->fill_rectangle(ui_spec.search_bar1);
    g->fill_rectangle(ui_spec.search_bar2);

   auto search = ezgl::renderer::load_png("libstreetmap/resources/search.png");

    g->draw_surface(search,{248,45});

    g->set_font_size(17);

    if (!ui_spec.search_entry1.empty()){
        g->set_color(15, 28, 35, 255); //input text
        g->draw_text({15, 20}, ui_spec.search_entry1 + " ⬅", 332, 30);
    } else {
        g->set_color(180, 180, 180, 100);
        g->draw_text({15, 20}, "Search Here First Street");
    }

    if (!ui_spec.search_entry2.empty()){
       g->set_color(15, 28, 35, 255); //input text
       g->draw_text({15, 55}, ui_spec.search_entry2 + " ⬅", 332, 30);
    } else {
       g->set_color(180, 180, 180, 100);
       g->draw_text({15,55}, "Search Here Second Street");
    }

    if (ui_spec.search1_activated){
        g->set_line_width(1);
        g->set_color(97, 210, 207, 255);//activate search border
        g->draw_rectangle(ui_spec.search_bar1);
    } else if (ui_spec.search2_activated){
       g->set_line_width(1);
       g->set_color(97, 210, 207, 255);//activate search border
       g->draw_rectangle(ui_spec.search_bar2);
    }
   ezgl::renderer::free_surface(search);
}



void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y){
   //GdkEventScroll *scroll_event = (GdkEventScroll *)event;
   //ezgl::point2d scroll_point(scroll_event->x, scroll_event->y);
   std::string main_canvas_id = app->get_main_canvas_id();
   auto mapCamera = app->get_canvas(main_canvas_id)->get_camera();
   //auto canvas = app->get_canvas(main_canvas_id);
   ezgl::point2d screenCoordinate = mapCamera.world_to_screen({x, y});
   std::stringstream ss;
   
   switch (event->button){
      case 1: // left mouse pressed
         ss << "Clicked at " << screenCoordinate.x << ", " << screenCoordinate.y;
            app->update_message(ss.str());

         if (ui_spec.esc_page_activated){
            if (ui_spec.esc_load_map_button.contains(screenCoordinate.x, screenCoordinate.y)){
               ui_spec.esc_page_activated = false;
               load_map_window(app);
            } else if (ui_spec.esc_settings_button.contains(screenCoordinate.x, screenCoordinate.y)){
               ui_spec.esc_page_activated = false;
               ui_spec.setting_activated = true;
            } else if (ui_spec.esc_help_button.contains(screenCoordinate.x, screenCoordinate.y)){
               ui_spec.esc_page_activated = false;
               ui_spec.help_activated = true;
            }
            app->refresh_drawing();
            return;
         }

         if (ui_spec.setting_activated){
            if (ui_spec.night_mode_bar.contains(screenCoordinate.x, screenCoordinate.y)){
               turn_on_night_mode(nullptr, app);
            }
            app->refresh_drawing();
            return;
         }

         if (ui_spec.search1_activated && !ui_spec.drop_down_boxes.empty()){
            for (int i = 0; i < ui_spec.drop_down_boxes.size(); i++){
               if (ui_spec.drop_down_boxes[i].contains(screenCoordinate.x, screenCoordinate.y)){
                  ui_spec.search_entry1 = getStreetName(ui_spec.drop_down_search_result[i]);
                  ui_spec.search1_activated = false;
                  app->refresh_drawing();
                  return;
               }
            }
         } else if (ui_spec.search2_activated && !ui_spec.drop_down_boxes.empty()){
            for (int i = 0; i < ui_spec.drop_down_boxes.size(); i++){
               if (ui_spec.drop_down_boxes[i].contains(screenCoordinate.x, screenCoordinate.y)){
                  ui_spec.search_entry2 = getStreetName(ui_spec.drop_down_search_result[i]);
                  ui_spec.search2_activated = false;
                  app->refresh_drawing();
                  return;
               }
            }
         }

         if (ui_spec.search_button.contains(screenCoordinate.x, screenCoordinate.y)){
            ui_spec.search1_activated = false;
            ui_spec.search2_activated = false;
            assert(!ui_spec.search_entry1.empty());
            assert(!ui_spec.search_entry2.empty());
            ss << "search activated";
            app->update_message(ss.str());
            searchIntersection(ui_spec.search_entry1, ui_spec.search_entry2, app);
         } else if (ui_spec.search_bar1.contains(screenCoordinate.x,screenCoordinate.y)){
            ui_spec.search1_activated = true;
            ui_spec.search2_activated = false;
            draw_main_ui(app->get_renderer());
            app->refresh_drawing();
         } else if (ui_spec.search_bar2.contains(screenCoordinate.x, screenCoordinate.y)){
            ui_spec.search1_activated = false;
            ui_spec.search2_activated = true;
            draw_main_ui(app->get_renderer());
            app->refresh_drawing();
         } else if (ui_spec.search1_activated || ui_spec.search2_activated){
            ui_spec.search1_activated = false;
            ui_spec.search2_activated = false;
            draw_main_ui(app->get_renderer());
            app->refresh_drawing();
            search_closest_intersection_on_click(app, x, y);
         } else {
            search_closest_intersection_on_click(app, x, y);
            draw_main_ui(app->get_renderer());
            app->refresh_drawing();
         }
      case 2:
        
         
      //wheel button to clear all highlighted
      case 3:
         // reset_highlight();
         // ss << "";
         // app->update_message(ss.str());
         // multi_intersections.clear();
         // draw_paths(app->get_renderer());
         // app->refresh_drawing();
         // break;

      default:
         break;
   }
      
}


void act_on_mouse_move(ezgl::application *app, GdkEventButton *event, double x, double y){
   std::string main_canvas_id = app->get_main_canvas_id();
   auto mapCamera = app->get_canvas(main_canvas_id)->get_camera();
   //auto canvas = app->get_canvas(main_canvas_id);
   ezgl::point2d screenCoordinate = mapCamera.world_to_screen({x, y});


   if (ui_spec.esc_page_activated || ui_spec.help_activated || ui_spec.setting_activated){
      ui_spec.mouse_highlight_location = {screenCoordinate.x,screenCoordinate.y};
   }

   if (ui_spec.esc_page_activated){
      if (ui_spec.esc_load_map_button.contains({screenCoordinate.x,screenCoordinate.y})){
         ui_spec.esc_load_map_highlight = true;
      } else if (ui_spec.esc_settings_button.contains({screenCoordinate.x,screenCoordinate.y})){
         ui_spec.esc_setting_highlight = true;
      } else if (ui_spec.esc_help_button.contains({screenCoordinate.x,screenCoordinate.y})){
         ui_spec.esc_help_highlight = true;
      }else{
         ui_spec.esc_load_map_highlight = false;
         ui_spec.esc_help_highlight = false;
         ui_spec.esc_setting_highlight = false;
      }
   }
   app->refresh_drawing();
}

void mouse_highlight(ezgl::renderer *g){

   for (double i = 0; i < 3; i++){
      g->set_color(255,255,255,15);
      g->fill_arc(ui_spec.mouse_highlight_location, 40*i, 0, 360);

   }
}

void act_on_key_press(ezgl::application *app, GdkEventKey* /*event*/, char* key){
   std::string canvas_id= app->get_main_canvas_id();
   ezgl::canvas *main_canvas = app->get_canvas(canvas_id);
   assert(main_canvas != nullptr);

   std::stringstream ss;

   ezgl::rectangle world({get_x_lon(boundary.min_lon), get_y_lat(boundary.min_lat)}, 
                            {get_x_lon(boundary.max_lon), get_y_lat(boundary.max_lat)});

   auto key_val = gdk_keyval_from_name(key);
   auto unicode = gdk_keyval_to_unicode(key_val);
   
   if (ui_spec.search1_activated){
      if (unicode == 9){
         return;
      } else if (unicode == 13){
         if (ui_spec.search_entry1.empty()||ui_spec.search_entry2.empty()){
            ss << "At least one search bar is empty.";
            app->update_message(ss.str());
            return;
         }else{
            searchIntersection(ui_spec.search_entry1, ui_spec.search_entry2, app);
            return;
         }
      } else if (unicode == 8){
         if (ui_spec.search_entry1.empty()){
            return;
         } else {
            ui_spec.search_entry1.erase(ui_spec.search_entry1.end()-1);
         }
      } else if (unicode == 32){
         if (ui_spec.search_entry1.empty()){
            return;
         }else{
            ui_spec.search_entry1 = ui_spec.search_entry1 + (char) unicode;
         }
      } else if (unicode == 27){
         ui_spec.search1_activated = false;
      } else {
         ui_spec.search_entry1 = ui_spec.search_entry1 + *key;
      }
      app->refresh_drawing();
      return;
   } else if (ui_spec.search2_activated){
      if (unicode == 9){
         return;
      } else if (unicode == 13){
         if (ui_spec.search_entry1.empty()||ui_spec.search_entry2.empty()){
            ss << "At least one search bar is empty.";
            app->update_message(ss.str());
            return;
         }else{
            searchIntersection(ui_spec.search_entry1, ui_spec.search_entry2, app);
            return;
         }
      } else if (unicode == 8){
         if (ui_spec.search_entry2.empty()){
            return;
         } else {
            ui_spec.search_entry2.erase(ui_spec.search_entry2.end()-1);
         }
      } else if (unicode == 32){
         if (ui_spec.search_entry2.empty()){
            return;
         }else{
            ui_spec.search_entry2 = ui_spec.search_entry2 + (char) unicode;
         }
      } else if (unicode == 27){
         ui_spec.search2_activated = false;

      } else {
         ui_spec.search_entry2 = ui_spec.search_entry2 + *key;
      }
      app->refresh_drawing();
      return;
   }
   
   if (!multi_intersections.empty()){
      if (unicode == 27){
         reset_highlight();
         ss << "";
         app->update_message(ss.str());
         multi_intersections.clear();
         draw_paths(app->get_renderer());
         app->refresh_drawing();
      } else if (*key == 'w'){
         if ((!ui_spec.direction_sentence.empty()) && ui_spec.direction_start_index > 0){
            ui_spec.direction_start_index --;
         }
      } else if (*key == 's'){
         if (ui_spec.direction_start_index < ui_spec.direction_sentence.size() - 6){
            ui_spec.direction_start_index ++;
         }
      }
      app->refresh_drawing();
      return;
   } 
   if (*key == 'h'){
      ui_spec.setting_activated = false;
      ui_spec.esc_page_activated = false;
      ui_spec.help_activated = true;

      app->refresh_drawing();
      return;

   } else if (ui_spec.setting_activated){
      if (unicode == 27){
         ui_spec.setting_activated = false;
         ui_spec.esc_page_activated = true;
      }
      app->refresh_drawing();
      return;
   } else if (ui_spec.help_activated){
      if (unicode == 27){
         ui_spec.help_activated = false;
         ui_spec.esc_page_activated = true;
      } else if (*key == 'a'){
         if (ui_spec.help_page > 1){
            ui_spec.help_page --;
         }
      } else if (*key == 'd'){
         if (ui_spec.help_page < 6){
            ui_spec.help_page ++;
         }
      }
      app->refresh_drawing();
      return;
   } else {
      if (*key == 'w'){
         translate_up(main_canvas, 5);
      } else if (*key == 's'){
         translate_down(main_canvas, 5);
      } else if (*key == 'a'){
         translate_left(main_canvas, 5);
      } else if (*key == 'd'){
         translate_right(main_canvas, 5);
      } else if (*key == 'f'){
         ezgl::zoom_fit(main_canvas, world);
      } else if (unicode == 27){
         ui_spec.esc_page_activated = !ui_spec.esc_page_activated;
         app->refresh_drawing();
      }
   }
   
}

void draw_esc_background(ezgl::renderer *g){

   auto screen = g->get_visible_screen();
   //draw background
   //draw green
   g->set_color(171, 200, 166, 200);
   g->fill_rectangle(screen);

   int opacity = 255;
   //top blue gradient 
   for (double j = screen.bottom(); j < screen.top(); ++j){
      g->set_color(167, 225, 249, opacity);
      g->fill_rectangle({screen.left(), j+1},{screen.right(),j});
      opacity -= 20/(screen.top());
   }
   //bottom yellow gradient
   opacity = 255;
   for (double j = screen.top(); j > screen.bottom(); --j){
      g->set_color(224, 194, 55, opacity);
      g->fill_rectangle({screen.left(),j},{screen.right(), j-1});
      opacity -= 20/(screen.top());
   }
}

void draw_esc_page(ezgl::renderer *g){

   auto screen = g->get_visible_screen();
   
   draw_esc_background(g);
   /*
   if(!night_mode) g->set_color(167, 225, 249, 180);
   else g->set_color(31, 53, 44, 180);
   g->fill_rectangle(screen);
   */

   //draw buttons
   ui_spec.esc_load_map_button = {{screen.right()*1/10, screen.top()/2+50}, {(screen.right()*3/10), screen.top()/2-50}};
   ui_spec.esc_settings_button = {{screen.right()*4/10, screen.top()/2+50}, {(screen.right()*6/10), screen.top()/2-50}};
   ui_spec.esc_help_button = {{screen.right()*7/10, screen.top()/2+50}, {(screen.right()*9/10), screen.top()/2-50}};

   if(!night_mode)g->set_color(64, 117, 86, 255);
   else g->set_color(115, 221, 196, 255);

   g->fill_rectangle(ui_spec.esc_load_map_button);
   g->fill_rectangle(ui_spec.esc_settings_button);
   g->fill_rectangle(ui_spec.esc_help_button);

   g->set_color(46, 78, 61, 255);
   if (ui_spec.esc_load_map_highlight){
      g->fill_rectangle(ui_spec.esc_load_map_button);
   }else if (ui_spec.esc_setting_highlight){
      g->fill_rectangle(ui_spec.esc_settings_button);
   }else if (ui_spec.esc_help_highlight){
      g->fill_rectangle(ui_spec.esc_help_button);
   }

   //draw button lines
   //thin outer lines of button
   g->set_color(205, 239, 110, 255);
   g->set_line_width(2);
   g->draw_rectangle({screen.right()*1/10-5, screen.top()/2+55}, {(screen.right()*3/10+5), screen.top()/2-55});
   g->draw_rectangle({screen.right()*4/10-5, screen.top()/2+55}, {(screen.right()*6/10+5), screen.top()/2-55});
   g->draw_rectangle({screen.right()*7/10-5, screen.top()/2+55}, {(screen.right()*9/10+5), screen.top()/2-55});

   //draw thicker partial lines around buttons
   g->set_line_width(16);
   g->draw_line({ui_spec.esc_load_map_button.left()-20, screen.top()/2-70}, {screen.right()*2/10-30, screen.top()/2-70});
   g->draw_line({ui_spec.esc_settings_button.left()-20, screen.top()/2-70}, {screen.right()*5/10-30, screen.top()/2-70});
   g->draw_line({ui_spec.esc_help_button.left()-20, screen.top()/2-70}, {screen.right()*8/10-30, screen.top()/2-70});

   g->set_line_width(17);
   g->draw_line({screen.right()*2/10, screen.top()/2+70}, {screen.right()*3/10+20, screen.top()/2+70});
   g->draw_line({screen.right()*5/10, screen.top()/2+70}, {screen.right()*6/10+20, screen.top()/2+70});
   g->draw_line({screen.right()*8/10, screen.top()/2+70}, {screen.right()*9/10+20, screen.top()/2+70});

   //draw small pink dashes
   g->set_color(255, 24, 76, 255);
   g->set_line_width(2);

   //bottom left dash
   g->draw_line({ui_spec.esc_load_map_button.left()-15, screen.top()/2+40}, {ui_spec.esc_load_map_button.left()+10, screen.top()/2+40});
   g->draw_line({ui_spec.esc_settings_button.left()-15, screen.top()/2+40}, {ui_spec.esc_settings_button.left()+10, screen.top()/2+40});
   g->draw_line({ui_spec.esc_help_button.left()-15, screen.top()/2+40}, {ui_spec.esc_help_button.left()+10, screen.top()/2+40});

   //top left dash
   g->draw_line({screen.right()*2/10-40, screen.top()/2-65}, {screen.right()*2/10-25, screen.top()/2-65});
   g->draw_line({screen.right()*5/10-40, screen.top()/2-65}, {screen.right()*5/10-25, screen.top()/2-65});
   g->draw_line({screen.right()*8/10-40, screen.top()/2-65}, {screen.right()*8/10-25, screen.top()/2-65});

   //print text bottom blue level and identify location
   g->set_font_size(25);
   g->set_color(141, 246, 246, 255);
   double text_x = ui_spec.esc_load_map_button.left() + ((ui_spec.esc_load_map_button.right()-ui_spec.esc_load_map_button.left())/2 - 54);
   double text_y = ui_spec.esc_load_map_button.bottom() + ((ui_spec.esc_load_map_button.top()-ui_spec.esc_load_map_button.bottom())/2 - 9);
   g->draw_text({text_x, text_y}, "Load Map");
   ui_spec.load_map_text_location = {text_x-1, text_y-1};

   text_x = ui_spec.esc_settings_button.left() + ((ui_spec.esc_settings_button.right()-ui_spec.esc_settings_button.left())/2 - 49);
   text_y = ui_spec.esc_settings_button.bottom() + ((ui_spec.esc_settings_button.top()-ui_spec.esc_settings_button.bottom())/2 - 9);
   g->draw_text({text_x, text_y}, "Settings");
   ui_spec.settings_text_location = {text_x-1, text_y-1};

   text_x = ui_spec.esc_help_button.left() + ((ui_spec.esc_help_button.right()-ui_spec.esc_help_button.left())/2 - 24);
   text_y = ui_spec.esc_help_button.bottom() + ((ui_spec.esc_help_button.top()-ui_spec.esc_help_button.bottom())/2 - 9);
   g->draw_text({text_x, text_y}, "Help");
   ui_spec.help_text_location = {text_x-1, text_y-1};
   
   
   //print text top level
   g->set_color(205, 239, 110, 255); //input text
   
   g->draw_text(ui_spec.load_map_text_location, "Load Map");
   g->draw_text(ui_spec.settings_text_location, "Settings");
   g->draw_text(ui_spec.help_text_location, "Help");
}

void draw_dropdown(std::string search_input, ezgl::rectangle searchBar, ezgl::renderer *g){

   ui_spec.drop_down_search_result.clear();
   ui_spec.drop_down_boxes.clear();

   std::vector<int> possible_names = findStreetIdsFromPartialStreetName(search_input);
   

   // for(auto id : possible_names) std::cout << getStreetName(id) << " ";
   ui_spec.drop_down_search_result = possible_names;

   if (ui_spec.drop_down_search_result.empty()){
      return;
   }
   int i = 1;
   while(i != ui_spec.drop_down_search_result.size() && i <= 5){
      ezgl::rectangle temp = {{searchBar.left(),searchBar.top()+30*i},{searchBar.right(),searchBar.top()+30*(i-1)}};
      ui_spec.drop_down_boxes.push_back(temp);

      g->set_color(114, 221, 196, 255);
      g->fill_rectangle(temp);

      g->set_color(15, 28, 35, 255);
      g->draw_text({temp.left()+10, temp.bottom()+8}, getStreetName(possible_names[i-1]));
      g->set_color(203, 239, 89, 255);

      g->set_line_width(3);
      g->set_line_dash(ezgl::line_dash::asymmetric_5_3);
      g->draw_line({temp.left(), temp.top()-1}, {temp.right(), temp.top()-1});

      g->set_line_width(4);
      g->set_line_dash(ezgl::line_dash::none);
      g->draw_line({temp.right()-30, temp.top()},{temp.right()-10, temp.bottom()});

      g->set_line_width(7);
      g->set_line_dash(ezgl::line_dash::none);
      g->draw_line({temp.right()-20, temp.top()},{temp.right(), temp.bottom()});

      g->set_line_width(3);
      g->set_color(255, 87, 125, 255);
      g->draw_line({temp.left()-5, temp.bottom()+2}, {temp.left()+15, temp.bottom()+2});

      i++;
   }
}

void draw_directions_ui(ezgl::renderer *g){
   

   if (ui_spec.direction_sentence.empty()){
      return;
   }

   ui_spec.direction_drop_down_boxes.clear();
   ui_spec.next_line = 0;

   int start;
   if (ui_spec.direction_start_index >= 0){
      start = ui_spec.direction_start_index;
   }else{
      start = 0;
   }


   auto south = ezgl::renderer::load_png("libstreetmap/resources/south.png");
   auto north = ezgl::renderer::load_png("libstreetmap/resources/north.png");
   auto west = ezgl::renderer::load_png("libstreetmap/resources/west.png");
   auto east = ezgl::renderer::load_png("libstreetmap/resources/east.png");
   auto straight = ezgl::renderer::load_png("libstreetmap/resources/go_straight.png");
   auto left = ezgl::renderer::load_png("libstreetmap/resources/turn_left.png");
   auto right = ezgl::renderer::load_png("libstreetmap/resources/turn_right.png");

   int i = 1;
   while(start+i-1 < ui_spec.direction_sentence.size() && i <= 7){
      ezgl::rectangle temp;
      std::string temp_string = ui_spec.direction_sentence[i+start-1];
      if (temp_string.length() > 35){
         temp = {{ui_spec.direction_box.left(), ui_spec.direction_box.top() + (i-1)*40 + (ui_spec.next_line+1)*15}, 
                  {ui_spec.direction_box.right(), ui_spec.direction_box.bottom() + (i-1)*40 + ui_spec.next_line*15}};
         ui_spec.next_line ++;
      } else {
         temp = {{ui_spec.direction_box.left(), ui_spec.direction_box.top() + (i-1)*40 + ui_spec.next_line*15}, 
                  {ui_spec.direction_box.right(), ui_spec.direction_box.bottom() + (i-1)*40 + ui_spec.next_line*15}};
      }
      ui_spec.direction_drop_down_boxes.push_back(temp);

      g->set_color(31, 53, 44, 200);
      g->fill_rectangle(temp);
      g->set_color(196, 237, 99, 200);

      if (temp_string.length() > 35){
         if (temp_string.find(' ',25) < 40){
            g->draw_text({temp.left()+10, temp.bottom()+4}, temp_string.substr(0,temp_string.find(' ',25)));
            g->draw_text({temp.left()+10, temp.bottom()+23}, temp_string.substr(temp_string.find(' ',25) + 1, temp_string.length()));
         }else{
            g->draw_text({temp.left()+10, temp.bottom()+4}, temp_string.substr(0,34));
            g->draw_text({temp.left()+10, temp.bottom()+23}, temp_string.substr(35,temp_string.length()));
         }
         
      } else {
         g->draw_text({temp.left()+10, temp.bottom()+8}, temp_string);
      }
      
      
      g->draw_line({temp.left(), temp.top()-1}, {temp.right(), temp.top()-1});

      if (ui_spec.directions[i+start-1] == "start" || ui_spec.directions[i+start-1] == "arrive"){

      } else if (ui_spec.directions[i+start-1] == "south"){
         g->draw_surface(south,{temp.left()-20, temp.bottom()});
      } else if (ui_spec.directions[i+start-1] == "north"){
         g->draw_surface(north,{temp.left()-20, temp.bottom()});
      } else if (ui_spec.directions[i+start-1] == "west"){
         g->draw_surface(west,{temp.left()-20, temp.bottom()});
      } else if (ui_spec.directions[i+start-1] == "east"){
         g->draw_surface(east,{temp.left()-20, temp.bottom()});
      } else if (ui_spec.directions[i+start-1] == "straight"){
         g->draw_surface(straight,{temp.left()-20, temp.bottom()});
      } else if (ui_spec.directions[i+start-1] == "left"){
         g->draw_surface(left,{temp.left()-20, temp.bottom()});
      } else if (ui_spec.directions[i+start-1] == "right"){
         g->draw_surface(right,{temp.left()-20, temp.bottom()});
      } else {

      }
      i++;
   }

   ezgl::renderer::free_surface(south);
   ezgl::renderer::free_surface(north);
   ezgl::renderer::free_surface(west);
   ezgl::renderer::free_surface(east);
   ezgl::renderer::free_surface(straight);
   ezgl::renderer::free_surface(left);
   ezgl::renderer::free_surface(right);


   
   return;
}

void draw_settings_page(ezgl::renderer *g){
   auto screen = g->get_visible_screen();
   //if(!night_mode) g->set_color(167, 225, 249, 200);
   //else g->set_color(31, 53, 44, 200);
   //g->fill_rectangle(screen);

   draw_esc_background(g);

   if(!night_mode) g->set_color(31, 53, 44, 200);
   else g->set_color(167, 225, 249, 200);
   
   ui_spec.night_mode_bar = {{screen.left(), screen.bottom()+100}, {screen.right(), screen.bottom()+70}};
   g->fill_rectangle(ui_spec.night_mode_bar);

   g->set_color(ezgl::BLACK);
   g->fill_rectangle({{screen.right()-110, screen.bottom()+100}, {screen.right()-50, screen.bottom()+70}});
   g->set_font_size(25);
   g->draw_text({screen.left()+30, screen.bottom()+75}, "Dark Mode");
   

   g->set_color(31, 53, 44, 255);
   if (night_mode){
      g->fill_rectangle({{screen.right()-80, screen.bottom()+100}, {screen.right()-50, screen.bottom()+70}});
   }else{
      g->fill_rectangle({{screen.right()-110, screen.bottom()+100}, {screen.right()-80, screen.bottom()+70}});
   }
   
}

void draw_help_page(ezgl::renderer *g){

   auto screen = g->get_visible_screen();
   //if(!night_mode) g->set_color(167, 225, 249, 200);
   //else g->set_color(31, 53, 44, 200);
   //g->fill_rectangle(screen);

   draw_esc_background(g);

   if (ui_spec.help_page > 6 || ui_spec.help_page < 1){
      ui_spec.help_page = 1;
      ui_spec.help_activated = false;
      ui_spec.esc_page_activated = true;
   }

   auto p1 = ezgl::renderer::load_png("libstreetmap/resources/page_1.png");
   auto p2 = ezgl::renderer::load_png("libstreetmap/resources/page_2.png");
   auto p3 = ezgl::renderer::load_png("libstreetmap/resources/page_3.png");
   auto p4 = ezgl::renderer::load_png("libstreetmap/resources/page_4.png");
   auto p5 = ezgl::renderer::load_png("libstreetmap/resources/page_5.png");
   auto p6 = ezgl::renderer::load_png("libstreetmap/resources/page_6.png");

   double x = screen.right()/2-320;
   double y = screen.top()/2-150;

   if (ui_spec.help_page == 1){
      g->draw_surface(p1,{x, y});
   } else if (ui_spec.help_page == 2){
      g->draw_surface(p2,{x, y});
   } else if (ui_spec.help_page == 3){
      g->draw_surface(p3,{x, y});
   } else if (ui_spec.help_page == 4){
      g->draw_surface(p4,{x, y});
   } else if (ui_spec.help_page == 5){
      g->draw_surface(p5,{x, y});
   } else if (ui_spec.help_page == 6){
      g->draw_surface(p6,{x, y});
   } else {
   }

   ezgl::renderer::free_surface(p1);
   ezgl::renderer::free_surface(p2);
   ezgl::renderer::free_surface(p3);
   ezgl::renderer::free_surface(p4);
   ezgl::renderer::free_surface(p5);
   ezgl::renderer::free_surface(p6);
}