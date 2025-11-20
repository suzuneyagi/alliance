#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "map.h"
#include "util.h"

#define FRAME_RATE 60
#define WIDTH 58
#define YELLOW1 315
#define CYAN_DOOR1 583
#define CYAN_DOOR2 631
#define RED 912
#define BLUE 884
#define YELLOW2 965
#define MAGENTA 943


int main(int argc, char** argv) {
  // Check for an argument
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <map file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Load a map
  map_t map;
  if (!load_map(argv[1], &map)) {
    fprintf(stderr, "Failed to load map %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  // Accept mouse clicks
  mousemask(BUTTON1_CLICKED, NULL);

  // Set up ncurses
  initscr();              // Initialize the standard window
  noecho();               // Don't display keys when they are typed
  nodelay(stdscr, true);  // Set up non-blocking input with getch()
  keypad(stdscr, true);   // Allow arrow keys
  curs_set(false);        // Hide the cursor

  // Set up colors for each cell type in the map
  start_color();
  init_pair(CELL_BLANK, COLOR_BLACK, COLOR_BLACK);
  init_pair(CELL_BG, COLOR_WHITE, COLOR_WHITE);
  init_pair(CELL_WALL, COLOR_GREEN, COLOR_GREEN);

  init_pair(CELL_RED, COLOR_RED, COLOR_RED);
  init_pair(CELL_RED_TEXT, COLOR_RED, COLOR_WHITE);

  init_pair(CELL_BLUE, COLOR_BLUE, COLOR_BLUE);
  init_pair(CELL_BLUE_TEXT, COLOR_BLUE, COLOR_WHITE);

  init_pair(CELL_OBJECT, COLOR_BLACK, COLOR_BLACK);

  init_pair(CELL_YELLOW, COLOR_YELLOW, COLOR_YELLOW);

  init_pair(CELL_CYAN, COLOR_CYAN, COLOR_CYAN);

  init_pair(CELL_BUTTON_B, COLOR_BLUE, COLOR_BLUE);
  init_pair(CELL_BUTTON_R, COLOR_RED, COLOR_RED);

  init_pair(CELL_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(CELL_MAGENTA_WALL, COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(CELL_MAGENTA_FINISH, COLOR_MAGENTA, COLOR_MAGENTA);

  init_pair(CELL_YELLOW1, COLOR_YELLOW, COLOR_YELLOW);
  init_pair(CELL_YELLOW_WALL, COLOR_YELLOW, COLOR_YELLOW);

  // Keep track of the player position (player red and player blue)

  coord_t player_R = {2,1};
  coord_t player_B = {56,1};

  coord_t object1 = {44, 9};
  coord_t object2 = {29, 9};

  // Start the game loop
  bool running = true;
  int index_R = 0;
  int index_B = 0;

  int index_R_current = 0;
  int index_B_current = 0;

  int loop_counter = 0;
  int dir = 0;

  float velocity_x_R = 0;
  float velocity_x_B = 0;

  float velocity_y_R = 0;
  float velocity_y_B = 0;

  bool key_up_R = false;
  bool key_right_R = false;
  bool key_left_R = false;

  bool key_up_B = false;
  bool key_right_B = false;
  bool key_left_B = false;

  int left_delay_R = 0;
  int right_delay_R = 0;

  int left_delay_B = 0;
  int right_delay_B = 0;

// Game start message
  attron(COLOR_PAIR(0));   
  mvprintw(8, 34, "                                                                                       ");
  mvprintw(9, 34, "          You two, Blue @ and Red @, have been separated by an evil Black @.           ");
  mvprintw(10, 34, "                Collaborate with each other to reunite once again.                     ");
  mvprintw(11, 34, "                                                                                       ");
  mvprintw(12, 34, "     Remember, Blue @ cannot walk on red tiles, and Red @ cannot walk on blue tiles.   ");
  mvprintw(13, 34, "                         Respect each other's private space.                           ");
  mvprintw(14, 34, "                                                                                       ");
  mvprintw(15, 34, "                    Avoid the enemies and use the buttons wisely.                      ");
  mvprintw(16, 34, "                             May your friendship prevail.                              ");
  mvprintw(17, 34, "                                                                                       ");
  mvprintw(18, 34, "                                                                                       ");
  mvprintw(19, 34, "                             Press space to start the game.                            ");
  mvprintw(20, 34, "                                                                                       ");
  attroff(COLOR_PAIR(0));

  // Start when pressed space.
  while (getch() != ' ') {};

  while (running) {

    // Index of current position
    index_R_current = map.width * ((int)player_R.y) + (int)player_R.x;
    index_B_current = map.width * ((int)player_B.y) + (int)player_B.x;

    // Index of the cell right under the current position
    index_R = index_R_current + map.width;
    index_B = index_B_current + map.width;

    // Level 1: If the yellow button is pressed, open the first door.
    if (map.data[index_B_current] == CELL_YELLOW) {
      for (int i=YELLOW1; i< YELLOW1+5; i++) {
        map.data[i] = CELL_BG;
      }
    } else {
      // If the button is not pressed, close the door.
      for (int i=YELLOW1; i< YELLOW1+5; i++) {
        map.data[i] = CELL_WALL;
      }
    }

    // Level 2: if the blue buttons are pressed at the same time, then drop the players down.
    if ((map.data[index_R_current] == CELL_CYAN) && (map.data[index_B_current] == CELL_CYAN)) {
      for (int i = CYAN_DOOR1; i < CYAN_DOOR1 + 4; i++){
        map.data[i] = CELL_BG;
      }
      for (int i = CYAN_DOOR2; i < CYAN_DOOR2 + 4; i++){
        map.data[i] = CELL_BG;
      }
    } else { 
        // If the buttons are not pressed, close the doors.
        for (int i = CYAN_DOOR1; i < CYAN_DOOR1 + 4; i++){
        map.data[i] = CELL_WALL;
      }
       for (int i = CYAN_DOOR2; i < CYAN_DOOR2 + 4; i++){
        map.data[i] = CELL_WALL;
      }
    }

    // Constantly moving objects

    // Move object 1 constantly
    if (object1.x == 44) {
      dir = -1;
    } else if (object1.x == 34) {
      dir = 1;
    }

    // Set the spped slower
    if (loop_counter % 6 == 0)  {
      object1.x = object1.x + dir;
    }

    // Move object 2 constantly
    if (object2.x == 32) {
      dir = -1;
    } else if (object2.x == 22) {
      dir = 1;
    }

    // Set the speed slower
    if (loop_counter % 6 == 0)  {
      object2.x = object2.x + dir;
    }

    // Check for collusions between the objects and the players

    if ((((int)(player_R.x) == object1.x) && ((int)(player_R.y) == object1.y)) || (((int)(player_R.x) == object2.x) && ((int)(player_R.y) == object2.y))) {
      attron(COLOR_PAIR(0));
      mvprintw(11, 34, "                                           ");
      mvprintw(12, 34, "        RED collided with an enemy!        ");
      mvprintw(13, 34, "                                           ");
      mvprintw(14, 34, "        Press space to play again.         ");
      mvprintw(15, 34, "                                           ");
      attroff(COLOR_PAIR(0));
      while (getch() != ' ') {};
      player_R.x = 2; 
      player_R.y = 1;
      player_B.x = 56; 
      player_B.y = 1;
    }

    if ((((int)(player_B.x) == object1.x) && ((int)(player_B.y) == object1.y)) || (((int)(player_B.x) == object2.x) && ((int)(player_B.y) == object2.y))){
      attron(COLOR_PAIR(0));
      mvprintw(11, 34, "                                           ");
      mvprintw(12, 34, "       BLUE collided with an enemy!        ");
      mvprintw(13, 34, "                                           ");
      mvprintw(14, 34, "        Press space to play again.         ");
      mvprintw(15, 34, "                                           ");
      attroff(COLOR_PAIR(0));
      while (getch() != ' ') {};
      player_R.x = 2; 
      player_R.y = 1;
      player_B.x = 56; 
      player_B.y = 1;
    }

    // Tell ncurses to initialize color support
    start_color();

    // Color pair 0 is white text on a black background
    init_pair(0, COLOR_BLACK, COLOR_WHITE);

    // Level 3: If the player stands on the button, change the color of the tiles for the other player
    if (map.data[index_R_current] == CELL_BUTTON_R) {
      for (int i = RED; i < RED + 5; i++){
        map.data[i] = CELL_BLUE;
      }
    } else {
      for (int i = RED; i < RED + 5; i++){
        map.data[i] = CELL_RED;
      }
    }
    if (map.data[index_B_current] == CELL_BUTTON_B){
      for (int i = BLUE; i < BLUE + 5; i++){
         map.data[i] = CELL_RED;
      }
     } else {
      for (int i = BLUE; i < BLUE + 5; i++){
         map.data[i] = CELL_BLUE;
      }
     }

     // Display an error message if the player stands on the wrong color
    if ((map.data[index_R] == CELL_BLUE) || (map.data[index_B] == CELL_RED)){
      attron(COLOR_PAIR(0));
      mvprintw(11, 34, "                                               ");
      mvprintw(12, 34, "  You can't step on the other player's color!  ");
      mvprintw(13, 34, "                                               ");
      mvprintw(14, 34, "          Press space to play again.           ");
      mvprintw(15, 34, "                                               ");
      attroff(COLOR_PAIR(0));

      // If the player presses space, restart
      while (getch() != ' ') {};
      player_R.x = 2; 
      player_R.y = 1;
      player_B.x = 56; 
      player_B.y = 1;
    }
  

    // Move down constantly if there's no wall, red tiles, or blue tiles one cell below the current position
    if (map.data[index_R] != CELL_WALL && map.data[index_R] != CELL_RED && map.data[index_R] != CELL_BLUE){
      velocity_y_R += 1.3;
    } else {
      velocity_y_R = 0;
    }

    if (map.data[index_B] != CELL_WALL && map.data[index_B] != CELL_RED && map.data[index_B] != CELL_BLUE){
      velocity_y_B += 1.3;
    } else {
      velocity_y_B = 0;
    }

    // Level 4: 
    // Yellow button and door
    if ((map.data[index_R_current] == CELL_YELLOW1) || (map.data[index_B_current] == CELL_YELLOW1)) {
      for (int i = YELLOW2; i < YELLOW2 + (3 * WIDTH + 1); i+= WIDTH) {
        map.data[i] = CELL_BG;
      }
    } else {
      for (int i = YELLOW2; i < YELLOW2 + (3 * WIDTH + 1); i+= WIDTH) {
        map.data[i] = CELL_YELLOW_WALL;
      }
    }

    // Magenta button and door 
    if ((map.data[index_R_current] == CELL_MAGENTA) || (map.data[index_B_current] == CELL_MAGENTA)) {
      for (int i = MAGENTA; i < MAGENTA + (3 * WIDTH + 1); i+= WIDTH) {
        map.data[i] = CELL_BG;
      }
    } else {
      for (int i = MAGENTA; i < MAGENTA + (3 * WIDTH + 1); i+= WIDTH) {
        map.data[i] = CELL_MAGENTA_WALL;
      }
    }

    // Display a message when both players reach the goal tile
    if ((map.data[index_R_current] == CELL_MAGENTA_FINISH) && (map.data[index_B_current] == CELL_MAGENTA_FINISH)) {
      attron(COLOR_PAIR(0));
      mvprintw(11, 34, "                                           ");
      mvprintw(12, 34, "             Congratulations!!             ");
      mvprintw(13, 34, "                                           ");
      mvprintw(14, 34, "       Press space to play again :)        ");
      mvprintw(15, 34, "                                           ");
      attroff(COLOR_PAIR(0));
      // Restart if space is pressed
      while (getch() != ' ') {};
      player_R.x = 2; 
      player_R.y = 1;
      player_B.x = 50; 
      player_B.y = 1;
    }

    // Check for user input
    int input;
    
    while ((input = getch()) != ERR) {
      if (input == 'q') {
        // Exit when the user types q
        running = false;
      }
      // Player Red movement
      if (input == 'w' && map.data[index_R] == CELL_WALL) {
        key_up_R = true;
      } 
      if (input == 'a') {
        key_left_R = true;
      } 
      if (input == 'd') {
        key_right_R = true;
      }
      // Player Blue movement
      if (input == KEY_UP && map.data[index_B] == CELL_WALL) {
        key_up_B = true;
      }
      if (input == KEY_LEFT){
        key_left_B = true;
      }
      if (input == KEY_RIGHT) {
        key_right_B = true;
      }
    }

    // Jump movement
    // Quickly decrease the velocity for y-coordinates
    if (key_up_R) {
      velocity_y_R -= 15;
      key_up_R = false;
    }
    if (key_up_B){
      velocity_y_B -= 15;
      key_up_B = false;
    }

    // Left movement
    // Decrease the velocity for x-coordinates to move the player leftward
    if (key_left_R) {
      if (left_delay_R < 2) {
        velocity_x_R = -16;
        left_delay_R++;
      } else {
        key_left_R = false;
        left_delay_R--;
      }
    }
    if (key_left_B) {
      if (left_delay_B < 2) {
        velocity_x_B = -16;
        left_delay_B++;
      } else {
        key_left_B = false;
        left_delay_B--;
      }
    }

    // Slow down to stop the player when left keys are not pressed
    if (key_left_R == false){
      if (map.data[index_R] != CELL_BLANK){
        if (velocity_x_R <= 0) {
        velocity_x_R *= 0.9;
        }
      }
    }
    if (key_left_B == false){
      if (map.data[index_B] != CELL_BLANK){
        if (velocity_x_B <= 0) {
          velocity_x_B *= 0.9;
        }
      }
    }

    // Right movement
    // Increase the velocity for x-coordinates to move the player righttward
    if (key_right_R){
      if (right_delay_R < 3){
        velocity_x_R = 16;
        right_delay_R++;
      } else {
        key_right_R = false;
        right_delay_R = 0;
      }
    }
    if (key_right_B){
      if (right_delay_B < 3){
        velocity_x_B = 16;
        right_delay_B++;
      } else {
        key_right_B = false;
        right_delay_B = 0;
      }
    }

    // Slow down to stop the player when left keys are not pressed
    if (key_right_R == false){
      if (map.data[index_R] != CELL_BLANK){
        if (velocity_x_R >= 0) {
        velocity_x_R *= 0.9;
        }
      }
    }
    if (key_right_B == false){
      if (map.data[index_B] != CELL_BLANK){
        if (velocity_x_B >= 0) {
          velocity_x_B *= 0.9;
        }
      }
    }

    // Update player positions using its velocity
    player_R.x += velocity_x_R / FRAME_RATE;
    player_B.x += velocity_x_B / FRAME_RATE;

    player_R.y += velocity_y_R / FRAME_RATE;
    player_B.y += velocity_y_B / FRAME_RATE;

    // Prevent the players from moving through walls
    if (map.data[map.width * ((int)(player_R.y)) + (int)(player_R.x)] == CELL_WALL || 
        map.data[map.width * ((int)(player_R.y)) + (int)(player_R.x)] == CELL_MAGENTA_WALL || 
        map.data[map.width * ((int)(player_R.y)) + (int)(player_R.x)] == CELL_YELLOW_WALL){
      player_R.x -= velocity_x_R / FRAME_RATE;
    } 
    if (map.data[map.width * ((int)(player_B.y)) + (int)(player_B.x)] == CELL_WALL ||
        map.data[map.width * ((int)(player_B.y)) + (int)(player_B.x)] == CELL_MAGENTA_WALL ||
        map.data[map.width * ((int)(player_B.y)) + (int)(player_B.x)] == CELL_YELLOW_WALL){
      player_B.x -= velocity_x_B / FRAME_RATE;
    }

    loop_counter++;
    // Display the map and player
    display_map(map, player_R, player_B, object1, object2);

    // Display everything
    refresh();

    // Pause to limit frame rate
    sleep_ms(1000 / FRAME_RATE);
  }

  // Shut down
  endwin();

  // Clean up the map
  free_map(&map);
}