#include "obb.h"

void init_obb(obb_t *obb) {
    memcpy(&obb->center, (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
    memcpy(&obb->half_side, (vec3){0.5, 0.5, 0.5}, sizeof(vec3));

    obb->has_collision = 0;

    mat4x4_identity(obb->rotation);
    update_obb(obb);

    // clang-format off
    u_int32_t edges[15][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, 
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
        // Axis
        {8, 9}, {8, 10}, {8, 11},
    };
    // clang-format on

    glGenVertexArrays(1, &obb->vao);
    glBindVertexArray(obb->vao);

    // Edges
    glGenBuffers(1, &obb->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obb->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);

    // Vertices
    glGenBuffers(1, &obb->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, obb->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 12, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer
}

void position_obb(obb_t *obb, float x, float y, float z) {
    obb->center[0] = x;
    obb->center[1] = y;
    obb->center[2] = z;
}

void resize_obb(obb_t *obb, float x, float y, float z) {
    obb->half_side[0] = x;
    obb->half_side[1] = y;
    obb->half_side[2] = z;
}

void rotate_obb(obb_t *obb, float yaw, float pitch, float roll) {
    mat4x4_identity(obb->rotation);
    mat4x4_rotate_Y(obb->rotation, obb->rotation, yaw);
    mat4x4_rotate_X(obb->rotation, obb->rotation, pitch);
    mat4x4_rotate_Z(obb->rotation, obb->rotation, roll);
}

void update_vector(obb_t *obb, float *vector) {
    vec4 result, temp;

    memcpy(temp, vector, sizeof(vec3));
    temp[3] = 1.0f;

    mat4x4_mul_vec4(result, obb->rotation, temp);
    vector[0] = obb->center[0] + result[0];
    vector[1] = obb->center[1] + result[1];
    vector[2] = obb->center[2] + result[2];
}

void update_obb(obb_t *obb) {
    // Rotate each axis
    memcpy(obb->axis[0], (vec3){1.0f, 0.0f, 0.0f}, sizeof(vec3));
    memcpy(obb->axis[1], (vec3){0.0f, 1.0f, 0.0f}, sizeof(vec3));
    memcpy(obb->axis[2], (vec3){0.0f, 0.0f, 1.0f}, sizeof(vec3));

    for (int i = 0; i < 3; i++)
        update_vector(obb, obb->axis[i]);

    // Rotate each corner
    float *hs = obb->half_side;
    vec3 corners[8] = {
        {0 - hs[0], 0 - hs[1], 0 - hs[2]},
        {0 + hs[0], 0 - hs[1], 0 - hs[2]},
        {0 + hs[0], 0 + hs[1], 0 - hs[2]},
        {0 - hs[0], 0 + hs[1], 0 - hs[2]},
        {0 - hs[0], 0 - hs[1], 0 + hs[2]},
        {0 + hs[0], 0 - hs[1], 0 + hs[2]},
        {0 + hs[0], 0 + hs[1], 0 + hs[2]},
        {0 - hs[0], 0 + hs[1], 0 + hs[2]},
    };

    memcpy(obb->corners, corners, sizeof(corners));

    for (int i = 0; i < 8; i++)
        update_vector(obb, obb->corners[i]);
}

void buffer_obb(obb_t *obb) {
    vec3 vertices[12];
    memcpy(vertices, obb->corners, sizeof(obb->corners));

    memcpy(vertices + 8, obb->center, sizeof(vec3));
    memcpy(vertices + 9, obb->axis[0], sizeof(vec3));
    memcpy(vertices + 10, obb->axis[1], sizeof(vec3));
    memcpy(vertices + 11, obb->axis[2], sizeof(vec3));

    }

    glBindBuffer(GL_ARRAY_BUFFER, obb->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void draw_obb(obb_t *obb, int shader) {
    update_obb(obb);
    buffer_obb(obb);

    GLint color_loc = glGetUniformLocation(shader, "color");
    glBindVertexArray(obb->vao);

    if (obb->has_collision)
        glUniform3f(color_loc, 1.0f, 0.75f, 0.0f);
    else
        glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);

    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void *)0);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 24));

    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 26));

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 28));
}

void free_obb(obb_t *obb) {
    glDeleteVertexArrays(1, &obb->vao);
    glDeleteBuffers(1, &obb->vbo);
    glDeleteBuffers(1, &obb->ebo);
}