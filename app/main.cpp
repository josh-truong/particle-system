#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include <cstdio>
#include <math.h>
#include "ParticleSystem.h"
#include "Shader.h"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
    if( action != GLFW_PRESS )
        return;

    switch(key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        default:
            return;
    }
}

/** framebuffer_size_callback is a callback function for glfwSetFramebufferSizeCallback.
 * This callback function should be called the moment a user resizes the window and the
 * viewport needs to be adjusted.
 * 
*/
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    /* 
        glViewport: Sends to open opengl the size of our rendering window in
        order to display the data and coordinates with respect to the window.
    */
    glViewport(0, 0, width, height);
}


int main(int argc, const char** argv)
{
    // ParticleSystem parti;
    // parti.RenderParticle();


    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

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

    const char* vertexPath = "/home/ubuntu/Documents/Github/particle-system/assets/ShaderCode/vertex.glsl";
    const char* fragmentPath = "/home/ubuntu/Documents/Github/particle-system/assets/ShaderCode/fragment.glsl";
    Shader particleShader;
    particleShader.InitShader(vertexPath, fragmentPath);

    float vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    float life = 0.0;
    glBindVertexArray(VAO);
    while(!glfwWindowShouldClose(window)) {
        // Render commands
        glClearColor(0.188f, 0.188f, 0.188f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 
            Result of linking shaders is a program object.
            Every shader and rendering call after glUseProgram will now use this program object.
        */
        particleShader.Use();
        float timeValue = glfwGetTime();
        life = (sin(timeValue) / 2.0f) + 0.5f;
        glm::vec3 color = glm::lerp(glm::vec3(1.0f, 0.0f, 0.0f),
                                    glm::vec3(0.0f, 1.0f, 0.0f),
                                    life);
        unsigned int vertexColorLocation = glGetUniformLocation(particleShader.ID, "particleColor");
        glUniform3f(vertexColorLocation, color.x, color.y, color.z);


        for(int i = 0; i < 10; i++) {
            glm::mat4 transform = glm::mat4(1.0f);
            float xpos = -1.0f + (float)i/10.0f;
            float ypos = 0.0f;
            transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f))
                        * glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, 0.0f))
                        * glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            unsigned int transformLoc = glGetUniformLocation(particleShader.ID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
            
            
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Deallocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}