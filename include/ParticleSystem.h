#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <cstdio>
#include <vector>

struct ParticleProps
{
    // Position
    // Velocity, velocity variation
    // Color begin, color end
    // Size begin, size end, size variation
    // lifetime
};
struct Particle {
    // Position
    // Velocity
    // Starting/ending color
    // rotation
    // Starting/ending size
    // Lifetime
    // Remaining time
    // Alive/dead
};

class ParticleSystem
{
    private:
        // ?? Do not forget to start rendering particles at the end of vector in order to prevent misindexing
        // Set a contiguous block of memory and use  modulo for circular queue(data structure)
        std::vector<Particle> n_particles;
        
    public:
        // Update
        // render
        // emit


};



#endif