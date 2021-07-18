#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

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

struct ParticleSettings
{
    glm::vec2 position;
    glm::vec2 velocity, velocity_variation;
    glm::vec4 color_start, color_finish;
    float size_begin, size_finish, size_variation;
    float lifespan;
};

struct WindowInfo
{
    int width, height;
};
class ParticleSystem
{
    private:
        struct Particle
        {
            glm::vec2 position;
            glm::vec2 velocity;
            glm::vec4 color_start, color_finish;
            float size_begin, size_finish;
            float lifespan, lifespan_remaining;
            float rotation;

            bool active;
        };

        int pool_size, curr_idx;
        std::vector<Particle> particle_pool;
        
        Shader particle_shader;
        GLuint VBO, VAO, EBO;

    public:
        
        ParticleSystem();
        ~ParticleSystem();
        
        void InitParticle(ParticleSettings &settings);
        void RenderParticle(WindowInfo &window_info);
        void UpdateParticle(float timestep);
        
};



#endif