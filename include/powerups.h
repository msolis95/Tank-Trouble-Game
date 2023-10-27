#ifndef __POWERUPS_H__
#define __POWERUPS_H__

#include "maze.h"
#include "state.h"
#include <stdio.h>
#include "tank.h"

typedef struct state {
  scene_t *scene;
  tank_t *red_player;
  tank_t *green_player;
  tank_t *blue_player;
  maze_t *maze;
  size_t red_wins;
  size_t green_wins;
  size_t blue_wins;
  size_t games_played;
  double dt;
  double count_down_until_next_game_start;
  double count_down_until_next_powerup;
  size_t tank_controls; // 0: red_player uses awsd, 1: red_player uses ijkl, 2: red_player uses arrows
} state_t;

typedef enum {
  RAILGUN,
  LASER,
  SHOTGUN,
  MOON
} powerup_type_t;

/**
 * Shoot normally from a tank
 *
 * @param tank the tank to shoot from
 */
void normal_shot(state_t* state, tank_t *tonk, size_t tank_size);

/**
 * Shoot railgunnily from a tank
 *
 * @param tank the tank to shoot from
 */
void railgun_shot(state_t* state, tank_t *tonk, size_t tank_size);

/**
 * Shoot a laser from a tank
 *
 * @param tank the tank to shoot from
 */
void laser_shot(state_t* state, tank_t *tonk, size_t tank_size);

/**
 * Shoots shotgun shells from a tank
 *
 * @param tank the tank to shoot from
 */
void shotgun_shot(state_t* state, tank_t *tonk, size_t tank_size);

/**
 * Shoot a moon from a tank
 *
 * @param tank the tank to shoot from
 */
void moon_shot(state_t* state, tank_t *tonk, size_t tank_size);

/**
 * Add a powerup to the state
 *
 * @param state the state
 */
void add_powerup(state_t *state, powerup_type_t powerup_type);

#endif // #ifndef __POWERUPS_H__
