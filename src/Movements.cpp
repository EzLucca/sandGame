#include "Simulation.h"
#include "Movements.h"

#include <algorithm>
#include <cmath>

bool Simulation::canDisplace(int particleIndex, int otherIndex)
{
    // Empty cell
    if (otherIndex == -1)
        return true;

    float myDensity =
        particles[particleIndex].getMaterial().density;

    float otherDensity =
        particles[otherIndex].getMaterial().density;

    return myDensity > otherDensity;
}

void Simulation::wakeNeighbors(int x, int y)
{
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= WIDTH ||
                ny < 0 || ny >= HEIGHT)
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

void Simulation::moveParticle(int index, int newX, int newY)
{
    Particle& p = particles[index];

    int oldX = p.getX();
    int oldY = p.getY();

    occupied[oldY][oldX] = -1;
    clearPixel(oldX, oldY);

    wakeNeighbors(oldX, oldY);

    p.setPosition(newX, newY);

    occupied[newY][newX] = index;

    setPixel(newX, newY, p.getMaterial());
}

void Simulation::removeParticle(int index)
{
    if (index < 0 || index >= particleCount)
        return;

    Particle& p = particles[index];

    int x = p.getX();
    int y = p.getY();

    if (x >= 0 && x < WIDTH &&
        y >= 0 && y < HEIGHT)
    {
        if (occupied[y][x] == index)
        {
            occupied[y][x] = -1;
            clearPixel(x, y);

            wakeNeighbors(x, y);
        }
    }

    p.setActive(false);
    p.setPosition(-1, -1);
}

void Simulation::updateParticle(
    Particle& p,
    int index,
    float deltaTime)
{
    // -----------------------------------------
    // Gravity
    // -----------------------------------------

    if (p.isAffectedByGravity())
    {
        p.applyGravity(
            p.getMaterial().gravityValue,
            deltaTime
        );
    }

    // -----------------------------------------
    // Calculate vertical movement
    // -----------------------------------------

    float movement = p.getVelocity() * deltaTime;

    int steps = static_cast<int>(std::abs(movement));

    steps = std::clamp(steps, 1, 4);

    int direction = p.getMaterial().dir;

    // -----------------------------------------
    // Vertical + diagonal movement
    // -----------------------------------------

    bool blocked = false;

    for (int step = 0; step < steps; step++)
    {
        int x = p.getX();
        int y = p.getY();

        int nextY = y + direction;

        // -------------------------------------
        // Outside screen
        // -------------------------------------

        if (nextY < 0 || nextY >= HEIGHT)
        {
            if (p.getMaterial().isFire)
            {
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

        if (canDisplace(index, otherIndex))
        {
            occupied[y][x] = otherIndex;

            if (otherIndex != -1)
            {
                particles[otherIndex].setPosition(x, y);
                activateParticle(otherIndex);

                setPixel(
                    x,
                    y,
                    particles[otherIndex].getMaterial()
                );
            }
            else
            {
                clearPixel(x, y);
                wakeNeighbors(x, y);
            }

            p.setPosition(x, nextY);

            occupied[nextY][x] = index;

            setPixel(
                x,
                nextY,
                p.getMaterial()
            );

            activateParticle(index);

            continue;
        }

        // -------------------------------------
        // Diagonal movement
        // -------------------------------------

        bool moved = false;

        int firstDirection =
            (fastRandom() & 1) ? -1 : 1;

        for (int attempt = 0; attempt < 2; attempt++)
        {
            int nextX = x + firstDirection;

            if (nextX >= 0 && nextX < WIDTH)
            {
                int diagonalIndex =
                    occupied[nextY][nextX];

                if (canDisplace(index, diagonalIndex))
                {
                    occupied[y][x] = diagonalIndex;

                    if (diagonalIndex != -1)
                    {
                        particles[diagonalIndex]
                            .setPosition(x, y);

                        activateParticle(diagonalIndex);

                        setPixel(
                            x,
                            y,
                            particles[diagonalIndex]
                                .getMaterial()
                        );
                    }
                    else
                    {
                        clearPixel(x, y);
                        wakeNeighbors(x, y);
                    }

                    p.setPosition(nextX, nextY);

                    activateParticle(index);

                    setPixel(
                        nextX,
                        nextY,
                        p.getMaterial()
                    );

                    occupied[nextY][nextX] = index;

                    moved = true;
                    break;
                }
            }

            firstDirection *= -1;
        }

        if (moved)
        {
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
    // Horizontal spreading
    // -----------------------------------------

    float spread = p.getSpread();

    if (spread > 0.0f)
    {
        int maxSpread =
            static_cast<int>(spread * 10.0f);

        if (maxSpread < 1)
            maxSpread = 1;

        for (int distance = 1;
             distance <= maxSpread;
             distance++)
        {
            int leftX = p.getX() - distance;
            int rightX = p.getX() + distance;

            int y = p.getY();

            if (leftX >= 0 &&
                occupied[y][leftX] == -1)
            {
                int oldX = p.getX();
                int oldY = p.getY();

                occupied[oldY][oldX] = -1;
                clearPixel(oldX, oldY);

                wakeNeighbors(oldX, oldY);

                p.setPosition(leftX, oldY);

                occupied[oldY][leftX] = index;

                setPixel(
                    leftX,
                    oldY,
                    p.getMaterial()
                );

                activateParticle(index);

                break;
            }

            if (rightX < WIDTH &&
                occupied[y][rightX] == -1)
            {
                int oldX = p.getX();
                int oldY = p.getY();

                occupied[oldY][oldX] = -1;
                clearPixel(oldX, oldY);

                wakeNeighbors(oldX, oldY);

                p.setPosition(rightX, oldY);

                occupied[oldY][rightX] = index;

                setPixel(
                    rightX,
                    oldY,
                    p.getMaterial()
                );

                activateParticle(index);

                break;
            }
        }
    }

    // -----------------------------------------
    // Completely blocked
    // -----------------------------------------

    if (blocked && spread <= 0.0f)
    {
        p.stop();

        if (p.hasLifetime())
        {
            activateParticle(index);
        }
    }
}

