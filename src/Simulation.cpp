#include "Simulation.h"
#include "iostream"

#include <algorithm>
#include <cmath>
#include <cstdlib>

Simulation::Simulation()
    : particles(PARTICLE_COUNT), pixelData(WIDTH * HEIGHT * 4, 0),
    randomState(123456789), selectedMaterial(&sandMaterial) {
        clearOccupied();
    }

void Simulation::clearOccupied() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            occupied[y][x] = -1;
        }
    }
}

void Simulation::setPixel(int x, int y, const Material &material) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    int index = (y * WIDTH + x) * 4;

    pixelData[index + 0] = static_cast<unsigned char>(material.r * 255.0f);

    pixelData[index + 1] = static_cast<unsigned char>(material.g * 255.0f);

    pixelData[index + 2] = static_cast<unsigned char>(material.b * 255.0f);

    pixelData[index + 3] = static_cast<unsigned char>(material.a * 255.0f);
}

void Simulation::clearPixel(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    int index = (y * WIDTH + x) * 4;

    pixelData[index + 0] = 0;
    pixelData[index + 1] = 0;
    pixelData[index + 2] = 0;
    pixelData[index + 3] = 255;
}

void Simulation::activateParticle(int index) {
    if (index < 0 || index >= particleCount)
        return;

    Particle &p = particles[index];

    if (p.isActive())
        return;

    p.setActive(true);

    nextActiveParticles.push_back(index);
}

void Simulation::deactivateParticle(int index) {
    if (index < 0 || index >= particleCount)
        return;

    particles[index].setActive(false);
}

void Simulation::wakeNeighbors(int x, int y) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT)
                continue;

            int index = occupied[ny][nx];

            if (index == -1)
                continue;

            if (!particles[index].isMovable())
                continue;

            activateParticle(index);
        }
    }
}

bool Simulation::canDisplace(int particleIndex, int otherIndex) {
    if (otherIndex == -1)
        return true;
    // Walls / immovable particles cannot be displaced
    if (!particles[otherIndex].isMovable())
        return false;

    float myDensity = particles[particleIndex].getMaterial().density;

    float otherDensity = particles[otherIndex].getMaterial().density;

    return myDensity > otherDensity;
}

void Simulation::moveParticle(int index, int newX, int newY) {
    if (index < 0 || index >= particleCount)
        return;

    if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT)
        return;

    Particle &p = particles[index];

    int oldX = p.getX();
    int oldY = p.getY();

    if (oldX < 0 || oldX >= WIDTH || oldY < 0 || oldY >= HEIGHT)
        return;

    occupied[oldY][oldX] = -1;
    clearPixel(oldX, oldY);

    wakeNeighbors(oldX, oldY);

    p.setPosition(newX, newY);

    occupied[newY][newX] = index;

    setPixel(newX, newY, p.getMaterial());
}

void Simulation::placeParticle(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    if (particleCount >= PARTICLE_COUNT)
        return;

    if (occupied[y][x] != -1)
        return;

    // saving the particle on index
    int newIndex = particleCount++;
    if (!freeParticles.empty()) {
        // Reuse a previously removed particle slot
        newIndex = freeParticles.back();
        freeParticles.pop_back();
    } else {
        if (particleCount >= PARTICLE_COUNT)
            return;

        newIndex = particleCount++;
    }
    particles[newIndex] = Particle(x, y, *selectedMaterial);

    // Fire

    if (selectedMaterial->isFire) {
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

void Simulation::removeParticle(int index) {
    if (index < 0 || index >= particleCount)
        return;

    Particle &p = particles[index];

    if (p.getX() < 0 || p.getY() < 0)
        return;

    int x = p.getX();
    int y = p.getY();

    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        if (occupied[y][x] == index) {
            occupied[y][x] = -1;
            clearPixel(x, y);
            wakeNeighbors(x, y);
        }
    }

    p.setActive(false);
    p.setPosition(-1, -1);
    freeParticles.push_back(index);
}

void Simulation::useBrush(int centerX, int centerY, int radius, bool erase) {
    for (int dx = -radius; dx <= radius; dx++) {
        for (int dy = -radius; dy <= radius; dy++) {
            if (dx * dx + dy * dy > radius * radius) {
                continue;
            }

            int x = centerX + dx;
            int y = centerY + dy;

            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
                continue;
            }

            if (erase) {
                int index = occupied[y][x];

                if (index != -1) {
                    removeParticle(index);
                }
            } else {
                placeParticle(x, y);
            }
        }
    }
}

void Simulation::update(float deltaTime) {
    for (int particleIndex : activeParticles) {
        if (particleIndex < 0 || particleIndex >= particleCount)
            continue;

        Particle &p = particles[particleIndex];

        p.setActive(false);

        if (p.getX() < 0 || p.getY() < 0)
            continue;

        p.updateLifetime(deltaTime);

        if (p.isDead()) {
            removeParticle(particleIndex);
            continue;
        }

        if (p.hasLifetime()) {
            activateParticle(particleIndex);
        }

        if (!p.isMovable())
            continue;

        updateParticle(p, particleIndex, deltaTime);
    }

    activeParticles.swap(nextActiveParticles);
    nextActiveParticles.clear();
}

void Simulation::updateParticle(Particle &p, int index, float deltaTime) {
    // -----------------------------------------
    // Gravity
    // -----------------------------------------

    if (p.isAffectedByGravity()) {
        p.applyGravity(getGravity() * p.getMaterial().gravityValue, deltaTime);
    }

    // -----------------------------------------
    // Calculate vertical movement
    // -----------------------------------------

    float movement = p.getVelocity() * deltaTime;

    int steps = static_cast<int>(std::abs(movement));

    steps = std::clamp(steps, 1, 4);

    int direction = 0;
    if (gravity > 0)
        direction = p.getMaterial().dir;
    else
        direction = - p.getMaterial().dir;

    // -----------------------------------------
    // Vertical + diagonal movement
    // -----------------------------------------

    bool blocked = false;

    for (int step = 0; step < steps; step++) {
        int x = p.getX();
        int y = p.getY();

        int nextY = y + direction;

        // -------------------------------------
        // Outside screen
        // -------------------------------------

        if (nextY < 0 || nextY >= HEIGHT) {
            if (p.getMaterial().isFire) {
                removeParticle(index);
                return;
            }

            p.stop();
            blocked = true;
            break;
        }

        // -------------------------------------
        // Vertical movement
        // -------------------------------------

        int otherIndex = occupied[nextY][x];

        if (p.getMaterial().isFire && otherIndex != -1 &&
                &particles[otherIndex].getMaterial() == &smokeMaterial) {
            fireDies(index);
            return;
        }

        if (canDisplace(index, otherIndex)) {
            occupied[y][x] = otherIndex;

            if (otherIndex != -1) {
                particles[otherIndex].setPosition(x, y);
                activateParticle(otherIndex);

                setPixel(x, y, particles[otherIndex].getMaterial());
            } else {
                clearPixel(x, y);
                wakeNeighbors(x, y);
            }

            p.setPosition(x, nextY);

            occupied[nextY][x] = index;

            setPixel(x, nextY, p.getMaterial());

            activateParticle(index);

            continue;
        }

        // -------------------------------------
        // Diagonal movement
        // -------------------------------------

        bool moved = false;

        int firstDirection = (fastRandom() & 1) ? -1 : 1;

        for (int attempt = 0; attempt < 2; attempt++) {
            int nextX = x + firstDirection;

            if (nextX >= 0 && nextX < WIDTH) {
                int diagonalIndex = occupied[nextY][nextX];

                if (canDisplace(index, diagonalIndex)) {
                    occupied[y][x] = diagonalIndex;

                    if (diagonalIndex != -1) {
                        particles[diagonalIndex].setPosition(x, y);

                        activateParticle(diagonalIndex);

                        setPixel(x, y, particles[diagonalIndex].getMaterial());
                    } else {
                        clearPixel(x, y);
                        wakeNeighbors(x, y);
                    }

                    p.setPosition(nextX, nextY);

                    activateParticle(index);

                    setPixel(nextX, nextY, p.getMaterial());

                    occupied[nextY][nextX] = index;

                    moved = true;
                    break;
                }
            }
            firstDirection *= -1;
        }

        if (moved) {
            activateParticle(index);
            continue;
        }

        // -------------------------------------
        // Couldn't move
        // -------------------------------------

        blocked = true;
        break;
    }

    // -----------------------------------------
    // Horizontal spreading (Fixed Liquid Movement & Solid Walls)
    // -----------------------------------------

    float spread = p.getSpread();

    if (spread > 0.0f) {
        int maxSpread = static_cast<int>(spread * 10.0f);
        if (maxSpread < 1)
            maxSpread = 1;

        int currentX = p.getX();
        int y = p.getY();

        bool leftBlocked = false;
        bool rightBlocked = false;

        // Randomize initial direction to keep fluid level flat
        bool checkLeftFirst = (fastRandom() & 1) != 0;

        for (int distance = 1; distance <= maxSpread; distance++) {
            auto trySide = [&](int dist, int dir, bool &isBlocked) -> bool {
                if (isBlocked)
                    return false;

                int targetX = currentX + (dist * dir);

                // 1. Boundary Check
                if (targetX < 0 || targetX >= WIDTH) {
                    isBlocked = true;
                    return false;
                }

                int otherIdx = occupied[y][targetX];

                // 2. Target cell is Empty Air -> Move into it!
                if (otherIdx == -1) {
                    occupied[y][currentX] = -1;
                    clearPixel(currentX, y);
                    wakeNeighbors(currentX, y);

                    p.setPosition(targetX, y);
                    occupied[y][targetX] = index;
                    setPixel(targetX, y, p.getMaterial());

                    activateParticle(index);
                    return true; // Successfully moved
                }

                // 3. Target cell has an immovable solid (Stone) -> Stop scanning this
                // side!
                if (!particles[otherIdx].isMovable()) {
                    isBlocked = true;
                    return false;
                }

                // 4. Target cell is another liquid/movable particle with same/higher
                // density -> Do NOT block scanning! Let the scan step OVER this
                // particle to look for air further out.
                return false;
            };

            if (checkLeftFirst) {
                if (trySide(distance, -1, leftBlocked))
                    break;
                if (trySide(distance, 1, rightBlocked))
                    break;
            } else {
                if (trySide(distance, 1, rightBlocked))
                    break;
                if (trySide(distance, -1, leftBlocked))
                    break;
            }

            if (leftBlocked && rightBlocked) {
                break;
            }
        }
    }
}

unsigned int Simulation::fastRandom() {
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;

    return randomState;
}

void Simulation::fireDies(int index) {
    if (index < 0 || index >= particleCount)
        return;

    Particle &fire = particles[index];

    int x = fire.getX();
    int y = fire.getY();

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    // Remove the fire from the grid
    if (occupied[y][x] == index) {
        occupied[y][x] = -1;
        clearPixel(x, y);
    }

    fire.setActive(false);
    fire.setPosition(-1, -1);

    freeParticles.push_back(index);

    // Create smoke in the same location
    selectedMaterial = &smokeMaterial;
    placeParticle(x, y);
}

void Simulation::clearAll() {
    // 1. Reset grid indices
    clearOccupied();

    // 2. Clear RGBA pixel data back to black (opaque)
    std::fill(pixelData.begin(), pixelData.end(), 0);
    for (size_t i = 3; i < pixelData.size(); i += 4) {
        pixelData[i] = 255; // Set Alpha to 255
    }

    // 3. Clear active tracking arrays
    activeParticles.clear();
    nextActiveParticles.clear();
    freeParticles.clear();

    // 4. Reset particle states
    for (int i = 0; i < PARTICLE_COUNT; i++) {
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

