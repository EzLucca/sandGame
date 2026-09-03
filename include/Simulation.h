#pragma once

#include "Particle.h"
#include "Materials.h"

#include <vector>

class Simulation
{
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        static constexpr int PARTICLE_COUNT = 10000000;
        int gravity = 10;

        Simulation();

        void update(float deltaTime);

        void placeParticle(int x, int y);
        void removeParticle(int index);

        void useBrush( int centerX, int centerY, int radius, bool erase);

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

        void fireDies(int index);

        void clearAll();

        float getGravity() const;
        void setGravity(float value);
        void setPixel( int x, int y, const Material& material);
        void clearPixel(int x, int y);
        void addParticle( int x, int y, const Material& material);
    private:
        unsigned int randomState;
        int occupied[HEIGHT][WIDTH];

        std::vector<Particle> particles;
        std::vector<int> freeParticles;

        int particleCount = 0;

        std::vector<int> activeParticles;
        std::vector<int> nextActiveParticles;

        std::vector<unsigned char> pixelData;

        const Material* selectedMaterial = &sandMaterial;

        void clearOccupied();

        void activateParticle(int index);
        void deactivateParticle(int index);

        void wakeNeighbors(int x, int y);

        bool canDisplace( int particleIndex, int otherIndex);

        void moveParticle( int index, int newX, int newY);

        void updateParticle( Particle& p, int index, float deltaTime);

        unsigned int fastRandom();
};

