#include "tank.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "powerups.h"
#include "collision.h"
#include "maze.h"
#include <SDL2/SDL_mixer.h>

#define TAU (6.28318530717958)

const char *DEATH_SOUND_PATH = "assets/lego_death.wav";

// tank constants
const double TANK_MASS = 100000;
const double TANK_SPEED = 300.;
const double TANK_ROTATION = TAU/110.;

// debris constants
const double DEBRIS_VELOCITY = 400.;
const double DEBRIS_MASS = 100;
const double DEBRIS_DECAY = 1.;
const double DEBRIS_DRAG = 250;
const size_t DEBRIS_SIZE = 2.;

bool tank_is_not_null(state_t *state, tank_t *tank) {
  if (tank != NULL && (tank == state->red_player || tank == state->green_player || tank == state->blue_player)) {
    return 1;
  }
  return 0;
}

void decay_force(void *aux) {
  decay_t *decay = aux;
  body_t *body = decay->body;
  double dt = decay->state->dt;
  decay->time = decay->time - dt;
  if (decay->time <= 0 || body_is_removed(body)) {
    body_remove(decay->body);
    if (tank_is_not_null(decay->state, decay->tank)) {
      decay->tank->bullets_onscreen = decay->tank->bullets_onscreen - 1;
    }
  }
}

void create_decay_force(state_t *state, tank_t *tank, body_t *body, double time) {
  decay_t *aux = malloc(sizeof(decay_t));
  assert(aux != NULL);
  *aux = (decay_t){.state = state, .time = time, .tank = tank, .body = body};
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(state->scene, decay_force, aux, bodies, free);
}

void tank_force_of_nature(void *aux) {
  tank_t *tank = ((body_tank_t *)aux)->tank;
  body_t *body = ((body_tank_t *)aux)->body;
  if (find_collision(tank->hitbox->shape, body->shape).collided) {
    tank->exists = 0;
    body_remove(tank->hitbox);
    body_remove(body);
  }
}

void create_tank_destructive_collision(scene_t *scene, tank_t *tank,
                                  body_t *body) {
  body_tank_t *aux = malloc(sizeof(body_tank_t));
  assert(aux != NULL);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, tank->hitbox);
  list_add(bodies, body);
  *aux = (body_tank_t){.tank = tank, .body = body};
  scene_add_bodies_force_creator(scene, tank_force_of_nature, aux, bodies, free);
}

void tank_unboxing(void *aux) {
  tank_t *tank = ((body_tank_t *)aux)->tank;
  body_t *body = ((body_tank_t *)aux)->body;
  if (find_collision(tank->hitbox->shape, body->shape).collided) {
    tank->exists = 0;
    body_remove(tank->hitbox);
  }
}

void create_tank_killbox(scene_t *scene, tank_t *tank,
                          body_t *body) {
  body_tank_t *aux = malloc(sizeof(body_tank_t));
  assert(aux != NULL);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, tank->hitbox);
  list_add(bodies, body);
  *aux = (body_tank_t){.tank = tank, .body = body};
  scene_add_bodies_force_creator(scene, tank_unboxing, aux, bodies, free);
}

list_t *tank_shape(vector_t center, size_t size) {
  vector_t *track1TL = malloc(sizeof(vector_t));
  vector_t *track1TR = malloc(sizeof(vector_t));
  vector_t *track1BL = malloc(sizeof(vector_t));
  vector_t *track1BR = malloc(sizeof(vector_t));
  vector_t *track1MT = malloc(sizeof(vector_t));
  vector_t *track1MB = malloc(sizeof(vector_t));

  vector_t *track2TL = malloc(sizeof(vector_t));
  vector_t *track2TR = malloc(sizeof(vector_t));
  vector_t *track2BL = malloc(sizeof(vector_t));
  vector_t *track2BR = malloc(sizeof(vector_t));
  vector_t *track2MT = malloc(sizeof(vector_t));
  vector_t *track2MB = malloc(sizeof(vector_t));

  vector_t *barrelTL = malloc(sizeof(vector_t));
  vector_t *barrelTR = malloc(sizeof(vector_t));
  vector_t *barrelBL = malloc(sizeof(vector_t));
  vector_t *barrelBR = malloc(sizeof(vector_t));


  *track1TL = (vector_t){.x = center.x - size * 0.5, .y = center.y + size * 0.5};
  *track1TR = (vector_t){.x = center.x - size * 0.25, .y = center.y + size * 0.5};
  *track1BL = (vector_t){.x = center.x - size * 0.5, .y = center.y - size * 0.5};
  *track1BR = (vector_t){.x = center.x - size * 0.25, .y = center.y - size * 0.5};
  *track1MT = (vector_t){.x = center.x - size * 0.25, .y = center.y + size * 0.3};
  *track1MB = (vector_t){.x = center.x - size * 0.25, .y = center.y - size * 0.3};
 
  *track2TL = (vector_t){.x = center.x + size * 0.25, .y = center.y + size * 0.5};
  *track2TR = (vector_t){.x = center.x + size * 0.5, .y = center.y + size * 0.5};
  *track2BL = (vector_t){.x = center.x + size * 0.25, .y = center.y - size * 0.5};
  *track2BR = (vector_t){.x = center.x + size * 0.5, .y = center.y - size * 0.5};
  *track2MT = (vector_t){.x = center.x + size * 0.25, .y = center.y + size * 0.3};
  *track2MB = (vector_t){.x = center.x + size * 0.25, .y = center.y - size * 0.3};

  *barrelTL = (vector_t){.x = center.x - size * 0.1, .y = center.y + size * 0.8};
  *barrelTR = (vector_t){.x = center.x + size * 0.1, .y = center.y + size * 0.8};
  *barrelBL = (vector_t){.x = center.x - size * 0.1, .y = center.y + size * 0.3};
  *barrelBR = (vector_t){.x = center.x + size * 0.1, .y = center.y + size * 0.3};

  list_t *shape = list_init(8, free);
  assert(shape != NULL);
  list_add(shape, track1MT);
  list_add(shape, track1TR);
  list_add(shape, track1TL);
  list_add(shape, track1BL);
  list_add(shape, track1BR);
  list_add(shape, track1MB);
  list_add(shape, track2MB);
  list_add(shape, track2BL);
  list_add(shape, track2BR);
  list_add(shape, track2TR);
  list_add(shape, track2TL);
  list_add(shape, track2MT);
  list_add(shape, barrelBR);
  list_add(shape, barrelTR);
  list_add(shape, barrelTL);
  list_add(shape, barrelBL);
  return shape;
}

list_t *tank_hitbox_shape(vector_t center, size_t size) {
  vector_t *hitboxTR = malloc(sizeof(vector_t));
  vector_t *hitboxTL = malloc(sizeof(vector_t));
  vector_t *hitboxBR = malloc(sizeof(vector_t));
  vector_t *hitboxBL = malloc(sizeof(vector_t));
  assert(hitboxTR != NULL);
  assert(hitboxTL != NULL);
  assert(hitboxBR != NULL);
  assert(hitboxBL != NULL);

  *hitboxTR = (vector_t){.x = center.x + size * 0.5, .y = center.y + size * 0.5};
  *hitboxTL = (vector_t){.x = center.x - size * 0.5, .y = center.y + size * 0.5};
  *hitboxBR = (vector_t){.x = center.x + size * 0.5, .y = center.y - size * 0.5};
  *hitboxBL = (vector_t){.x = center.x - size * 0.5, .y = center.y - size * 0.5};

  list_t *shape = list_init(8, free);
  list_add(shape, hitboxTR);
  list_add(shape, hitboxTL);
  list_add(shape, hitboxBL);
  list_add(shape, hitboxBR);
  return shape;
}

tank_t *tank_init(vector_t center, size_t size, rgb_color_t color) {
  tank_t *tank = malloc(sizeof(tank_t));
  assert(tank != NULL);
  tank->body = body_init_with_info(tank_shape(center, size), TANK_MASS, color, NULL, free, 1);
  body_set_center(tank->body, center);
  tank->body->orientation = TAU/4;
  tank->hitbox = body_init_with_info(tank_hitbox_shape(center, size), TANK_MASS, color, NULL, free, 0);
  body_set_center(tank->hitbox, center);
  tank->hitbox->orientation = TAU/4;
  tank->bullets_onscreen = 0;
  mov_flags_t *mov_flag = malloc(sizeof(mov_flags_t));
  mov_flag->flag_backwards = 0;
  mov_flag->flag_forwards = 0;
  mov_flag->flag_left = 0;
  mov_flag->flag_right = 0;
  tank->mov_flags = mov_flag;
  tank->bang = normal_shot;
  tank->powerup_shots_left = 0;
  tank->exists = 1;
  return tank;
}

body_t *get_tank_body(tank_t *tank) {return tank->body;}

body_t *get_tank_hitbox(tank_t *tank) {return tank->hitbox;}

void tank_move(tank_t *tank, double velocity) {
  body_set_velocity(tank->body, vec_rotate((vector_t){.x = velocity, .y = 0}, body_get_orientation(tank->body)));
  body_set_velocity(tank->hitbox, vec_rotate((vector_t){.x = velocity, .y = 0}, body_get_orientation(tank->hitbox)));
}

void tank_set_position(tank_t *tank, vector_t position) {
  body_set_position(tank->body, position);
  body_set_position(tank->hitbox, position);
}

void tank_rotate(tank_t *tank, double angle) {
  body_rotate(tank->body, angle, body_get_center(tank->body));
  body_rotate(tank->hitbox, angle, body_get_center(tank->hitbox));
}

void tank_set_flags(tank_t *tank, size_t forwards, size_t backwards, size_t left, size_t right) {
  if (forwards != 2) {
    tank->mov_flags->flag_forwards = forwards;
  }
  if (backwards != 2) {
    tank->mov_flags->flag_backwards = backwards;
  }
  if (left != 2) {
    tank->mov_flags->flag_left = left;
  }
  if (right != 2) {
    tank->mov_flags->flag_right = right;
  }
}

list_t* tank_get_flags(tank_t *tank) {
  list_t *flags = list_init(4, free);
  list_add(flags, (void *)tank->mov_flags->flag_forwards);
  list_add(flags, (void *)tank->mov_flags->flag_backwards);
  list_add(flags, (void *)tank->mov_flags->flag_left);
  list_add(flags, (void *)tank->mov_flags->flag_right);
  return flags;
}

void tank_execute_flags(tank_t *tank) {
  if (tank->mov_flags->flag_forwards == 1) {
    tank_move(tank, TANK_SPEED);
  }
  if (tank->mov_flags->flag_backwards == 1) {
    tank_move(tank, -TANK_SPEED/3.);
  }
  if (tank->mov_flags->flag_backwards == 0 && tank->mov_flags->flag_forwards == 0) {
    tank_move(tank, 0.0);
  }

  if (tank->mov_flags->flag_left == 1) {
    tank_rotate(tank, TANK_ROTATION);
  }
  if (tank->mov_flags->flag_right == 1) {
    tank_rotate(tank, -TANK_ROTATION);
  }
  if (tank->mov_flags->flag_left == 0 && tank->mov_flags->flag_right == 0) {
    tank_rotate(tank, 0.0);
  }
}

void spawn_tank_debris(vector_t center, rgb_color_t color, double orientation, double speed, state_t *state) {
  vector_t *debris1 = malloc(sizeof(vector_t));
  vector_t *debris2 = malloc(sizeof(vector_t));
  vector_t *debris3 = malloc(sizeof(vector_t));
  assert(debris1 != NULL);
  assert(debris2 != NULL);
  assert(debris3 != NULL);

  *debris1 = (vector_t){.x = center.x + 10*DEBRIS_SIZE, .y = center.y - 4*DEBRIS_SIZE};
  *debris2 = (vector_t){.x = center.x - 8*DEBRIS_SIZE, .y = center.y - 4*DEBRIS_SIZE};
  *debris3 = (vector_t){.x = center.x, .y = center.y + 8*DEBRIS_SIZE};

  list_t *debris = list_init(8, free);
  list_add(debris, debris1);
  list_add(debris, debris2);
  list_add(debris, debris3);
  body_t *body = body_init_with_info(debris, DEBRIS_MASS, color, NULL, NULL, 1);
  scene_add_body(state->scene, body);
  create_drag(state->scene, DEBRIS_DRAG, body);
  body_set_velocity(body, vec_multiply(speed, (vector_t) {.x = cos(orientation), .y = sin(orientation)}));
  body_set_rotation(body, 5);
  create_decay_force(state, NULL, body, DEBRIS_DECAY);
}

void tank_debris(tank_t *tank, state_t *state) {
  for (size_t i = 0; i < 8; i++) {
    spawn_tank_debris(tank->body->center, tank->body->color, i* TAU / 8., DEBRIS_VELOCITY, state);
  }
}

void tank_remove(tank_t *tank, state_t *state) {
  Mix_Chunk *death_sound = Mix_LoadWAV(DEATH_SOUND_PATH);
  tank_debris(tank, state);
  body_remove(tank->body);
  free(tank);
  Mix_PlayChannel(-1, death_sound, 0);
}

typedef struct tank_maze_aux {
  tank_t *true_tank;
  maze_t *maze;
  size_t size;
  vector_t last_velocity;
  double last_rotation;
  state_t *state;
  double last_dt;
} tank_maze_aux_t;

void tank_maze_force(void *aux) {
  tank_maze_aux_t* cable = aux;
  tank_t *tank = ((tank_maze_aux_t *)aux)->true_tank;
  maze_t *maze = ((tank_maze_aux_t *)aux)->maze;
  cell_t position = vector_to_cell(maze, body_get_center(tank->hitbox));
  size_t at_least_one_wall_collided = 0;
  vector_t temp_vel = body_get_velocity(tank->body);
  double temp_rotate = body_get_rotation(tank->body);
  
  list_t *walls_around = get_walls_around(maze, position);
  size_t count = 1;
  collision_info_t collision_info;
  for (size_t i = 0; i < list_size(walls_around); i++) {
    body_t *wall = list_get(walls_around, i); 
    collision_info = find_collision(tank->hitbox->shape, wall->shape);
    if (collision_info.collided) {
      at_least_one_wall_collided = count;
    }
    count = count +1;
  }

  if (at_least_one_wall_collided != 0) {
    vector_t translate_vector = vec_multiply(cable->last_dt, cable->last_velocity);
    body_translate(tank->body, vec_negate(translate_vector));
    body_translate(tank->hitbox, vec_negate(translate_vector));
    body_rotate(tank->body, -1*cable->last_rotation, tank->body->center);
    body_rotate(tank->hitbox, -1*cable->last_rotation, tank->hitbox->center);
  }
  cable->last_dt = cable->state->dt;
  cable->last_velocity = temp_vel;
  cable->last_rotation = temp_rotate;
}

void add_tank_maze_force(state_t *state, maze_t *maze, tank_t *tank, size_t tank_size) {
  tank_maze_aux_t *aux = malloc(sizeof(tank_maze_aux_t));
  aux->maze = maze;
  aux->true_tank = tank;
  aux->size = tank_size;
  aux->state = state;
  aux->last_velocity = VEC_ZERO;
  aux->last_rotation = 0;
  aux->last_dt = 0;
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, tank->hitbox);
  scene_add_bodies_force_creator(state->scene, tank_maze_force, aux, bodies, free);
}