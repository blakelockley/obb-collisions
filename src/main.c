#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "collision.h"
#include "obb.h"
#include "shader.h"

GLFWwindow *window;

int paused = 0;

void init();
void deinit();

int main() {
    init();

    char title[16];
    int frames = 0;

    double play_time = 0.0f;
    double previous_time = 0.0f, last_second = 0.0f;

    obb_t obb1, obb2;
    init_obb(&obb1);
    init_obb(&obb2);

    int shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        double delta = current_time - previous_time;
        previous_time = current_time;

        if (!paused)
            play_time += delta;

        frames++;
        if (current_time - last_second > 1.0) {
            double fps = frames / (current_time - last_second);

            sprintf(title, "FPS: %.2f", fps);
            glfwSetWindowTitle(window, title);

            frames = 0;
            last_second = current_time;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 view, projection;
        mat4x4_look_at(view, (vec3){0, 0, 5}, (vec3){0, 0, 0}, (vec3){0, 1, 0});
        mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

        GLint view_loc = glGetUniformLocation(shader, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

        GLint projection_loc = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

        if (!paused) {
            float t = play_time * 0.5f;

            position_obb(&obb1, -1, cos(t), 0);
            rotate_obb(&obb1, cos(t) * 2, cos(t) * 2, 0);

            position_obb(&obb2, sin(t) * 2, 0, 0);
            rotate_obb(&obb2, sin(t) * 2, sin(t) * 2, 0);

            obb1.has_collision = obb2.has_collision = check_collision(&obb1, &obb2);
        }

        draw_obb(&obb1, shader);
        draw_obb(&obb2, shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_obb(&obb1);
    free_obb(&obb2);

    glDeleteProgram(shader);

    deinit();
    return EXIT_SUCCESS;
}

void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        paused = !paused;
}

void init() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "GLFW Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    // OpenGL setup

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(8.0f);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void deinit() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
