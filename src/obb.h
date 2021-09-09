#ifndef OBB_H
#define OBB_H

#include "glfw.h"
#include "linmath.h"

struct _obb_t {
    vec3 center;
    vec3 half_side;
    vec3 axis[3];

    mat4x4 rotation;
    vec3 corners[8];

    int has_collision;

    GLuint vao, vbo[2];
};

typedef struct _obb_t obb_t;

void init_obb(obb_t* obb);
void position_obb(obb_t* obb, float x, float y, float z);
void resize_obb(obb_t* obb, float x, float y, float z);
void rotate_obb(obb_t* obb, float yaw, float pitch, float roll);
void update_obb(obb_t* obb);
void draw_obb(obb_t* obb, int shader);
void free_obb(obb_t* obb);

#endif  // OBB_H