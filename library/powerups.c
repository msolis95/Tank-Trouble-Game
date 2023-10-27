#include "powerups.h"
#include "body.h"
#include "forces.h"
#include "list.h"
#include "color.h"
#include "maze.h"
#include <assert.h>
#include "tank.h"
#include "collision.h"
#include <SDL2/SDL_mixer.h>
#define TAU (2 * 3.14159265358979)

// sound constants
const char *NORMAL_SHOT_SOUND_PATH = "assets/normalshot.wav";
const char *RAILGUN_SHOT_SOUND_PATH = "assets/railgunshot.wav";
const char *LASER_SHOT_SOUND_PATH = "assets/lasershot.wav";
const char *SHOTGUN_SHOT_SOUND_PATH = "assets/shotgunshot.wav";
const char *MOON_SHOT_SOUND_PATH = "assets/moonshot.wav";

// bullet constants
const size_t BULLET_VERTICES = 6;
const double BULLET_MASS = 100.;
const double NORMAL_SIZE = 6.;
const rgb_color_t NORMAL_COLOR = (rgb_color_t) {.r = 0, .g = 0, .b = 0};
const vector_t NORMAL_VELOCITY = (vector_t) {.x = 200., .y = 0};
const double NORMAL_DECAY = 5.;

// railgun constants
const double RAILGUN_BLAST_WIDTH = 50.;
const double RAILGUN_LENGTH = 1000.;
const rgb_color_t RAILGUN_COLOR = (rgb_color_t) {.r = 1, .g = 0, .b = 0};
const double RAILGUN_DECAY = 1.0;

// laser constants
const double LASER_LENGTH = 10.;
const double LASER_WIDTH = 10.;
const rgb_color_t LASER_COLOR = (rgb_color_t) {.r = 0.5, .g = 0, .b = 0};
const vector_t LASER_VELOCITY = (vector_t) {.x = 500., .y = 0};
const double LASER_DECAY = 1.5;

// shotgun constants
const size_t SHOTGUN_BULLETS = 20;
const double SHOTGUN_ANGLE = TAU / 8.;
const double SHOTGUN_SIZE = 5.;
const rgb_color_t SHOTGUN_COLOR = (rgb_color_t){.r = 0.5, .g = 0.5, .b = 0};
const vector_t SHOTGUN_VELOCITY = (vector_t) {.x = 350., .y = 0};
const double SHOTGUN_DECAY = 1.5;
const double SHOTGUN_SHOT_RANGE = TAU/15.;

// moon constants
const rgb_color_t MOON_COLOR = (rgb_color_t){.r = 0.5, .g = 0.5, .b = 0.5};
const size_t MOON_SIZE = 30;
const vector_t MOON_VELOCITY = (vector_t) {.x = 300., .y = 0};
const size_t MOON_MASS = 5;
const size_t MOON_G = 10;

// powerup box constants
const double POWERUP_BOX_LENGTH = 25;

vector_t bullet_location(double orientation, vector_t center, size_t tank_size, size_t bullet_size, double forwards) {
  return vec_add(vec_rotate((vector_t) {.x = (tank_size/2+1.) + (bullet_size/2+1.) + forwards, .y = 0}, orientation), center);
}

list_t *normal_bullet(double size, vector_t center) {
  double curr_angle = 0;
  double vert_angle = TAU/(double)BULLET_VERTICES;
  double x;
  double y;
  list_t *bullet = list_init(BULLET_VERTICES, free);
  assert(bullet != NULL);
  for (size_t i = 0; i < BULLET_VERTICES; i++) {
    x = cos(curr_angle) * size + center.x;
    y = sin(curr_angle) * size + center.y;
    vector_t *vec_ptr = malloc(sizeof(vector_t));
    assert(vec_ptr != NULL);
    *vec_ptr = (vector_t) { .x = x, .y = y };
    list_add(bullet, vec_ptr);
    curr_angle += vert_angle;
  }
  return bullet;
}

void normal_shot(state_t* state, tank_t *tonk, size_t tank_size) {
  Mix_Chunk *shot_sound = Mix_LoadWAV(NORMAL_SHOT_SOUND_PATH);
  Mix_PlayChannel(-1, shot_sound, 0);
  body_t *tank = get_tank_hitbox(tonk);
  double orientation = body_get_orientation(tank);
  vector_t bullet_loc = bullet_location(body_get_orientation(tank), body_get_center(tank), tank_size, NORMAL_SIZE, 5);
  body_t *bullet = body_init_with_info(
    normal_bullet(NORMAL_SIZE, bullet_loc), BULLET_MASS, NORMAL_COLOR, NULL, NULL, 1
    );
  scene_t *scene = state->scene;
  vector_t vel = vec_rotate(NORMAL_VELOCITY, orientation);
  body_set_velocity(bullet, vel);
  add_maze_collision(scene, state->maze, bullet);
  tank_t *red_player = state->red_player;
  tank_t *blue_player = state->blue_player;
  tank_t *green_player = state->green_player;
  if (red_player != NULL) {
    create_tank_destructive_collision(scene, red_player, bullet);
  }
  if (blue_player != NULL) {
    create_tank_destructive_collision(scene, blue_player, bullet);
  }
  if (green_player != NULL) {
    create_tank_destructive_collision(scene, green_player, bullet);
  }
  create_decay_force(state, tonk, bullet, NORMAL_DECAY);
  tonk->bullets_onscreen = tonk->bullets_onscreen + 1;
  scene_add_body(scene, bullet);
}

list_t *rectangle(double length, double width, double orientation, vector_t center) {
  list_t *bullet = list_init(4, free);

  vector_t *vec_ptr = malloc(sizeof(vector_t));
  assert(vec_ptr != NULL);
  *vec_ptr = vec_add(vec_rotate((vector_t) { .x = length/2, .y = width/2 }, orientation), center);
  list_add(bullet, vec_ptr);

  vec_ptr = malloc(sizeof(vector_t));
  assert(vec_ptr != NULL);
  *vec_ptr = vec_add(vec_rotate((vector_t) { .x = length/2, .y = -width/2 }, orientation), center);
  list_add(bullet, vec_ptr);

  vec_ptr = malloc(sizeof(vector_t));
  assert(vec_ptr != NULL);
  *vec_ptr = vec_add(vec_rotate((vector_t) { .x = -length/2, .y = -width/2 }, orientation), center);
  list_add(bullet, vec_ptr);

  vec_ptr = malloc(sizeof(vector_t));
  assert(vec_ptr != NULL);
  *vec_ptr = vec_add(vec_rotate((vector_t) { .x = -length/2, .y = width/2 }, orientation), center);
  list_add(bullet, vec_ptr);

  return bullet;
}

void railgun_shot(state_t* state, tank_t *tonk, size_t tank_size) {
  Mix_Chunk *shot_sound = Mix_LoadWAV(RAILGUN_SHOT_SOUND_PATH);
  Mix_PlayChannel(-1, shot_sound, 0);
  body_t *tank = get_tank_hitbox(tonk);
  double orientation = body_get_orientation(tank);
  body_t *bullet = body_init_with_info(
    rectangle(RAILGUN_LENGTH, RAILGUN_BLAST_WIDTH, orientation, bullet_location(body_get_orientation(tank),
              body_get_center(tank), tank_size, RAILGUN_LENGTH, 20)),
    BULLET_MASS, RAILGUN_COLOR, NULL, NULL, 1);
  scene_t *scene = state->scene;
  tank_t *red_player = state->red_player;
  tank_t *blue_player = state->blue_player;
  tank_t *green_player = state->green_player;
  if (red_player != NULL) { 
    create_tank_killbox(scene, red_player, bullet);
  }
  if (blue_player != NULL) {
    create_tank_killbox(scene, blue_player, bullet);
  }
  if (green_player != NULL) {
    create_tank_killbox(scene, green_player, bullet);
  }
  create_decay_force(state, tonk, bullet, RAILGUN_DECAY);
  tonk->bullets_onscreen = tonk->bullets_onscreen + 1;
  tonk->powerup_shots_left = tonk->powerup_shots_left - 1;
  scene_add_body(scene, bullet);
}

void laser_shot(state_t* state, tank_t *tonk, size_t tank_size) {
  Mix_Chunk *shot_sound = Mix_LoadWAV(LASER_SHOT_SOUND_PATH);
  Mix_PlayChannel(-1, shot_sound, 0);
  body_t *tank = get_tank_hitbox(tonk);
  double orientation = body_get_orientation(tank);
  scene_t *scene = state->scene;
  vector_t vel = vec_rotate(LASER_VELOCITY, orientation);
  for (size_t i = 0; i < 20; i++) {
    body_t *bullet = body_init_with_info(  
    rectangle(LASER_LENGTH, LASER_WIDTH, body_get_orientation(tank), 
    bullet_location(body_get_orientation(tank), body_get_center(tank), tank_size, LASER_LENGTH, 5 + i*5)), BULLET_MASS, LASER_COLOR, NULL, NULL, 1);
    body_set_velocity(bullet, vel);
    add_maze_collision(scene, state->maze, bullet);
    tank_t *red_player = state->red_player;
    tank_t *blue_player = state->blue_player;
    tank_t *green_player = state->green_player;
    if (red_player != NULL) {
      create_tank_destructive_collision(scene, red_player, bullet);
    }
    if (blue_player != NULL) {
      create_tank_destructive_collision(scene, blue_player, bullet);
    }
    if (green_player != NULL) {
      create_tank_destructive_collision(scene, green_player, bullet);
    }
    create_decay_force(state, tonk, bullet, LASER_DECAY);
    tonk->bullets_onscreen = tonk->bullets_onscreen + 1;
    scene_add_body(scene, bullet);
  }
  tonk->powerup_shots_left = tonk->powerup_shots_left - 1;
}

void shotgun_shot(state_t* state, tank_t *tonk, size_t tank_size) {
  body_t *tank = get_tank_hitbox(tonk);
  double tank_orientation = body_get_orientation(tank);
  tank_t *red_player = state->red_player;
  tank_t *green_player = state->green_player;
  tank_t *blue_player = state->blue_player;
  for (size_t i = 0; i < SHOTGUN_BULLETS; i++) {
    double bullet_orientation = tank_orientation + ((double)i - (double)SHOTGUN_BULLETS/2.)*(SHOTGUN_SHOT_RANGE)/(SHOTGUN_BULLETS/2.);
    body_t *bullet = body_init_with_info(
      normal_bullet(SHOTGUN_SIZE, bullet_location(bullet_orientation, body_get_center(tank), tank_size, SHOTGUN_SIZE, 20)), 
                BULLET_MASS, SHOTGUN_COLOR, NULL, NULL, 1);
    scene_t *scene = state->scene;
    body_set_velocity(bullet, vec_rotate(SHOTGUN_VELOCITY, bullet_orientation));
    if (red_player != NULL) {
      create_tank_destructive_collision(scene, red_player, bullet);
    }
    if (blue_player != NULL) {
      create_tank_destructive_collision(scene, blue_player, bullet);
    }
    if (green_player != NULL) {
      create_tank_destructive_collision(scene, green_player, bullet);
    }
    create_decay_force(state, tonk, bullet, SHOTGUN_DECAY);
    tonk->bullets_onscreen = tonk->bullets_onscreen + 1;
    add_maze_collision(scene, state->maze, bullet);
    scene_add_body(scene, bullet);
  }
  tonk->powerup_shots_left = tonk->powerup_shots_left - 1;
  Mix_Chunk *shot_sound = Mix_LoadWAV(SHOTGUN_SHOT_SOUND_PATH);
  Mix_PlayChannel(-1, shot_sound, 0);
}

void moon_shot(state_t* state, tank_t *tonk, size_t tank_size) {
  body_t *tank = get_tank_hitbox(tonk);
  double orientation = body_get_orientation(tank);
  vector_t bullet_loc = bullet_location(body_get_orientation(tank), body_get_center(tank), tank_size, NORMAL_SIZE, 30);
  body_t *bullet = body_init_with_info(
    normal_bullet(MOON_SIZE, bullet_loc), MOON_MASS, NORMAL_COLOR, NULL, NULL, 1
    );
  scene_t *scene = state->scene;
  vector_t vel = vec_rotate(MOON_VELOCITY, orientation);
  body_set_velocity(bullet, vel);
  tank_t *red_player = state->red_player;
  tank_t *blue_player = state->blue_player;
  tank_t *green_player = state->green_player;
  if (red_player != NULL) {
    create_tank_destructive_collision(scene, red_player, bullet);
    create_newtonian_gravity(state->scene, MOON_G, bullet, red_player->hitbox);
    create_newtonian_gravity(state->scene, MOON_G, bullet, red_player->body);
  }
  if (green_player != NULL) {
    create_tank_destructive_collision(scene, green_player, bullet);
    create_newtonian_gravity(state->scene, MOON_G, bullet, green_player->hitbox);
    create_newtonian_gravity(state->scene, MOON_G, bullet, green_player->body);
  }
  if (blue_player != NULL) {
    create_tank_destructive_collision(scene, blue_player, bullet);
    create_newtonian_gravity(state->scene, MOON_G, bullet, blue_player->hitbox);
    create_newtonian_gravity(state->scene, MOON_G, bullet, blue_player->body);
  }
  scene_add_body(scene, bullet);
  tonk->powerup_shots_left = tonk->powerup_shots_left - 1;
  Mix_Chunk *shot_sound = Mix_LoadWAV(MOON_SHOT_SOUND_PATH);
  Mix_PlayChannel(-1, shot_sound, 0);
}

list_t *powerup_shape(vector_t center) {
  return rectangle(POWERUP_BOX_LENGTH, POWERUP_BOX_LENGTH, 0, center);
}

body_t *powerup_init(vector_t center, rgb_color_t color, powerup_type_t powerup_info) {
  return body_init_with_info(powerup_shape(center), INFINITY, color, (void *)powerup_info, NULL, 1);
}

void powerup_force(void *aux) {
  tank_t *tank = ((body_tank_t *)aux)->tank;
  body_t *powerup = ((body_tank_t *)aux)->body;
  if (find_collision(tank->hitbox->shape, powerup->shape).collided) {
    powerup_type_t powerup_type = (powerup_type_t)powerup->info;
    if (powerup_type == RAILGUN) {
      tank->bang = railgun_shot;
      tank->powerup_shots_left = 1;
    }
    if (powerup_type == LASER) {
      tank->bang = laser_shot;
      tank->powerup_shots_left = 1;
    }
    if (powerup_type == SHOTGUN) {
      tank->bang = shotgun_shot;
      tank->powerup_shots_left = 2;
    }
    if (powerup_type == MOON) {
      tank->bang = moon_shot;
      tank->powerup_shots_left = 1;
    }
    body_remove(powerup);
  }
}

void create_powerup_destructive_collision(scene_t *scene, tank_t *tank, body_t *powerup) {
  body_tank_t *aux = malloc(sizeof(body_tank_t));
  assert(aux != NULL);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, tank->hitbox);
  list_add(bodies, powerup);
  *aux = (body_tank_t){.tank = tank, .body = powerup};
  scene_add_bodies_force_creator(scene, powerup_force, aux, bodies, free);
}

void add_powerup(state_t *state, powerup_type_t powerup_type) {
  rgb_color_t color;
  if (powerup_type == RAILGUN) {
    color = RAILGUN_COLOR;
  } else if (powerup_type == LASER) {
    color = LASER_COLOR;
  } else if (powerup_type == SHOTGUN) {
    color = SHOTGUN_COLOR;
  } else {
    color = MOON_COLOR;
  }
  body_t *powerup = powerup_init(get_random_cell_center(state->maze), color, powerup_type);
  scene_add_body(state->scene, powerup);
  if (state->red_player != NULL) {
    create_powerup_destructive_collision(state->scene, state->red_player, powerup);
  }
  if (state->green_player != NULL) {
    create_powerup_destructive_collision(state->scene, state->green_player, powerup);
  }
  if (state->blue_player != NULL) {
    create_powerup_destructive_collision(state->scene, state->blue_player, powerup);
  }
}
