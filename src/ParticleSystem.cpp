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

#include "ParticleSystem.h"
#include "Shader.h"

#include <cstdio>
#include <vector>

ParticleSystem::ParticleSystem()
{
    pool_size = 1000;
    curr_idx = 0;
    particle_pool.resize(pool_size);
    
    VBO = 0;
    VAO = 0;
    EBO = 0;
}

ParticleSystem::~ParticleSystem()
{
    printf("Deallocating VAO, VBO, and EBO\n");
    // Deallocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void ParticleSystem::InitParticle(ParticleSettings &settings)
{
    Particle &particle = particle_pool[curr_idx];
    particle.position = settings.position;

    
    particle.rotation = rand() % 360 + (-360);
    particle.velocity = settings.velocity 
                        + glm::vec2(fmod(rand(), settings.velocity_variation.x), 
                                    fmod(rand(), settings.velocity_variation.y));
    particle.velocity = glm::vec2(particle.velocity.x * cos(particle.rotation),
                                    particle.velocity.y * sin(particle.rotation));

    particle.color_start = settings.color_start;
    particle.color_finish = settings.color_finish;

    particle.size_begin = settings.size_begin + fmod(rand(), settings.size_variation);
    particle.size_finish = settings.size_finish;

    particle.lifespan = settings.lifespan;
    particle.lifespan_remaining = particle.lifespan;

    particle.rotation = rand() % 360 + (-360);
    particle.active = true;

    curr_idx++;
    curr_idx = (curr_idx % pool_size);
}

void ParticleSystem::RenderParticle(WindowInfo &window_info)
{
    int width = window_info.width;
    int height = window_info.height;
    if (!VBO) {
        const char* vertex_path = "/home/ubuntu/Documents/Github/particle-system/assets/ParticleShaderCode/vertex.glsl";
        const char* fragment_path = "/home/ubuntu/Documents/Github/particle-system/assets/ParticleShaderCode/fragment.glsl";
        particle_shader.InitShader(vertex_path, fragment_path);

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
    }

    glBindVertexArray(VAO);
    particle_shader.Use();

    // ---------------------------- //
    for(auto& particle : particle_pool) {
        if(!particle.active)
            continue;

        unsigned int vertexColorLocation = glGetUniformLocation(particle_shader.ID, "particleColor");
        unsigned int transformLoc = glGetUniformLocation(particle_shader.ID, "transform");

        // Color
        float life = 1.0f - (particle.lifespan_remaining/particle.lifespan);
        glm::vec4 color = glm::lerp(particle.color_start,
                                    particle.color_finish,
                                    life);
        glUniform4fv(vertexColorLocation, 1, glm::value_ptr(color));

        // ------------ Transform ------------ //
        // Size
        float particle_size = glm::lerp(particle.size_begin, particle.size_finish, life);
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(particle.position, 0.0f))
                    * glm::rotate(transform, particle.rotation, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::scale(transform, glm::vec3(particle_size, particle_size, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void ParticleSystem::UpdateParticle(float timestep)
{
    for(auto& particle : particle_pool) {
        if(!particle.active)
            continue;
        
        if(particle.lifespan_remaining <= 0.0f) {
            particle.active = false;
            continue;
        }

        particle.lifespan_remaining -= timestep;
        particle.position += particle.velocity * timestep;
        particle.rotation += 1.0f * timestep;
    }
}


