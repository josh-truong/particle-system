
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

#include "Callbacks.h"
#include "ParticleSystem.h"


int main(int argc, const char** argv)
{
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Particle System", NULL, NULL);    
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    ParticleSystem particle_trail;
    WindowInfo window_info;
    ParticleSettings settings;
    settings.velocity = glm::vec2(0.1f, 0.1f);
    settings.velocity_variation = glm::vec2(0.1f, 0.1f);
    settings.color_start = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    settings.color_finish = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    settings.size_begin = 0.15f;
    settings.size_finish = 0.05f;
    settings.size_variation = 0.05f;
    settings.lifespan = 2.0f;

    double xpos, ypos;
    float timestep = 0.01;
    while(!glfwWindowShouldClose(window)) {
        // Render commands
        glClearColor(0.188f, 0.188f, 0.188f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // ########################################################################## //
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if(state == GLFW_PRESS) {
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwGetWindowSize(window, &window_info.width, &window_info.height);

            settings.position = glm::vec2(xpos/(float)window_info.width - 1.0f, -ypos/(float)window_info.height + 1.0f);
            particle_trail.InitParticle(settings);
        }

        particle_trail.RenderParticle(window_info);
        particle_trail.UpdateParticle(timestep);
        // ########################################################################## //

        // Swap front buffer w/ back buffer
        glfwSwapBuffers(window);
        // Checks if any events has been triggered like key press
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}