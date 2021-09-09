#include "obb.h"

void init_obb(obb_t *obb) {
    memcpy(&obb->center, (vec3){0.0f, 0.0f, 0.0f}, sizeof(vec3));
    memcpy(&obb->axis[0], (vec3){1.0f, 0.0f, 0.0f}, sizeof(vec3));
    memcpy(&obb->axis[1], (vec3){0.0f, 1.0f, 0.0f}, sizeof(vec3));
    memcpy(&obb->axis[2], (vec3){0.0f, 0.0f, 1.0f}, sizeof(vec3));

    float s = 0.5f;
    memcpy(&obb->half_side, (vec3){s, s, s}, sizeof(vec3));

    mat4x4_identity(obb->model);

    glGenVertexArrays(1, &obb->vao);
    glBindVertexArray(obb->vao);

    glGenBuffers(2, obb->vbo);

    // Edges

    // clang-format off
    u_int32_t edges[15][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, 
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
        // Axis
        {8, 9}, {10, 11}, {12, 13},
    };
    // clang-format on

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obb->vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
}

void buffer_obb(obb_t *obb) {
    float *o = obb->center;
    float *hs = obb->half_side;

    float *x_axis = obb->axis[0];
    float *y_axis = obb->axis[1];
    float *z_axis = obb->axis[2];

    vec3 vertices[14] = {
        {o[0] - hs[0], o[1] - hs[1], o[2] - hs[2]},
        {o[0] + hs[0], o[1] - hs[1], o[2] - hs[2]},
        {o[0] + hs[0], o[1] + hs[1], o[2] - hs[2]},
        {o[0] - hs[0], o[1] + hs[1], o[2] - hs[2]},
        {o[0] - hs[0], o[1] - hs[1], o[2] + hs[2]},
        {o[0] + hs[0], o[1] - hs[1], o[2] + hs[2]},
        {o[0] + hs[0], o[1] + hs[1], o[2] + hs[2]},
        {o[0] - hs[0], o[1] + hs[1], o[2] + hs[2]},
        // Axis
        {o[0] + x_axis[0], o[1] + x_axis[1], o[2] + x_axis[2]},
        {o[0] - x_axis[0], o[1] - x_axis[1], o[2] - x_axis[2]},
        {o[0] + y_axis[0], o[1] + y_axis[1], o[2] + y_axis[2]},
        {o[0] - y_axis[0], o[1] - y_axis[1], o[2] - y_axis[2]},
        {o[0] + z_axis[0], o[1] + z_axis[1], o[2] + z_axis[2]},
        {o[0] - z_axis[0], o[1] - z_axis[1], o[2] - z_axis[2]},
    };

    // Rotate corners by model matrix

    vec4 v, t;
    for (int i = 0; i < 8; i++) {
        float *vertex = vertices[i];

        memcpy(t, vertex, sizeof(vec3));
        t[3] = 1.0f;

        mat4x4_mul_vec4(v, obb->model, t);
        vertex[0] = v[0];
        vertex[1] = v[1];
        vertex[2] = v[2];
    }

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, obb->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void roate_obb(obb_t *obb, float yaw, float pitch, float roll) {
    mat4x4_identity(obb->model);

    mat4x4_rotate_Y(obb->model, obb->model, yaw);
    mat4x4_rotate_X(obb->model, obb->model, pitch);
    mat4x4_rotate_Z(obb->model, obb->model, roll);

    memcpy(obb->axis[0], (vec3){1.0f, 0.0f, 0.0f}, sizeof(vec3));
    memcpy(obb->axis[1], (vec3){0.0f, 1.0f, 0.0f}, sizeof(vec3));
    memcpy(obb->axis[2], (vec3){0.0f, 0.0f, 1.0f}, sizeof(vec3));

    // Rotate axis by model matrix

    vec4 v, t;
    for (int i = 0; i < 3; i++) {
        float *axis = obb->axis[i];

        memcpy(t, axis, sizeof(vec3));
        t[3] = 1.0f;

        mat4x4_mul_vec4(v, obb->model, t);
        axis[0] = v[0];
        axis[1] = v[1];
        axis[2] = v[2];
    }
}

void draw_obb(obb_t *obb, int shader) {
    GLint color_loc = glGetUniformLocation(shader, "color");
    glBindVertexArray(obb->vao);

    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void *)0);

    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
    glDrawElements(GL_LINES, 3, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 24));

    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    glDrawElements(GL_LINES, 3, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 26));

    glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 3, GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * 28));
}

void free_obb(obb_t *obb) {
    glDeleteVertexArrays(1, &obb->vao);
    glDeleteBuffers(2, obb->vbo);
}