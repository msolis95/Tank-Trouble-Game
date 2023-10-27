#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "maze.h"
#include "collision.h"
#include <time.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0

typedef struct wall {
  size_t vertex1_index;
  size_t vertex2_index;
} wall_vertices_t;

// maze constants
const double WALL_THICKNESS = 6;
const size_t MINUS_ONE = -1;

void rand_seed_init() {
  srand((unsigned int)time(NULL));
}
double rand_num() { 
  return ((double)rand()) / RAND_MAX; 
}

size_t list_find_size(list_t *list, size_t* n) {
  for (size_t i = 0; i < list_size(list); i++) {
    if (*(size_t *)list_get(list, i) == *n) {
      return i;
    }
  }
  return MINUS_ONE;
}

size_t list_find_wall_vertices(list_t *list, wall_vertices_t *wall) {
 for (size_t i = 0; i < list_size(list); i++) {
  wall_vertices_t picked_wall = *(wall_vertices_t *)list_get(list, i);
   if (picked_wall.vertex1_index == (*wall).vertex1_index 
      && picked_wall.vertex2_index == (*wall).vertex2_index) {
     return i;
   }
 }
 return MINUS_ONE;
}

vector_t cell_to_vector(maze_t *maze, cell_t cell) {
  vector_t lower_left = maze->lower_left;
  vector_t upper_right = maze->upper_right;
  size_t columns = maze->columns;
  size_t rows = maze->rows;
  double edge_horizontal = (upper_right.x - lower_left.x) / columns;
  double edge_vertical = (upper_right.y - lower_left.y) / rows;
  double x = cell.x * edge_horizontal + (edge_horizontal / 2.) + lower_left.x;
  double y = cell.y * edge_vertical + (edge_vertical / 2.) + lower_left.y;
  assert(lower_left.x <= x && x <= upper_right.x);
  assert(lower_left.y <= y && y <= upper_right.y);
  return (vector_t) { .x = x, .y = y };
}

bool check_outside(maze_t *maze, vector_t vector) {
  vector_t lower_left = maze->lower_left;
  vector_t upper_right = maze->upper_right;
  if ((lower_left.x <= vector.x && vector.x <= upper_right.x) && (lower_left.y <= vector.y && vector.y <= upper_right.y)) {
   return 0;
  }
  return 1;
}

cell_t vector_to_cell(maze_t *maze, vector_t vector) {
  vector_t lower_left = maze->lower_left;
  vector_t upper_right = maze->upper_right;
  vector_t true_upper_right = vec_subtract(upper_right, lower_left);
  vector_t true_vector = vec_subtract(vector, lower_left);
  size_t columns = maze->columns;
  size_t rows = maze->rows;
  double edge_horizontal = true_upper_right.x / columns;
  double edge_vertical =  true_upper_right.y / rows;
  size_t x = (size_t) floor(true_vector.x / edge_horizontal);
  size_t y = (size_t) floor(true_vector.y / edge_vertical);
  return (cell_t) {.x = x, .y = y};
}

size_t cell_to_index(maze_t *maze, cell_t cell) {
  size_t columns = maze->columns;
  size_t rows = maze->rows;
  assert(0 <= cell.x && cell.x < columns);
  assert(0 <= cell.y && cell.y < rows);
  return cell.x + columns * cell.y;
}

cell_t index_to_cell(maze_t *maze, size_t index) {
  size_t columns = maze->columns;
  size_t rows = maze->rows;
  assert(0 <= index && index < columns * rows);
  size_t x = index % columns;
  return (cell_t) { .x = x, .y = (index - x) / columns};
}

list_t *get_walls_around(maze_t *maze, cell_t cell) {
  list_t *walls_around = list_init(4, NULL);
  size_t first_wall_index_horizontal = (maze->rows + 1) * cell.x + cell.y;
  size_t first_wall_index_vertical = (maze->columns + 1) * cell.y + cell.x;
  list_add(walls_around, list_get(maze->vertical_walls, first_wall_index_vertical));
  list_add(walls_around, list_get(maze->vertical_walls, first_wall_index_vertical + 1));
  list_add(walls_around, list_get(maze->horizontal_walls, first_wall_index_horizontal));
  list_add(walls_around, list_get(maze->horizontal_walls, first_wall_index_horizontal + 1));
  return walls_around;
}

typedef struct vertex {
  size_t i; // corresponds to y
  size_t j; // corresponds to x
} vertex_t;

vector_t vertex_to_vector_helper(size_t columns, size_t rows, vector_t lower_left, vector_t upper_right, vertex_t vertex) {
  double edge_horizontal = (upper_right.x - lower_left.x) / columns;
  double edge_vertical = (upper_right.y - lower_left.y) / rows;
  double x = vertex.j * edge_horizontal + lower_left.x;
  double y = vertex.i * edge_vertical + lower_left.y;
  assert(lower_left.x <= x && x <= upper_right.x);
  assert(lower_left.y <= y && y <= upper_right.y);
  return (vector_t) { .x = x, .y = y };
}
vector_t vertex_to_vector(maze_t *maze, vertex_t vertex) {
  vector_t lower_left = maze->lower_left;
  vector_t upper_right = maze->upper_right;
  size_t columns = maze->columns;
  size_t rows = maze->rows;
  return vertex_to_vector_helper(columns, rows, lower_left, upper_right, vertex);
}

vertex_t vector_to_vertex(maze_t *maze, vector_t vector) {
  vector_t lower_left = maze->lower_left;
  vector_t upper_right = maze->upper_right;
  vector_t true_upper_right = vec_subtract(upper_right, lower_left);
  vector_t true_vector = vec_subtract(vector, lower_left);
  size_t columns = maze->columns;
  size_t rows = maze->rows;
  double edge_horizontal = true_upper_right.x / columns;
  double edge_vertical =  true_upper_right.y / rows;
  size_t j = (size_t) floor(true_vector.x / edge_horizontal);
  size_t i = (size_t) floor(true_vector.y / edge_vertical);
  assert(lower_left.x <= vector.x && vector.x <= upper_right.x);
  assert(lower_left.y <= vector.y && vector.y <= upper_right.y);
  return (vertex_t) {.i = i, .j = j};
}

size_t vertex_to_index_helper(size_t columns, size_t rows, vertex_t vertex) {
  assert(0 <= vertex.j && vertex.j < columns + 1);
  assert(0 <= vertex.i && vertex.i < rows + 1);
  return vertex.j + (columns + 1) * vertex.i;
}
size_t vertex_to_index(maze_t *maze, vertex_t vertex) {
  return vertex_to_index_helper(maze->columns, maze->rows, vertex);
}

vertex_t index_to_vertex_helper(size_t columns, size_t rows, size_t index) {
  assert(0 <= index && index < (columns + 1) * (rows + 1));
  size_t j = index % (columns + 1);
  return (vertex_t) { .j = j, .i = (index - j) / (columns + 1) };
}
vertex_t index_to_vertex(maze_t *maze, size_t index) {
  return index_to_vertex_helper(maze->columns, maze->rows, index);
}

list_t *walls_index_init(size_t columns, size_t rows) {
  list_t *walls = list_init((columns + 1) * rows + columns * (rows + 1), free);
  
  vertex_t vertex1;
  vertex_t vertex2;
  for (size_t j = 0; j < columns; j++) {
    vertex1 = (vertex_t) {.i = 0, .j = j};
    vertex2 = (vertex_t) {.i = 0, .j = j + 1};
    wall_vertices_t *wall1 = malloc(sizeof(wall_vertices_t));
    *wall1 = (wall_vertices_t) {
      .vertex1_index = vertex_to_index_helper(columns, rows, vertex1), 
      .vertex2_index = vertex_to_index_helper(columns, rows, vertex2)
    };
    list_add(walls, wall1);
    vertex1 = (vertex_t) {.i = rows, .j = j};
    vertex2 = (vertex_t) {.i = rows, .j = j + 1};
    wall_vertices_t *wall2 = malloc(sizeof(wall_vertices_t));
    *wall2 = (wall_vertices_t) {
      .vertex1_index = vertex_to_index_helper(columns, rows, vertex1), 
      .vertex2_index = vertex_to_index_helper(columns, rows, vertex2),
    };
    list_add(walls, wall2);
  }
  for (size_t i = 0; i < rows; i++) {
    vertex1 = (vertex_t) {.i = i, .j = 0};
    vertex2 = (vertex_t) {.i = i + 1, .j = 0};
    wall_vertices_t *wall3 = malloc(sizeof(wall_vertices_t));
    *wall3 = (wall_vertices_t) {
      .vertex1_index = vertex_to_index_helper(columns, rows, vertex1), 
      .vertex2_index = vertex_to_index_helper(columns, rows, vertex2)
    };
    list_add(walls, wall3);
    vertex1 = (vertex_t) {.i = i, .j = columns};
    vertex2 = (vertex_t) {.i = i + 1, .j = columns};
    wall_vertices_t *wall4 = malloc(sizeof(wall_vertices_t));
    *wall4 = (wall_vertices_t) {
      .vertex1_index = vertex_to_index_helper(columns, rows, vertex1), 
      .vertex2_index = vertex_to_index_helper(columns, rows, vertex2),
    };
    list_add(walls, wall4);
  }
  return walls;
}

list_t *nodes_index_init(columns, rows) {
  list_t *nodes = list_init((columns + 1) * (rows + 1), free);
  for (size_t i = 1; i < rows; i++) {
    for (size_t j = 1; j < columns; j++) {
      size_t *index = malloc(sizeof(size_t));
      *index = vertex_to_index_helper(columns, rows, (vertex_t) {.i = i, .j = j});
      list_add(nodes, index);
    }
  }
  return nodes;
}

list_t *get_neighbors_vertex_index(size_t columns, size_t rows, size_t index) {
  list_t *neighbors_index = list_init(4, free);
  vertex_t vertex = index_to_vertex_helper(columns, rows, index);
  if (0 <= vertex.j - 1) {
    vertex_t left = (vertex_t) {.i = vertex.i, .j = vertex.j - 1};
    size_t *index = malloc(sizeof(size_t));
    *index = vertex_to_index_helper(columns, rows, left);
    list_add(neighbors_index, index);
  }
  if (vertex.j + 1 <= columns) {
    vertex_t right = (vertex_t) {.i = vertex.i, .j = vertex.j + 1};
    size_t *index = malloc(sizeof(size_t));
    *index = vertex_to_index_helper(columns, rows, right);
    list_add(neighbors_index, index);
  }
  if (0 <= vertex.i - 1) {
    vertex_t down = (vertex_t) {.i = vertex.i - 1, .j = vertex.j};
    size_t *index = malloc(sizeof(size_t));
    *index = vertex_to_index_helper(columns, rows, down);
    list_add(neighbors_index, index);
  }
  if (vertex.i + 1 <= rows) {
    vertex_t up = (vertex_t) {.i = vertex.i + 1, .j = vertex.j};
    size_t *index = malloc(sizeof(size_t));
    *index = vertex_to_index_helper(columns, rows, up);
    list_add(neighbors_index, index);
  }
  return neighbors_index;
}

list_t *random_walls_index(size_t columns, size_t rows) {
  list_t *walls = walls_index_init(columns, rows);
  list_t *nodes = nodes_index_init(columns, rows);
  list_t *path = list_init((columns + 1) * (rows + 1), NULL);
  
  while (list_size(nodes) > 0) {
    list_add(path, list_remove(nodes, floor(rand_num() * list_size(nodes))));  
    size_t wall_neighbor_index = MINUS_ONE;
    size_t valid_neighbor_index = MINUS_ONE;
    while (wall_neighbor_index == MINUS_ONE) {
      list_t *neighbors_index = get_neighbors_vertex_index(columns, rows, *(size_t *)list_get(path, list_size(path) - 1));
      while (list_size(neighbors_index) > 0) {
        size_t *neighbor_index = (size_t *)list_remove(neighbors_index, floor(rand_num() * list_size(neighbors_index)));
        size_t nodes_index_neighbor = list_find_size(nodes, neighbor_index);
        size_t path_index_neighbor = list_find_size(path, neighbor_index);
        bool neighbor_in_nodes = (nodes_index_neighbor != MINUS_ONE);
        bool neighbor_in_path = (path_index_neighbor != MINUS_ONE);
        bool neighbor_is_wall = !(neighbor_in_nodes || neighbor_in_path);
        if (neighbor_is_wall) {
          wall_neighbor_index = *neighbor_index;
          break;
        } else if (neighbor_in_nodes) {
          valid_neighbor_index = *neighbor_index;
          break;
        }
      }
      if (wall_neighbor_index == MINUS_ONE) {
        if (valid_neighbor_index != MINUS_ONE) {
          size_t *valid_neighbor = list_remove(nodes, list_find_size(nodes, &valid_neighbor_index));
          list_add(path, valid_neighbor);
        } else {
          list_add(nodes, list_remove(path, list_size(path) - 1));
        }
      } else {
        size_t *wall_neighbor = malloc(sizeof(size_t));
        *wall_neighbor = wall_neighbor_index;
        list_add(path, wall_neighbor);
      }
    }
    for (size_t k = 0; k < list_size(path) - 1; k++) {
      wall_vertices_t *wall = malloc(sizeof(wall_vertices_t));
      *wall = (wall_vertices_t) { .vertex1_index = *(size_t *)list_get(path, k), .vertex2_index = *(size_t *)list_get(path, k+1)};
      list_add(walls, wall);
    }
    list_clear(path);
  }
  list_free(nodes);
  list_free(path);
  return walls;
}

body_t *wall_init(
  size_t columns, size_t rows, vector_t lower_left, vector_t upper_right,
  size_t vertex1_index, size_t vertex2_index) {
  double WALL_C = WALL_THICKNESS / 2;
  vertex_t vertex1 = index_to_vertex_helper(columns, rows, vertex1_index);
  vertex_t vertex2 = index_to_vertex_helper(columns, rows, vertex2_index);
  vector_t vector1 = vertex_to_vector_helper(columns, rows, lower_left, upper_right, vertex1);
  vector_t vector2 = vertex_to_vector_helper(columns, rows, lower_left, upper_right, vertex2);
  
  vector_t *wallTL = malloc(sizeof(vector_t));
  vector_t *wallTR = malloc(sizeof(vector_t));
  vector_t *wallBL = malloc(sizeof(vector_t));
  vector_t *wallBR = malloc(sizeof(vector_t));
  
  if (vector1.x == vector2.x) {
    bool vector1_is_above = vector1.y > vector2.y;
    *wallTL = (vector_t){.x = vector1.x - WALL_C, .y = vector1.y + (vector1_is_above * 2 - 1) * WALL_C};
    *wallTR = (vector_t){.x = vector1.x + WALL_C, .y = vector1.y + (vector1_is_above * 2 - 1) * WALL_C};
    *wallBL = (vector_t){.x = vector1.x - WALL_C, .y = vector2.y - (vector1_is_above * 2 - 1) * WALL_C};
    *wallBR = (vector_t){.x = vector1.x + WALL_C, .y = vector2.y - (vector1_is_above * 2 - 1) * WALL_C};
  } else {
    bool vector1_is_right = vector1.x > vector2.x;
    *wallTL = (vector_t){.x = vector1.x  + (vector1_is_right * 2 - 1) * WALL_C, .y = vector1.y + WALL_C};
    *wallTR = (vector_t){.x = vector2.x  - (vector1_is_right * 2 - 1) * WALL_C, .y = vector1.y + WALL_C};
    *wallBL = (vector_t){.x = vector1.x  + (vector1_is_right * 2 - 1) * WALL_C, .y = vector1.y - WALL_C };
    *wallBR = (vector_t){.x = vector2.x  - (vector1_is_right * 2 - 1) * WALL_C, .y = vector2.y - WALL_C};
  }

  list_t *wall_shape = list_init(4, free);
  list_add(wall_shape, wallTL);
  list_add(wall_shape, wallTR);
  list_add(wall_shape, wallBR);
  list_add(wall_shape, wallBL);
  body_t *wall = body_init_with_info(wall_shape, INFINITY, (rgb_color_t) { .r = 0, .g = 0, .b = 0 }, (void *)1, NULL, 1);
  return wall;
}

body_t *null_wall_init() {
  list_t *wall_shape = list_init(4, free);
  vector_t *vec1 = malloc(sizeof(vector_t));
  *vec1 = (vector_t){.x = 0, .y = 0};
  list_add(wall_shape, vec1);
  vector_t *vec2 = malloc(sizeof(vector_t));
  *vec2 = (vector_t){.x = 1, .y = 0};
  list_add(wall_shape, vec2);
  vector_t *vec3 = malloc(sizeof(vector_t));
  *vec3 = (vector_t){.x = 1, .y = 1};
  list_add(wall_shape, vec3);
  vector_t *vec4 = malloc(sizeof(vector_t));
  *vec4 = (vector_t){.x = 0, .y = 1};
  list_add(wall_shape, vec4);
  
  body_t *wall = body_init_with_info(wall_shape, INFINITY, (rgb_color_t) { .r = 0, .g = 0, .b = 0 }, (void *)0, NULL, 0);
  return wall;
}
        
list_t *vertical_walls_init(
  size_t columns, size_t rows, vector_t lower_left, vector_t upper_right, list_t *walls_indices) {
  list_t *vertical_walls = list_init(rows * (columns + 1), NULL); // should this be free?
  wall_vertices_t *wall1 = malloc(sizeof(wall_vertices_t));
  wall_vertices_t *wall2 = malloc(sizeof(wall_vertices_t));
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < columns + 1; j++) {
      size_t vertex1_index = vertex_to_index_helper(columns, rows, (vertex_t) { .i = i, .j = j });
      size_t vertex2_index = vertex_to_index_helper(columns, rows, (vertex_t) { .i = i + 1, .j = j });
      *wall1 = (wall_vertices_t) { .vertex1_index = vertex1_index, .vertex2_index = vertex2_index };
      *wall2 = (wall_vertices_t) { .vertex1_index = vertex2_index, .vertex2_index = vertex1_index };
      if (list_find_wall_vertices(walls_indices, wall1) == MINUS_ONE && list_find_wall_vertices(walls_indices, wall2) == MINUS_ONE) {
        body_t *null_wall = null_wall_init();
        list_add(vertical_walls, null_wall);
      } else {
        body_t *wall = wall_init(columns, rows, lower_left, upper_right, vertex1_index, vertex2_index);
        list_add(vertical_walls, wall);
      }
    }
  }
  return vertical_walls;
}

list_t *horizontal_walls_init(
  size_t columns, size_t rows, vector_t lower_left, vector_t upper_right, list_t *walls_index) {
  list_t *horizontal_walls = list_init((rows + 1) * columns, NULL); // should this be free?
  wall_vertices_t *wall1 = malloc(sizeof(wall_vertices_t));
  wall_vertices_t *wall2 = malloc(sizeof(wall_vertices_t));
  for (size_t j = 0; j < columns; j++) {
    for (size_t i = 0; i < rows + 1; i++) {
      size_t vertex1_index = vertex_to_index_helper(columns, rows, (vertex_t) { .i = i, .j = j });
      size_t vertex2_index = vertex_to_index_helper(columns, rows, (vertex_t) { .i = i, .j = j + 1 });
      *wall1 = (wall_vertices_t) { .vertex1_index = vertex1_index, .vertex2_index = vertex2_index };
      *wall2 = (wall_vertices_t) { .vertex1_index = vertex2_index, .vertex2_index = vertex1_index };
      if (list_find_wall_vertices(walls_index, wall1) == MINUS_ONE && list_find_wall_vertices(walls_index, wall2) == MINUS_ONE) {
        body_t *null_wall = null_wall_init();
        list_add(horizontal_walls, null_wall);
      } else {
        body_t *wall = wall_init(columns, rows, lower_left, upper_right, vertex1_index, vertex2_index);
        list_add(horizontal_walls, wall);
      }
    }
  }
  return horizontal_walls;
}

maze_t *maze_init(size_t columns, size_t rows, vector_t lower_left, vector_t upper_right) {
  maze_t *maze = malloc(sizeof(maze_t));
  maze->columns = columns;
  maze->rows = rows;
  maze->lower_left = lower_left;
  maze->upper_right = upper_right;
  rand_seed_init();
  list_t *walls_index = random_walls_index(columns, rows);
  maze->vertical_walls = vertical_walls_init(columns, rows, lower_left, upper_right, walls_index);
  maze->horizontal_walls = horizontal_walls_init(columns, rows, lower_left, upper_right, walls_index);
  return maze;
}

void maze_free(maze_t *maze) {
  list_free(maze->vertical_walls);
  list_free(maze->horizontal_walls);
  free(maze);
}

typedef struct body_maze_bool {
  body_t *body;
  maze_t *maze;
  bool bol;
} body_maze_bool_t;

void amazing_force(void *aux) {
  body_t *body = ((body_maze_bool_t *)aux)->body;
  maze_t *maze = ((body_maze_bool_t *)aux)->maze;
  bool previously_collided = ((body_maze_bool_t *)aux)->bol;
  cell_t position = vector_to_cell(maze, body_get_center(body));
  if (check_outside(maze, body_get_center(body)) == 1) {
      body_remove(body);
      return;
  }
  list_t *walls_around = get_walls_around(maze, position);
  bool at_least_one_wall_collided = FALSE;
  list_t *collision_info_list = list_init(4, free);
  for (size_t i = 0; i < list_size(walls_around); i++) {
    body_t *wall = list_get(walls_around, i);
    collision_info_t *collision_info = malloc(sizeof(collision_info_t)); 
    *collision_info = find_collision(body->shape, wall->shape);
    if (collision_info->collided) {
      at_least_one_wall_collided = TRUE;
      list_add(collision_info_list, collision_info);
    }
  }

  if (previously_collided) {
    if (at_least_one_wall_collided == FALSE) {
      ((body_maze_bool_t *)aux)->bol = FALSE;
    }
    list_free(collision_info_list);
    list_free(walls_around);
    return;
  }
  if (at_least_one_wall_collided) {
    for (size_t i = 0; i < list_size(collision_info_list); i++) {
      body_t *wall = list_get(walls_around, i);
      collision_info_t *collision_info = list_get(collision_info_list, i);
      if (collision_info->collided) {
        ((body_maze_bool_t *)aux)->bol = TRUE;
        double *aux_nature = malloc(sizeof(double));
        *aux_nature = 1.0;
        collision_of_nature_handler(body, wall, collision_info->axis, (void *)aux_nature);
        free(aux_nature);
      }
    }
  }
  list_free(walls_around);
  list_free(collision_info_list);
}

void add_maze_collision(scene_t *scene, maze_t *maze, body_t *body) {
  body_maze_bool_t *aux = malloc(sizeof(body_maze_bool_t));
  aux->body = body;
  aux->maze = maze;
  aux->bol = FALSE;
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, amazing_force, aux, bodies, free);
}

vector_t get_random_cell_center(maze_t *maze) {
  size_t index = (maze->rows * maze->columns) * rand_num();
  return cell_to_vector(maze, index_to_cell(maze, index));
}

size_t list_find_vector(list_t *list, vector_t* v) {
  for (size_t i = 0; i < list_size(list); i++) {
    vector_t vi = *(vector_t *)list_get(list, i);
    if (vi.x == v->x && vi.y == v->y) {
      return i;
    }
  }
  return MINUS_ONE;
}
list_t *get_random_cell_centers(maze_t *maze, size_t num) {
  list_t *random_vectors = list_init(num, free);
  while (list_size(random_vectors) < num) {
    size_t index = (maze->rows * maze->columns) * rand_num();
    vector_t *center = malloc(sizeof(vector_t));
    *center = cell_to_vector(maze, index_to_cell(maze, index));
    if (list_find_vector(random_vectors, center) == MINUS_ONE) {
      list_add(random_vectors, center);
    } else {
      free(center);
    }
  }
  return random_vectors;
}
