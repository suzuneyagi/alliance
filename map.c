#include "map.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

// Load a map from a file. Returns false if any error occurs during loading
bool load_map(const char* filename, map_t* map) {
  // Set the map up for loading
  map->data = NULL;
  map->width = 0;
  map->height = 0;

  // Open the map file
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Failed to open map file");
    return false;
  }

  // Read in the map file
  char* line = NULL;
  size_t linecap = 0;
  ssize_t length;
  while ((length = getline(&line, &linecap, fp)) > 0) {
    // If this is the first line, the map width will be zero. Calculate the real width
    if (map->width == 0) {
      // The length of the line is the map width plus one for the newline character
      map->width = length - 1;

    } else if (length < map->width) {
      // The line is too short to be a map row, so leave the reading loop. This might be an error, or could just be the last line
      break;
    }

    // Add space for a row of the map
    map->height++;
    map->data = realloc(map->data, sizeof(cell_t) * map->width * map->height);

    // Fill in the new row of the map
    int start_index = map->width * (map->height - 1);
    for (int i=0; i<map->width; i++) {
      if (line[i] == '~') {
        map->data[start_index + i] = CELL_BG;
      } else if (line[i] == 'W') {
        map->data[start_index + i] = CELL_WALL;
      } else if (line[i] == 'R') {
        map->data[start_index + i] = CELL_RED;
      } else if (line[i] == 'B') {
        map->data[start_index + i] = CELL_BLUE;
      } else if (line[i] == 'O') {
        map->data[start_index + i] = CELL_OBJECT;
      } else if (line[i] == 'Y') {
        map->data[start_index + i] = CELL_YELLOW;
      } else if (line[i] == 'C') {
        map->data[start_index + i] = CELL_CYAN;
      } else if (line[i] == 'T') {
        map->data[start_index + i] = CELL_BUTTON_B;
      } else if (line[i] == 'G') {
        map->data[start_index + i] = CELL_BUTTON_R;
      } else if (line[i] == 'E') {
        map->data[start_index + i] = CELL_YELLOW1;
      } else if (line[i] == 'M') {
        map->data[start_index + i] = CELL_MAGENTA_WALL;
      } else if (line[i] == 'F') {
        map->data[start_index + i] = CELL_MAGENTA_FINISH;
      } else if (line[i] == 'A') {
        map->data[start_index + i] = CELL_MAGENTA;
      } else if (line[i] == 'L') {
        map->data[start_index + i] = CELL_YELLOW_WALL;
      } else {
        fprintf(stderr, "Invalid character %c in map file\n", line[i]);
        free(line);
        fclose(fp);
        return false;
      }
    }
  }
  free(line);
  fclose(fp);
  
  return true;
}

// Free all the memory used to store a map
void free_map(map_t* map) {
  free(map->data);
  map->data = NULL;
}


// Display the map and player
void display_map(map_t map, coord_t player_R, coord_t player_B, coord_t object1, coord_t object2) {
  // Get screen size
  coord_t screen_size;
  screen_size.x = getmaxx(stdscr);
  screen_size.y = getmaxy(stdscr);


  // Display the map
  for (int screen_y = 0; screen_y < screen_size.y-1; screen_y++) {
    for (int screen_x = 0; screen_x < screen_size.x-1; screen_x++) {
      int map_x = screen_x / 2;
      int map_y = screen_y;
      
      int color = CELL_BLANK;
      if (map_x >= 0 && map_x < map.width && map_y >= 0 && map_y < map.height) {
        color = map.data[map_x + map_y * map.width];
      }
      attron(COLOR_PAIR(color));
      mvaddch(screen_y, screen_x, ' ');
      attroff(COLOR_PAIR(color));
    }
  }

  // Draw player red
  attron(COLOR_PAIR(CELL_RED_TEXT));
  mvaddch(player_R.y, player_R.x*2, '@');
  attroff(COLOR_PAIR(CELL_RED_TEXT));

  // Draw player blue
  attron(COLOR_PAIR(CELL_BLUE_TEXT));
  mvaddch(player_B.y, player_B.x*2, '@');
  attroff(COLOR_PAIR(CELL_BLUE_TEXT));

  //Draw object1
  attron(COLOR_PAIR(CELL_OBJECT));
  mvaddch(object1.y, object1.x*2, ' ');
  addch(' ');
  attroff(COLOR_PAIR(CELL_OBJECT));

  //Draw object2
  attron(COLOR_PAIR(CELL_OBJECT));
  mvaddch(object2.y, object2.x*2, ' ');
  addch(' ');
  attroff(COLOR_PAIR(CELL_OBJECT));

}
