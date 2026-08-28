#pragma once

#include "Particle.h"
#include "Materials.h"

#include <vector>

class Simulation
{
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        static constexpr int PARTICLE_COUNT = 1000000;

        Simulation();

        void update(float deltaTime);

        void placeParticle(int x, int y);
        void removeParticle(int index);

        void useBrush(
                int centerX,
                int centerY,
                int radius,
                bool erase
                );


        void setSelectedMaterial(const Material* material)
        {
            selectedMaterial = material;
        }

        const std::vector<unsigned char>& getPixelData() const
        {
            return pixelData;
        }

        int getParticleCount() const
        {
            return particleCount;
        }

        std::size_t getActiveParticleCount() const
        {
            return activeParticles.size();
        }

    private:
        unsigned int randomState;
        int occupied[HEIGHT][WIDTH];

        std::vector<Particle> particles;

        int particleCount = 0;

        std::vector<int> activeParticles;
        std::vector<int> nextActiveParticles;

        std::vector<unsigned char> pixelData;

        const Material* selectedMaterial = &sandMaterial;

        void clearOccupied();

        void setPixel(
                int x,
                int y,
                const Material& material
                );

        void clearPixel(int x, int y);

        void activateParticle(int index);
        void deactivateParticle(int index);

        void wakeNeighbors(int x, int y);

        bool canDisplace(
                int particleIndex,
                int otherIndex
                );

        void moveParticle(
                int index,
                int newX,
                int newY
                );

        void updateParticle(
                Particle& p,
                int index,
                float deltaTime
                );

        unsigned int fastRandom();
};

