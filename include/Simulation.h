#pragma once

#include "Particle.h"
#include "Materials.h"

#include <vector>

class Simulation
{
    public:
        static constexpr int WIDTH = 192;
        static constexpr int HEIGHT = 108;
        static constexpr int SCALE = 10;                        // Pixel size
        static constexpr int PARTICLE_COUNT = 10000000;
        int gravity = 10;
        bool horizontalMoves = true;
        bool diagonalMoves = true;
        bool verticalMoves = true;

        Simulation();

        bool canDisplace( int particleIndex, int otherIndex);
        bool checkBoundaryWindow(Particle& p, int index, int position, const int border);
        bool isOccupied(int x, int y) const;
        bool moveDiagonal(Particle& p, int index, int direction);
        bool moveHorizontal(Particle&p, int index);
        bool moveVertical(Particle&p, int index, int direction);
        bool particleReaction(Particle& p, int index, int otherIndex);

        const std::vector<unsigned char>& getPixelData() const { return pixelData; }
        float getGravity() const;
        int getParticleCount() const { return particleCount; }
        std::size_t getActiveParticleCount() const { return activeParticles.size(); }
        unsigned int fastRandom();

        void activateParticle(int index);
        void addParticle( int x, int y, const Material& material);
        void clearAll();
        void clearOccupied();
        void clearPixel(int x, int y);
        void deactivateParticle(int index);
        void fireDies(int index);
        void moveParticle( int index, int newX, int newY);
        void placeParticle(int x, int y);
        void removeParticle(int index);
        void removeParticlesInRadius( int centerX, int centerY, int radius);
        void scheduleParticle(int index);
        void scheduleParticleAbove(int x, int y);
        void setGravity(float value);
        void setPixel( int x, int y, const Material& material);
        void setSelectedMaterial(const Material* material) { selectedMaterial = material; }
        void update(float deltaTime);
        void updateParticle( Particle& p, int index, float deltaTime);
        void useBrush( int centerX, int centerY, int radius, bool erase);
        void wakeNeighbors(int x, int y);

    private:
        unsigned int randomState;
        int occupied[HEIGHT][WIDTH];
        int particleCount = 0;

        std::vector<Particle> particles;
        std::vector<int> freeParticles;
        std::vector<int> activeParticles;
        std::vector<int> nextActiveParticles;
        std::vector<unsigned char> pixelData;

        const Material* selectedMaterial = &sandMaterial;

};

