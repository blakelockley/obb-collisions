#ifndef OBB_H
#define OBB_H

#include "glfw.h"
#include "linmath.h"

struct _obb_t {
    vec3 center;
    vec3 half_side;
    vec3 axis[3];

    GLuint vao, vbo[2];

    mat4x4 model;
};

typedef struct _obb_t obb_t;

void init_obb(obb_t* obb);
void buffer_obb(obb_t* obb);
void roate_obb(obb_t* obb, float yaw, float pitch, float roll);
void draw_obb(obb_t* obb, int shader);
void free_obb(obb_t* obb);

#endif  // OBB_H