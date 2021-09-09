#ifndef OBB_H
#define OBB_H

#include "glfw.h"
#include "linmath.h"

struct _obb_t {
    vec3 center;
    vec3 half_side;
    vec3 axis[3];

    GLuint vao, vbo, ebo;
};

typedef struct _obb_t obb_t;

void buffer_obb(obb_t* obb);
void draw_obb(obb_t* obb, int shader);
void free_obb(obb_t* obb);

#endif  // OBB_H