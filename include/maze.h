#ifndef __MAZE_H__
#define __MAZE_H__

#include <stddef.h>
#include "scene.h"
#include "forces.h"
#include "body.h"

typedef struct cell {
  size_t x; 
  size_t y; 
} cell_t;

/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct maze {
  list_t *vertical_walls;
  list_t *horizontal_walls;
  size_t columns;
  size_t rows;
  vector_t lower_left;
  vector_t upper_right;
} maze_t;

double rand_num();

/**
 * Determines the center vector (position) of a given cell
 * 
 * @param maze the maze
 * @param cell a cell
 * @return the center position of the cell
*/
vector_t cell_to_vector(maze_t *maze, cell_t cell);

/**
 * Determines the cell position a vector is in
 * 
 * @param maze the maze
 * @param vector a vector
 * @return the corresponding cell
*/
cell_t vector_to_cell(maze_t *maze, vector_t vector);

/**
 * Translates a cell to a corresponding index (used for list_get)
 *
 * @param maze the maze
 * @param cell a cell
 * @return the corresponding index
*/
size_t cell_to_index(maze_t *maze, cell_t cell);

/**
 * Translates an index to a corresponding cell
 * 
 * @param maze the maze
 * @param index an int (size_t)
 * @return cell
*/
cell_t index_to_cell(maze_t *maze, size_t index);

/** 
 * Returns the indices in horizontal_walls and vertical_walls of walls around a given cell
 * 
 * @param maze the maze
 * @param cell the cell
 * @return list_t of bodies
 */
list_t *get_walls_around(maze_t *maze, cell_t cell);

/**
 * Allocates memory for a maze and creates random walls.
 *
 * @param columns the number of columns
 * @param rows the number of rows
 * @param lower_left the coordinate of lower left
 * @param upper_right the coordinate of the upper right
 * @return maze
 */
maze_t *maze_init(size_t columns, size_t rows, vector_t lower_left, vector_t upper_right);

/**
 * Releases memory allocated for a given maze
 * and all the connections and walls it contains.
 *
 * @param maze a pointer to a maze returned from maze_init()
 */
void maze_free(maze_t *maze);

/**
 * A function called when a collision occurs.
 * @param scene the scene
 * @param maze the maze (amazing I know)
 * @param body the body
 */
void add_maze_collision(scene_t *scene, maze_t *maze, body_t *body);

/**
 * Returns the center coordinate of a random cell in a given maze.
 * @param maze the maze
 * @return random coordinate
 */
vector_t get_random_cell_center(maze_t *maze);

/**
 * Returns the given number of center coordinates 
 * of a random cell in a given maze so that
 * none of them are overlapped.
 * @param maze the maze
 * @return list of vectors
 */
list_t *get_random_cell_centers(maze_t *maze, size_t num);

/** 
 * Checks if the given vector is outside of the maze
 * @param maze the maze
 * @param vector the coordinate
 * @return 1 if the vector is outside; 0 otherwise.
 */
bool check_outside(maze_t *maze, vector_t vector);

#endif // #ifndef __MAZE_H__
