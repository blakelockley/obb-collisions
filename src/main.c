#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glfw.h"
#include "linmath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENGINE_INCLUDES
#include "obb.h"
#include "shader.h"

GLFWwindow *window;

void init();
void deinit();

int main() {
    init();

    char title[16];

    double last_second = 0;
    int frames = 0;

    obb_t obb1, obb2;

    init_obb(&obb1);
    resize_obb(&obb1, 0.5f, 0.25f, 0.25f);

    init_obb(&obb2);
    roate_obb(&obb2, 0, 0, M_PI_4);

    int shader = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();

        frames++;
        if (time - last_second > 1.0) {
            double fps = frames / (time - last_second);

            sprintf(title, "FPS: %.2f", fps);
            glfwSetWindowTitle(window, title);

            frames = 0;
            last_second = time;
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

        position_obb(&obb2, sin(time) * 2, 0, 0);


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
