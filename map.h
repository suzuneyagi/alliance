#pragma once

#include <stdbool.h>
#include <stdlib.h>

// Use an enum to keep track of map tiles
typedef enum cell {
  CELL_BLANK, // terminal background
  CELL_BG, // game background
  CELL_WALL, // Game wall
  CELL_RED_TEXT, // Player red (@)
  CELL_BLUE_TEXT, // Player blue (@)
  CELL_RED, // Red tiles
  CELL_BLUE, // Blue tiles
  CELL_YELLOW, // Yellow button 1
  CELL_OBJECT, // Moving object 
  CELL_CYAN, // Cyan button
  CELL_BUTTON_B, // Blue button
  CELL_BUTTON_R, // Red button
  CELL_YELLOW1, // Yellow button 2
  CELL_MAGENTA, // Magenta button
  CELL_MAGENTA_WALL, // Magenta wall
  CELL_YELLOW_WALL, // Yellow wall
  CELL_MAGENTA_FINISH // The goal tile
} cell_t;

// A coordinate is just an x and y value
typedef struct coord {
  float x;
  float y;
} coord_t;

// A map is a data array with width, height, and starting location information
typedef struct map {
  cell_t* data;
  size_t width;
  size_t height;
} map_t;

// Load a map from a file. Returns false if any error occurs during loading
bool load_map(const char* filename, map_t* map);

// Free all the memory used to store a map
void free_map(map_t* map);

// Display the map and player
void display_map(map_t map, coord_t player_R, coord_t player_B, coord_t object1, coord_t object2);