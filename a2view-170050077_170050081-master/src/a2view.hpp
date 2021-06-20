#ifndef _A2VIEW_HPP_
#define _A2VIEW_HPP_

// Defining the ESCAPE Key Code
#define ESCAPE 27
// Defining the DELETE Key Code
#define DELETE 127

#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//#define NUM_BUFFERS 3

//257x257x3 lines, 2 points/line  => 396294 points.
#define GRID_SIZE 30
#define GRID_POINTS (GRID_SIZE+1)*(GRID_SIZE+1)*6
#define PLANE_POINTS (GRID_SIZE+1)*12
#define WIN_WIDTH 1024
#define WIN_HEIGHT 1024
#define NUM_POINTS_PER_CUBE 36

//-------------------------------------------------------------------------

#endif
