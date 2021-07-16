#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <cstdio>
#include <math.h>
#include <stb_image.h>

#include "ParticleSystem.h"
#include "Shader.h"

float mixValue = 0.2;

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
        case GLFW_KEY_UP:
            (mixValue >= 1.0f) ? mixValue = 1.0f : mixValue += 0.1;
            break;
        case GLFW_KEY_DOWN:
            (mixValue <= 0.0f) ? mixValue = 0.0f : mixValue -= 0.1;
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
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = window = glfwCreateWindow(640, 480, "Particle System", NULL, NULL);    
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }
    int width, height;
    
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    // Missing glad



    const char* vertexPath = "/home/ubuntu/Documents/Github/particle-system/assets/ShaderCode/vertex.glsl";
    const char* fragmentPath = "/home/ubuntu/Documents/Github/particle-system/assets/ShaderCode/fragment.glsl";
    Shader ourShader;
    ourShader.InitShader(vertexPath, fragmentPath);



    // Setup vertex data, buffer, and configure vertex attributes
    // ------------------------------------------
    float vertices[] = {
        // Positions       // Colors         // texture coords
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    /* Vertex Buffer Object 
        Vertex data is now stored within the memory of the GPU and managed by variable VBO
    */
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 0. Bind VAO
    glBindVertexArray(VAO);

    // 1. bind and set vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /*  2. Set the vertex attributes pointers
        Input data is now sent to GPU and instructed the GPU how it should process the vertex
        data within a vertex and fragment shader. But OpenGL does not know how to interpret
        the vertex data in memory and how to connect the vertex data to the shader's attribute.
    */
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO
    glBindVertexArray(0);


    // ------------------------- //
    // load and create a texture //
    // ------------------------- //
    unsigned int texture1, texture2;
    // Texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int tex_width, tex_height, tex_nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("/home/ubuntu/Documents/Github/particle-system/assets/texture/container.jpg",
                                    &tex_width, &tex_height, &tex_nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);


    // Texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Minifying/magnifying filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("/home/ubuntu/Documents/Github/particle-system/assets/texture/awesomeface.png",
                                    &tex_width, &tex_height, &tex_nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);


    ourShader.Use();
    ourShader.SetInt1("texture1", 0);
    ourShader.SetInt1("texture2", 1);

    
    while(!glfwWindowShouldClose(window)) {
        // Render commands
        glClearColor(0.188f, 0.188f, 0.188f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* 
            Result of linking shaders is a program object.
            Every shader and rendering call after glUseProgram will now use this program object.
        */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.SetFloat1("mixValue", mixValue);
        

        ourShader.Use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Swap front buffer w/ back buffer
        glfwSwapBuffers(window);
        // Checks if any events has been triggered like key press
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