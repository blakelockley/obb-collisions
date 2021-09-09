#include "obb.h"

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

    // clang-format off
    u_int32_t edges[15][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, 
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
        // Axis
        {8, 9}, {10, 11}, {12, 13},
    };
    // clang-format on

    glGenVertexArrays(1, &obb->vao);
    glBindVertexArray(obb->vao);

    glGenBuffers(1, &obb->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, obb->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);

    glGenBuffers(1, &obb->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obb->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
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
    glDeleteBuffers(1, &obb->ebo);
    glDeleteBuffers(1, &obb->vbo);
}