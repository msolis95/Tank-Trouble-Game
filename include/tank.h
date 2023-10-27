#ifndef __TANK_H__
#define __TANK_H__

#include <math.h>
#include "sdl_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "state.h"
#include "maze.h"

typedef struct tank tank_t;

typedef struct mov_flags {
  size_t flag_forwards;
  size_t flag_backwards;
  size_t flag_left;
  size_t flag_right;
} mov_flags_t;

typedef void (*shot_handler_t)(state_t* state, tank_t *tonk, size_t tank_size);

typedef struct tank {
  body_t *body;
  body_t *hitbox;
  size_t bullets_onscreen;
  mov_flags_t *mov_flags; 
  shot_handler_t bang;
  size_t powerup_shots_left;
  size_t exists;
} tank_t;

typedef struct body_tank {
  tank_t *tank;
  body_t *body;
} body_tank_t;

typedef struct decay {
  body_t *body;
  double time;
  tank_t *tank;
  state_t *state;
} decay_t;

/** 
 * Adds a decay effect on the given bullet to the scene.
 * Gives to the forcer the state, time, tank, and body as aux.
 * 
 * @param state
 * @param tank
 * @param body
 * @param time
*/
void create_decay_force(state_t *state, tank_t *tank, body_t *body, double time);

/** 
 * Adds a destructive collision on the given bullet and the given tank in the scene.
 * Gives to the forcer tank and body as aux.
 * 
 * @param scene
 * @param tank
 * @param body
*/
void create_tank_destructive_collision(scene_t *scene, tank_t *tank,
                                  body_t *body);

/**
 * Creates the shape of a tank
 * 
 * @param center the center of the tank
 * @param size the size of the tank
 * @return the list of vectors making up the shape of the tank
*/
list_t *tank_shape(vector_t center, size_t size);

/**
 * Creates the shape of a hitbox
 * 
 * @param center the center of the tank
 * @param size the size of the tank
 * @return the list of vectors making up the shape of the hitbox
*/
list_t *tank_hitbox_shape(vector_t center, size_t size);

/**
 * Creates a tank with a body and a hitbox
 * 
 * @param center the center of the tank
 * @param size the size of the tank
 * @param color the color of the body
 * @return the new tank
*/
tank_t *tank_init(vector_t center, size_t size, rgb_color_t color);

/**
 * Returns the body of a tank
 * 
 * @param tank the tank
 * @return the body of the tank
*/
body_t *get_tank_body(tank_t *tank);

/**
 * Returns the hitbox of a tank
 * 
 * @param tank the tank
 * @return the hitbox of the tank
*/
body_t *get_tank_hitbox(tank_t *tank);

/**
 * Moves a tank
 * 
 * @param tank the tank
 * @param velocity the new velocity of the tank
*/
void tank_move(tank_t *tank, double velocity);

/** 
 * Translates the given tank to the given position. 
 * Moves both the tank shape itself and the hitbox.
 * 
 * @param tank
 * @param position
 */
void tank_set_position(tank_t *tank, vector_t position);

/** 
 * Rotates the given tank by the given angle. 
 * Moves both the tank shape itself and the hitbox.
 * 
 * @param tank
 * @param angle
 */
void tank_rotate(tank_t *tank, double angle);

/** 
 * Sets the flags that indicates its current movement.
 * 
 * @param tank
 * @param forwards
 * @param backwards
 * @param left
 * @param right
 */
void tank_set_flags(tank_t *tank, size_t forwards, size_t backwards, size_t left, size_t right);

/** 
 * Gets the flags that indicates its current movement.
 * 
 * @param tank
 */
list_t* tank_get_flags(tank_t *tank);

/** 
 * Based on the flags and tank's orientation,
 * translates and rotates the tank.
 *
 * @param tank
 */
void tank_execute_flags(tank_t *tank);

/** 
 * Removes the tank from the state.
 * Calls tank_debris to plays the debris effect.
 *
 * @param tank
 * @param state
 */
void tank_remove(tank_t *tank, state_t *state);

/** 
 * Plays the debris effect to be called when a tank
 * is removed.
 *
 * @param tank
 * @param state
 */
void tank_debris(tank_t *tank, state_t *state);

/** 
 * The forcer function between a tank and maze
 * It calculates the collision with only the 4 walls
 * around the tank instead of all walls.
 * 
 * @param aux
 */
void tank_maze_force(void *aux);

/** 
 * Adds the collision between tank and maze using
 * tank_maze_force as the forcer.
 * 
 * @param state
 * @param maze
 * @param tank
 * @param tank_size
 */
void add_tank_maze_force(state_t *state, maze_t *maze, tank_t *tank, size_t tank_size);

/** 
 * Adds the collision between tank and special bullet
 * that doesn't die when collided with the tank.
 *
 * @param scene
 * @param tank
 * @param body
 */
void create_tank_killbox(scene_t *scene, tank_t *tank, body_t *body);

#endif // #ifndef __TANK_H__
