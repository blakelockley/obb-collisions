#include "obb.h"

void buffer_obb(obb_t *obb) {
    float *o = obb->center;
    float *hs = obb->half_side;

    vec3 vertices[8] = {
        {o[0] - hs[0], o[1] - hs[1], o[2] - hs[2]},
        {o[0] + hs[0], o[1] - hs[1], o[2] - hs[2]},
        {o[0] + hs[0], o[1] + hs[1], o[2] - hs[2]},
        {o[0] - hs[0], o[1] + hs[1], o[2] - hs[2]},
        {o[0] - hs[0], o[1] - hs[1], o[2] + hs[2]},
        {o[0] + hs[0], o[1] - hs[1], o[2] + hs[2]},
        {o[0] + hs[0], o[1] + hs[1], o[2] + hs[2]},
        {o[0] - hs[0], o[1] + hs[1], o[2] + hs[2]},
    };

    // clang-format off
    u_int32_t edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, 
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
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

void draw_obb(obb_t *obb) {
    glBindVertexArray(obb->vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void *)0);
}

void free_obb(obb_t *obb) {
    glDeleteVertexArrays(1, &obb->vao);
    glDeleteBuffers(1, &obb->ebo);
    glDeleteBuffers(1, &obb->vbo);
}