#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <linmath.h>
#include <cstdio>

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

const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";


int main(int argc, const char** argv)
{
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
    // glfwSwapInterval(1);


    // Build and compile shader program
    // -----------------------------
    /* Vertex Shader */
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    /* Check for compilation error for vertex shader */
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }


    /* Fragment Shader */
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    /* Check for compilation error for fragment shader */
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }


    /* Link shaders */
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    /* Check for linking errors */
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER:LINKING::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Delete Shader objects after linking to program */
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Setup vertex data, buffer, and configure vertex attributes
    // ------------------------------------------
    float triangle1[] = {
        -1.0f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    float triangle2[] = {
         1.0f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };

    unsigned int VBOs[2], VAOs[2];
    /* glGen == glGenerate */
    glGenBuffers(2, VBOs);
    glGenVertexArrays(2, VAOs);
    
    // Triangle 1
    glBindVertexArray(VAOs[0]); // Make VAO active
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // Make VBO active, by initializing it
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW); // Copy information about vertex into buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // Add information about how to interpret vertex array
    glEnableVertexAttribArray(0); // Enable vertex attribute and give vertex attribute location as arg

    // Triangle 2
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window)) {
        // Render commands
        glClearColor(0.188f, 0.188f, 0.188f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* 
            Result of linking shaders is a program object.
            Every shader and rendering call after glUseProgram will now use this program object.
        */
        glUseProgram(shaderProgram);

        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);



        // Swap front buffer w/ back buffer
        glfwSwapBuffers(window);
        // Checks if any events has been triggered like key press
        glfwPollEvents();
    }

    // Deallocate resources
    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(1, VBOs);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}