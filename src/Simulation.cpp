#include "Simulation.h"
#include "Movements.h"
#include "iostream"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cassert>

Simulation::Simulation()
    : particles(PARTICLE_COUNT), pixelData(WIDTH * HEIGHT * 4, 0),
    randomState(123456789), selectedMaterial(&sandMaterial) 
{
    clearOccupied();
}

void Simulation::clearOccupied() 
{
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            occupied[y][x] = -1;
        }
    }
}

void Simulation::setPixel(int x, int y, const Material &material)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    int index = (y * WIDTH + x) * 4;

    pixelData[index + 0] = static_cast<unsigned char>(material.r * 255.0f);
    pixelData[index + 1] = static_cast<unsigned char>(material.g * 255.0f);
    pixelData[index + 2] = static_cast<unsigned char>(material.b * 255.0f);
    pixelData[index + 3] = static_cast<unsigned char>(material.a * 255.0f);
}

void Simulation::clearPixel(int x, int y) 
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    int index = (y * WIDTH + x) * 4;

    pixelData[index + 0] = 0;
    pixelData[index + 1] = 0;
    pixelData[index + 2] = 0;
    pixelData[index + 3] = 255;
}

void Simulation::activateParticle(int index) 
{
    if (index < 0 || index >= particleCount)
        return;

    Particle &p = particles[index];

    if (p.isActive())
        return;

    p.setActive(true);

    nextActiveParticles.push_back(index);
}

void Simulation::deactivateParticle(int index) 
{
    if (index < 0 || index >= particleCount)
        return;

    particles[index].setActive(false);
}

void Simulation::wakeNeighbors(int x, int y)
{
    // Above
    if (y > 0)
    {
        int index = occupied[y - 1][x];

        if (index != -1 && particles[index].isMovable())
            scheduleParticle(index);
    }

    // Below
    if (y + 1 < HEIGHT)
    {
        int index = occupied[y + 1][x];

        if (index != -1 && particles[index].isMovable())
            scheduleParticle(index);
    }
}

bool Simulation::canDisplace(int particleIndex, int otherIndex) 
{
    if (otherIndex == -1)
        return true;

    // ----- Walls / immovable particles cannot be displaced -----
    if (!particles[otherIndex].isMovable())
        return false;

    float myDensity = particles[particleIndex].getMaterial().density;
    float otherDensity = particles[otherIndex].getMaterial().density;

    return myDensity > otherDensity;
}

void Simulation::moveParticle(int index, int newX, int newY) 
{
    if (index < 0 || index >= particleCount)
        return;

    if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT)
        return;

    Particle &p = particles[index];

    int oldX = p.getX();
    int oldY = p.getY();

    if (oldX < 0 || oldX >= WIDTH || oldY < 0 || oldY >= HEIGHT)
        return;

    int direction = gravity > 0 ? p.getMaterial().dir : -p.getMaterial().dir;

    occupied[oldY][oldX] = -1;
    clearPixel(oldX, oldY);

    // wakeNeighbors(oldX, oldY, direction);
    wakeNeighbors(oldX, oldY);

    p.setPosition(newX, newY);

    occupied[newY][newX] = index;

    setPixel(newX, newY, p.getMaterial());
    scheduleParticle(index); // make it fall diagonally
}

void Simulation::placeParticle(int x, int y) 
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    if (particleCount >= PARTICLE_COUNT)
        return;

    if (occupied[y][x] != -1)
        return;

    // saving the particle on index
    // int newIndex = particleCount++;
    // if (!freeParticles.empty()) {
    //     // Reuse a previously removed particle slot
    //     newIndex = freeParticles.back();
    //     freeParticles.pop_back();
    // }
    // else
    // {
    //     if (particleCount >= PARTICLE_COUNT)
    //         return;
    //
    //     newIndex = particleCount++;
    // }

    int newIndex;

    if (!freeParticles.empty())
    {
        newIndex = freeParticles.back();
        freeParticles.pop_back();
    }
    else
    {
        if (particleCount >= PARTICLE_COUNT)
            return;

        newIndex = particleCount++;
    }

    particles[newIndex] = Particle(x, y, *selectedMaterial);

    // ----- Fire -----

    if (selectedMaterial->isFire)
    {
        particles[newIndex].setVelocity(-(400.0f + fastRandom() % 150));

        particles[newIndex].setHorizontalVelocity(
                static_cast<float>(static_cast<int>(fastRandom() % 61) - 30));

        particles[newIndex].setLifetime(1.0f + (fastRandom() % 100) / 100.0f);
    }

    occupied[y][x] = newIndex;

    // Draw the particle directly into the pixel buffer.
    setPixel(x, y, *selectedMaterial);

    activateParticle(newIndex);
}

void Simulation::removeParticle(int index) 
{
    if (index < 0 || index >= static_cast<int>(particles.size()))
        return;

    Particle &p = particles[index];

    if (p.getX() < 0 || p.getY() < 0)
        return;

    int x = p.getX();
    int y = p.getY();
    int direction = gravity > 0 ? p.getMaterial().dir : -p.getMaterial().dir;

    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) 
    {
        if (occupied[y][x] == index) 
        {
            occupied[y][x] = -1;
            clearPixel(x, y);
            // wakeNeighbors(x, y, direction);
            wakeNeighbors(x, y);
        }
    }

    p.setActive(false);
    p.setPosition(-1, -1);
    freeParticles.push_back(index);
    // --particleCount;
}

void Simulation::useBrush(int centerX, int centerY, int radius, bool erase) 
{
    for (int dx = -radius; dx <= radius; dx++) 
    {
        for (int dy = -radius; dy <= radius; dy++) 
        {
            if (dx * dx + dy * dy > radius * radius) 
            {
                continue;
            }

            int x = centerX + dx;
            int y = centerY + dy;

            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
            {
                continue;
            }

            if (erase)
            {
                int index = occupied[y][x];

                if (index != -1) 
                    removeParticle(index);
            }
            else 
                placeParticle(x, y);
        }
    }
}

void Simulation::update(float deltaTime)
{
    for (int particleIndex : activeParticles) 
    {
        if (particleIndex < 0 || particleIndex >= particleCount)
            continue;
        particles[particleIndex].setScheduled(false);
    }
    for (int particleIndex : activeParticles) 
    {
        if (particleIndex < 0 || particleIndex >= particleCount)
            continue;

        Particle &p = particles[particleIndex];

        // p.setActive(false);
        // p.setScheduled(false);

        if (p.getX() < 0 || p.getY() < 0)
            continue;

        p.updateLifetime(deltaTime);

        if (p.isDead())
        {
            if(p.getMaterial().type == MaterialType::Fire)
                fireDies(particleIndex);
            else
            {
                removeParticle(particleIndex);
            }
            continue;
        }

        // if (p.hasLifetime()) 
        //     activateParticle(particleIndex);

        if (!p.isMovable())
            continue;

        updateParticle(p, particleIndex, deltaTime);
    }

    activeParticles.swap(nextActiveParticles);
    nextActiveParticles.clear();
}

void Simulation::updateParticle(Particle &p, int index, float deltaTime) 
{
    // ----- Gravity -----

    if (p.isAffectedByGravity()) 
        p.applyGravity(getGravity() * p.getMaterial().gravityValue, deltaTime);

    //  ----- Calculate vertical movement -----

    // float movement = p.getVelocity() * deltaTime;
    //
    // int steps = static_cast<int>(std::abs(movement));

    // steps = std::clamp(steps, 1, 4);

    int direction = gravity > 0 ? p.getMaterial().dir : -p.getMaterial().dir;

    // moveVertical(p, index, direction);
    if (Simulation::verticalMoves)
    {
        if(moveVertical(p, index, direction))
            return;
    }

    // Particle may have been destroyed.
    if (p.getX() < 0 || p.getY() < 0)
        return;

    if (Simulation::diagonalMoves)
    {
        moveDiagonal(p, index, direction);
    }
    // if(moveDiagonal(p, index, direction))
    //     return;

    // Particle may have been destroyed.
    if (p.getX() < 0 || p.getY() < 0)
        return;
    // if (Simulation::horizontalMoves)
    // {
    // moveHorizontal(p, index);
    // }
}

unsigned int Simulation::fastRandom() 
{
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;
    return randomState;
}

void Simulation::fireDies(int index) 
{
    if (index < 0 || index >= particleCount)
        return;

    Particle &fire = particles[index];

    int x = fire.getX();
    int y = fire.getY();

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    // Remove the fire from the grid
    if (occupied[y][x] == index) 
    {
        occupied[y][x] = -1;
        clearPixel(x, y);
    }

    fire.setActive(false);
    fire.setPosition(-1, -1);

    freeParticles.push_back(index);

    const Material* previousMaterial = selectedMaterial;

    selectedMaterial = &smokeMaterial;
    placeParticle(x, y);

    // Restore player's selected material
    selectedMaterial = previousMaterial;
    wakeNeighbors(x, y);
}

void Simulation::clearAll() 
{
    // 1. Reset grid indices
    clearOccupied();

    // 2. Clear RGBA pixel data back to black (opaque)
    std::fill(pixelData.begin(), pixelData.end(), 0);
    for (size_t i = 3; i < pixelData.size(); i += 4)
        pixelData[i] = 255; // Set Alpha to 255

    // 3. Clear active tracking arrays
    activeParticles.clear();
    nextActiveParticles.clear();
    freeParticles.clear();

    // 4. Reset particle states
    for (int i = 0; i < PARTICLE_COUNT; i++) 
    {
        particles[i] = Particle();
        particles[i].setPosition(-1, -1);
        particles[i].setActive(false);
    }

    // 5. Reset active counters
    particleCount = 0;
}

float Simulation::getGravity() const
{
    return gravity;
}

void    Simulation::setGravity(float value) 
{
    gravity = value;
}

void Simulation::addParticle( int x, int y, const Material& material)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    if (occupied[y][x] != -1)
        return;

    int newIndex;

    if (!freeParticles.empty())
    {
        // Reuse an old particle slot
        newIndex = freeParticles.back();
        freeParticles.pop_back();
    }
    else
    {
        if (particleCount >= PARTICLE_COUNT)
            return;

        newIndex = particleCount++;
    }

    particles[newIndex] = Particle(x, y, material);

    occupied[y][x] = newIndex;

    // Draw it immediately
    setPixel(x, y, material);

    // Make it part of the active particle list
    activateParticle(newIndex);
}

void Simulation::removeParticlesInRadius( int centerX, int centerY, int radius)
{
    int radiusSquared = radius * radius;

    std::vector<int> toRemove;

    for (int y = centerY - radius; y <= centerY + radius; ++y)
    {
        if (y < 0 || y >= HEIGHT)
            continue;

        for (int x = centerX - radius; x <= centerX + radius; ++x)
        {
            if (x < 0 || x >= WIDTH)
                continue;

            int dx = x - centerX;
            int dy = y - centerY;

            if (dx * dx + dy * dy > radiusSquared)
                continue;

            int index = occupied[y][x];

            if (index != -1)
            {
                toRemove.push_back(index);
            }
        }
    }

    // Remove after we've finished scanning the grid
    for (int index : toRemove)
    {
        removeParticle(index);
    }
}

bool Simulation::isOccupied(int x, int y) const
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return false;

    return occupied[y][x] != -1;
}

void Simulation::scheduleParticleAbove(int x, int y)
{
    if (y <= 0)
        return;

    int index = occupied[y - 1][x];

    if (index != -1 && particles[index].isMovable())
        scheduleParticle(index);
}

// void Simulation::scheduleParticle(int index)
// {
//     if (index < 0 || index >= particleCount)
//         return;
//
//     Particle& p = particles[index];
//
//     if (p.getX() < 0 || p.getY() < 0)
//         return;
//
//     if (!p.isMovable())
//         return;
//
//     if (p.isScheduled())
//         return;
//
//     p.setScheduled(true);
//     nextActiveParticles.push_back(index);
// }

void Simulation::scheduleParticle(int index)
{
    if (index < 0 || index >= particleCount)
        return;

    Particle& p = particles[index];

    if (p.isScheduled())
        return;

    p.setScheduled(true);
    nextActiveParticles.push_back(index);
}

bool    Simulation::moveVertical(Particle& p, int index, int direction)
{
    int x = p.getX();
    int y = p.getY();
    int nextY = y + direction;

    // Particle reached the vertical boundary
    // Fire dies and other stop
    if (checkBoundaryWindow(p, index, nextY, HEIGHT))
        return false;

    int otherIndex = occupied[nextY][x];

    // ----- Reaction on contact -----
    if (particleReaction(p, index, otherIndex))
        return false;

    // Target empty cell
    if (otherIndex == -1)
    {
        occupied[y][x] = -1;
        clearPixel(x, y);
        wakeNeighbors(x, y);

        p.setPosition(x, nextY);
        occupied[nextY][x] = index;
        setPixel(x, nextY, p.getMaterial());

        scheduleParticle(index);
        return true;
    }

    // Occupied cell: only swap if we are denser
    if (!canDisplace(index, otherIndex))
    {
        // Make sure the fire keep on schedule even as blocked
        if(p.getMaterial().isFire)
            scheduleParticle(index);
        return false;
    }

    Particle& other = particles[otherIndex];

    // Move displaced particle into our old position
    occupied[y][x] = otherIndex;

    other.setPosition(x, y);
    setPixel(x, y, other.getMaterial());

    // Move current particle into target
    p.setPosition(x, nextY);
    occupied[nextY][x] = index;
    setPixel(x, nextY, p.getMaterial());

    // both particles moved, so both need another update
    scheduleParticle(otherIndex);
    scheduleParticle(index);

    // The movement changed both cells, so wake neighbors
    wakeNeighbors(x, y);
    wakeNeighbors(x, nextY);
    return true;
}

// bool Simulation::moveDiagonal(Particle& p, int index, int direction)
// {
//     int x = p.getX();
//     int y = p.getY();
//
//     int nextY = y + direction;
//
//     if (nextY < 0 || nextY >= HEIGHT)
//         return false;
//
//     // Straight cell isn't blocked, so don't move diagonally.
//     if (occupied[nextY][x] == -1)
//         return false;
//
//     // Randomly choose which diagonal to try first
//     int firstDir = (fastRandom() & 1) ? -1 : 1;
//
//     // Try first side, then the other side
//     for (int i = 0; i < 2; ++i)
//     {
//         int dx = (i == 0) ? firstDir : -firstDir;
//         int nextX = x + dx;
//
//         if (nextX < 0 || nextX >= WIDTH)
//             continue;
//
//         // ONLY move if diagonal is empty
//         if (occupied[nextY][nextX] == -1)
//         {
//             moveParticle(index, nextX, nextY);
//             return true;
//         }
//     }
//
//     return false;
// }

bool Simulation::moveDiagonal(Particle& p, int index, int direction)
{

    int x = p.getX();
    int y = p.getY();

    int nextY = y + direction;

    // ----- Outside vertical bounds -----
    if (checkBoundaryWindow(p, index, nextY, HEIGHT))
        return false;

    //  ----- Randomize left/right -----
    int firstDirection = (fastRandom() & 1) ? -1 : 1;

    for (int attempt = 0; attempt < 2; ++attempt)
    {
        int horizontalDirection = (attempt == 0) ? firstDirection : -firstDirection;

        // maybe horizontalDirection might be the steps according to material
        int nextX = x + horizontalDirection;

        // Outside horizontal bounds
        if (nextX < 0 || nextX >= WIDTH)
            continue;

        int otherIndex = occupied[nextY][nextX];

        // ----- Empty diagonal cell -----

        if (otherIndex == -1)
        {
            occupied[y][x] = -1;
            clearPixel(x, y);

            p.setPosition(nextX, nextY);
            occupied[nextY][nextX] = index;

            setPixel(nextX, nextY, p.getMaterial());

            scheduleParticle(index);

            wakeNeighbors(x, y);
            wakeNeighbors(nextX, nextY);

            return true;
        }

        // ----- Occupied diagonal cell -----

        if (!canDisplace(index, otherIndex))
            continue;

        Particle& other = particles[otherIndex];

        // Move displaced particle into old position
        occupied[y][x] = otherIndex;

        other.setPosition(x, y);
        setPixel(x, y, other.getMaterial());

        // Move current particle diagonally
        p.setPosition(nextX, nextY);
        occupied[nextY][nextX] = index;

        setPixel(nextX, nextY, p.getMaterial());

        scheduleParticle(otherIndex);
        scheduleParticle(index);

        wakeNeighbors(x, y);
        wakeNeighbors(nextX, nextY);

        return true;
    }

    return false;
}

bool Simulation::moveHorizontal(Particle& p, int index)
{
    int x = p.getX();
    int y = p.getY();

    const Material& material = p.getMaterial();

    // No horizontal spreading.
    if (material.spread <= 0.0f)
        return false;

    // --------------------------------------------------
    // Viscosity
    //
    // Low viscosity  -> moves easily
    // High viscosity -> moves less often
    // --------------------------------------------------

    // float viscosity = std::clamp(material.viscosity, 0.0f, 1.0f);
    float viscosity = p.getMaterial().viscosity;

    float movementChance = 1.0f - viscosity;

    float randomValue =
        static_cast<float>(fastRandom()) / static_cast<float>(UINT_MAX);

    if (randomValue > movementChance)
        return false;

    // --------------------------------------------------
    // Maximum horizontal search distance
    // --------------------------------------------------

    int maxSpread = static_cast<int>(material.spread * 10.0f);

    if (maxSpread < 1)
        maxSpread = 1;

    // Randomize which side we try first.
    int firstDirection = (fastRandom() & 1) ? -1 : 1;

    for (int distance = 1; distance <= maxSpread; ++distance)
    {
        for (int attempt = 0; attempt < 2; ++attempt)
        {
            int horizontalDirection =
                (attempt == 0) ? firstDirection : -firstDirection;

            int nextX = x + horizontalDirection * distance;

            if (checkBoundaryWindow(p, index, nextX, WIDTH))
                continue;

            int otherIndex = occupied[y][nextX];

            // --------------------------------------------------
            // Empty cell
            // --------------------------------------------------

            if (otherIndex == -1)
            {
                // Don't jump over particles.
                bool pathBlocked = false;

                for (int checkX = x + horizontalDirection; checkX != nextX; checkX += horizontalDirection)
                {
                    if (occupied[y][checkX] != -1)
                    {
                        pathBlocked = true;
                        break;
                    }
                }

                if (pathBlocked)
                    break;

                // Move particle.
                occupied[y][x] = -1;
                clearPixel(x, y);

                p.setPosition(nextX, y);
                occupied[y][nextX] = index;

                setPixel(nextX, y, material);

                wakeNeighbors(x, y);
                wakeNeighbors(nextX, y);

                scheduleParticle(index);

                return true;
            }

            // Something is blocking this direction.
            // Don't jump through it.
            break;
        }
    }

    return false;
}

bool Simulation::checkBoundaryWindow(Particle& p, int index, int position, const int border)
{

    if (position < 0 || position >= border)
    {
        if (p.getMaterial().isFire)
        {
            assert(index >= 0);
            assert(index < static_cast<int>(particles.size()));
            fireDies(index);
        }
        else
        {
            p.stop();
        }
        return true;
    }
    return false;
}

bool Simulation::particleReaction(Particle& p, int index, int otherIndex)
{
    if (otherIndex == -1)
        return false;
    if (p.getMaterial().type == MaterialType::Fire && otherIndex != -1 &&
            particles[otherIndex].getMaterial().type == MaterialType::Smoke) 
    {
        fireDies(index);
        return true;
    }
    return false;
}
