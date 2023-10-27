#include "body.h"
#include "collision.h"
#include "forces.h"
#include "math.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "tank.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "powerups.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define TAU (2 * 3.14159265358979)

// window constants
const vector_t WINDOW = (vector_t){.x = 1000., .y = 500.};
const size_t MAGIC_NUMBER = 8;

// game constants
const double COUNT_DOWN_NEXT_GAME = 5.;
const double CANNON_EVENT_2099 = 0.0001;

// maze constants
size_t MAZE_COLUMNS = 10;
size_t MAZE_ROWS = 5;

// tank constants
const rgb_color_t RED_PLAYER_COLOR = {.r = 1., .g = 0., .b = 0.};
const rgb_color_t GREEN_PLAYER_COLOR = {.r = 0., .g = 1., .b = 0.};
const rgb_color_t BLUE_PLAYER_COLOR = {.r = 0., .g = 0., .b = 1.};
const size_t TANK_SIZE = 50;
const size_t BULLET_LIMIT = 5;

// powerup constants
const double POWERUP_SPAWN_INTERVAL = 5;
const double POWERUP_SPAWN_PROB = .25;

void tank_shoot(state_t *state, tank_t *tank) {
  if (tank->powerup_shots_left <= 0) {
    tank->bang = normal_shot;
  }
  tank->bang(state, tank, TANK_SIZE);
}

void on_key(char key, key_event_type_t key_type, double held_time, void *state) {
  tank_t *red_player;
  tank_t *blue_player;
  tank_t *green_player;
  if (((state_t *)state)->tank_controls == 0) {
    red_player = ((state_t *)state)->red_player;
    blue_player = ((state_t *)state)->blue_player;
    green_player = ((state_t *)state)->green_player;
  } else if (((state_t *)state)->tank_controls == 1) {
    green_player = ((state_t *)state)->red_player;
    red_player = ((state_t *)state)->blue_player;
    blue_player = ((state_t *)state)->green_player;
  } else {
    blue_player = ((state_t *)state)->red_player;
    green_player = ((state_t *)state)->blue_player;
    red_player = ((state_t *)state)->green_player;
  }

  if (key_type == KEY_PRESSED) {
    if (red_player != NULL) {
      switch (key) {
        case SDLK_q:
          if (red_player->bullets_onscreen < BULLET_LIMIT) {
            tank_shoot(state, red_player);
          }
          else {
            Mix_Chunk *empty_sound = Mix_LoadWAV("assets/empty_shot.wav");
            Mix_PlayChannel(-1, empty_sound, 0);
          }
          break;
        case SDLK_w:
          tank_set_flags(red_player, 1, 2, 2, 2);
          break;
        case SDLK_s:
          tank_set_flags(red_player, 2, 1, 2, 2);
          break;
        case SDLK_a:
          tank_set_flags(red_player, 2, 2, 1, 2);
          break;
        case SDLK_d:
          tank_set_flags(red_player, 2, 2, 2, 1);
          break;
      }
    }

    if (green_player != NULL) {
      switch (key) {
        case SDLK_u:
          if (green_player->bullets_onscreen < BULLET_LIMIT) {
            tank_shoot(state, green_player);
          }
          else {
            Mix_Chunk *empty_sound = Mix_LoadWAV("assets/empty_shot.wav");
            Mix_PlayChannel(-1, empty_sound, 0);
          }
          break;
        case SDLK_i:
          tank_set_flags(green_player, 1, 2, 2, 2);
          break;
        case SDLK_k:
          tank_set_flags(green_player, 2, 1, 2, 2);
          break;
        case SDLK_j:
          tank_set_flags(green_player, 2, 2, 1, 2);
          break;
        case SDLK_l:
          tank_set_flags(green_player, 2, 2, 2, 1);
          break;
      }
    }

    if (blue_player != NULL) {
      switch (key) {
        case SPACE_BAR:
          if (blue_player->bullets_onscreen < BULLET_LIMIT) {
            tank_shoot(state, blue_player);
          }
          else {
            Mix_Chunk *empty_sound = Mix_LoadWAV("assets/empty_shot.wav");
            Mix_PlayChannel(-1, empty_sound, 0);
          }
          break;
        case UP_ARROW:
          tank_set_flags(blue_player, 1, 2, 2, 2);
          break;
        case DOWN_ARROW:
          tank_set_flags(blue_player, 2, 1, 2, 2);
          break;
        case LEFT_ARROW:
          tank_set_flags(blue_player, 2, 2, 1, 2);
          break;
        case RIGHT_ARROW:
          tank_set_flags(blue_player, 2, 2, 2, 1);
          break;
      }
    }            
  }

  if (key_type == KEY_RELEASED) {
    if (red_player != NULL) {
      switch (key) {
        case SDLK_w:
          tank_set_flags(red_player, 0, 2, 2, 2);
          break;
        case SDLK_s:
          tank_set_flags(red_player, 2, 0, 2, 2);
          break;
        case SDLK_a:
          tank_set_flags(red_player, 2, 2, 0, 2);
          break;
        case SDLK_d:
          tank_set_flags(red_player, 2, 2, 2, 0);
          break;
      }
    }

    if (green_player != NULL) {
      switch (key) {
        case SDLK_i:
          tank_set_flags(green_player, 0, 2, 2, 2);
          break;
        case SDLK_k:
          tank_set_flags(green_player, 2, 0, 2, 2);
          break;
        case SDLK_j:
          tank_set_flags(green_player, 2, 2, 0, 2);
          break;
        case SDLK_l:
          tank_set_flags(green_player, 2, 2, 2, 0);
          break;
      }
    }

    if (blue_player != NULL) {
      switch (key) {
        case UP_ARROW:
          tank_set_flags(blue_player, 0, 2, 2, 2);
          break;
        case DOWN_ARROW:
          tank_set_flags(blue_player, 2, 0, 2, 2);
          break;
        case LEFT_ARROW:
          tank_set_flags(blue_player, 2, 2, 0, 2);
          break;
        case RIGHT_ARROW:
          tank_set_flags(blue_player, 2, 2, 2, 0);
          break;
      }
    }            
  }
}

tank_t *tank_player_init_force(state_t *state, vector_t init_pos, rgb_color_t color) {
  tank_t *tank_body = tank_init(init_pos, TANK_SIZE, color);
  scene_add_body(state->scene, get_tank_body(tank_body));
  scene_add_body(state->scene, get_tank_hitbox(tank_body));
  add_tank_maze_force(state, state->maze, tank_body, TANK_SIZE);
  return tank_body;
}

void reset_state(state_t *state) {
  scene_free(state->scene);
  scene_t *scene = scene_init();
  state->scene = scene;

  state->count_down_until_next_game_start = 0;
  state->count_down_until_next_powerup = POWERUP_SPAWN_INTERVAL;
  state->tank_controls = 0;

  maze_free(state->maze);
  state->maze = maze_init(MAZE_COLUMNS, MAZE_ROWS, VEC_ZERO, WINDOW);
  for (size_t i = 0; i < list_size(state->maze->vertical_walls); i++) {
    scene_add_body(scene, list_get(state->maze->vertical_walls, i));
  }
  for (size_t i = 0; i < list_size(state->maze->horizontal_walls); i++) {
    scene_add_body(scene, list_get(state->maze->horizontal_walls, i));
  }

  list_t *random_vectors = get_random_cell_centers(state->maze, 3);
  state->red_player = tank_player_init_force(state, *(vector_t *)list_get(random_vectors, 0), RED_PLAYER_COLOR);
  state->blue_player = tank_player_init_force(state, *(vector_t *)list_get(random_vectors, 1), BLUE_PLAYER_COLOR);
  state->green_player = tank_player_init_force(state, *(vector_t *)list_get(random_vectors, 2), GREEN_PLAYER_COLOR);
  list_free(random_vectors);
  sdl_on_key(on_key);
  
  FILE *fptr = fopen("/tmp/savedat.txt", "w");
  fprintf(fptr, "%zu %zu %zu %zu", state->red_wins, state->blue_wins, state->green_wins, state->games_played);
  fclose(fptr);
}

void check_game_over(state_t *state) {
  bool red_null = state->red_player == NULL;
  bool blue_null = state->blue_player == NULL;
  bool green_null = state->green_player == NULL;
  
  bool red_win = !red_null && blue_null && green_null;
  bool blue_win = red_null && !blue_null && green_null;
  bool green_win = red_null && blue_null && !green_null;
  bool draw = red_null && blue_null && green_null;

  if (red_win || blue_win || green_win || draw) {
    printf("\n--------------------------\n");
    state->games_played = state->games_played + 1;
    state->count_down_until_next_game_start = COUNT_DOWN_NEXT_GAME;
    if (red_win) {
      printf("Red player wins!");
      state->red_wins = state->red_wins + 1;
    }
    if (blue_win) {
      printf("Blue player wins!");
      state->blue_wins = state->blue_wins + 1;
    }
    if (green_win) {
      printf("Green player wins!");
      state->green_wins = state->green_wins + 1;
    }
    if (draw) {
      printf("It's a draw!");
    }
    printf(" The next game will start in %zu seconds\n", (size_t)COUNT_DOWN_NEXT_GAME);
    printf("Current score:\nRed: %zu, Blue: %zu, Green: %zu\n", state->red_wins, state->blue_wins, state->green_wins);
    printf("--------------------------\n\n");
  }
}

void add_random_powerup(state_t *state) {
  double r = rand_num();
  if (r < POWERUP_SPAWN_PROB) {
    add_powerup(state, RAILGUN);
  } else if (r < 2*POWERUP_SPAWN_PROB) {
    add_powerup(state, LASER);
  } else if (r < 3*POWERUP_SPAWN_PROB) {
    add_powerup(state, SHOTGUN);
  } else {
    add_powerup(state, MOON);
  }
}

void run_cannon_event(state_t *state) {
  double r = rand_num();
  if (r < CANNON_EVENT_2099) {
    Mix_Chunk *empty_sound = Mix_LoadWAV("assets/cannon_event.wav");
    Mix_PlayChannel(-1, empty_sound, 0);
    state->tank_controls = (state->tank_controls + 1) % 2;
    if (state->red_player != NULL) {
      tank_set_flags(state->red_player, 0, 0, 0, 0);
    }
    if (state->green_player != NULL) {
      tank_set_flags(state->green_player, 0, 0, 0, 0);
    }
    if (state->blue_player != NULL) {
      tank_set_flags(state->blue_player, 0, 0, 0, 0);
    }
  }
}

state_t *emscripten_init() {
  scene_t *scene = scene_init();
  vector_t min = VEC_ZERO;
  vector_t max = WINDOW;
  sdl_init(min, max);
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene;
  state->count_down_until_next_game_start = 0;
  state->count_down_until_next_powerup = POWERUP_SPAWN_INTERVAL;
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  maze_t *maze = maze_init(MAZE_COLUMNS, MAZE_ROWS, VEC_ZERO, WINDOW);
  for (size_t i = 0; i < list_size(maze->vertical_walls); i++) {
    scene_add_body(scene, list_get(maze->vertical_walls, i));
  }
  for (size_t i = 0; i < list_size(maze->horizontal_walls); i++) {
    scene_add_body(scene, list_get(maze->horizontal_walls, i));
  }
  state->red_wins = 0;
  state->blue_wins = 0;
  state->green_wins = 0;
  state->games_played = 0;
  FILE *fptr = fopen("/savedat.txt", "r");
  if (fptr != NULL) {
    fscanf(fptr, "%zu %zu %zu %zu", &(state->red_wins), &(state->blue_wins), &(state->green_wins), &(state->games_played));
    fclose(fptr);
  }
  state->maze = maze;
  state->tank_controls = 0;
  list_t *random_vectors = get_random_cell_centers(maze, 3);
  state->red_player = tank_player_init_force(state, *(vector_t *)list_get(random_vectors, 0), RED_PLAYER_COLOR);
  state->blue_player = tank_player_init_force(state, *(vector_t *)list_get(random_vectors, 1), BLUE_PLAYER_COLOR);
  state->green_player = tank_player_init_force(state, *(vector_t *)list_get(random_vectors, 2), GREEN_PLAYER_COLOR);
  Mix_Music *music = Mix_LoadMUS("assets/soul_sanctum.ogg");
  Mix_PlayMusic(music, -1);
  list_free(random_vectors);
  sdl_on_key(on_key);
  printf("Welcome to Tank Trouble!\nThese are the controls:\nRed player: awsd + q     Green player: ijkl + u     Blue player: arrows + space bar\n");
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  state->dt = dt;
  if (state->count_down_until_next_game_start == 0) {
    check_game_over(state);
  } else if (state->count_down_until_next_game_start > 0) {
    state->count_down_until_next_game_start = state->count_down_until_next_game_start - dt;
    if (state->count_down_until_next_game_start <= 0) {
      reset_state(state);
    }
  }
  run_cannon_event(state);
  scene_tick(state->scene, dt);
  state->count_down_until_next_powerup = state->count_down_until_next_powerup - dt;
  if (state->count_down_until_next_powerup < 0) {
    add_random_powerup(state);
    state->count_down_until_next_powerup = POWERUP_SPAWN_INTERVAL;
  }
  if (state->red_player != NULL && state->red_player->exists == 0) {
    tank_remove(state->red_player, state);
    state->red_player = NULL;
  }
  if (state->green_player != NULL && state->green_player->exists == 0) {
    tank_remove(state->green_player, state);
    state->green_player = NULL;
  }
  if (state->blue_player != NULL && state->blue_player->exists == 0) {
    tank_remove(state->blue_player, state);
    state->blue_player = NULL;
  }
  if (state->red_player != NULL) {
    tank_execute_flags(state->red_player);
  }
  if (state->blue_player != NULL) {
    tank_execute_flags(state->blue_player);
  }
  if (state->green_player != NULL) {
    tank_execute_flags(state->green_player);
  }
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
