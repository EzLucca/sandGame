#include "Simulation.h"
#include "Movements.h"

#include <algorithm>
#include <cmath>
#include <chrono>

#include "Movements.h"
#include "Material.h"

enum class ExplosionStage
{
    None,
    White,
    Red
};

namespace Movements
{
    void drawTemporaryCircle(
            Simulation& simulation,
            int centerX,
            int centerY,
            int radius,
            const Material& material)
    {
        for (int dy = -radius; dy <= radius; ++dy)
        {
            for (int dx = -radius; dx <= radius; ++dx)
            {
                if (dx * dx + dy * dy <= radius * radius)
                {
                    int x = centerX + dx;
                    int y = centerY + dy;

                    if (x >= 0 && x < Simulation::WIDTH &&
                            y >= 0 && y < Simulation::HEIGHT)
                    {
                        simulation.addParticle(x, y, material);
                    }
                }
            }
        }
    }

struct Explosion
    {
        bool active = false;

        int x = 0;
        int y = 0;
        int radius = 0;

        ExplosionStage stage = ExplosionStage::None;

        std::chrono::steady_clock::time_point startTime;
    };

    static Explosion explosion;

    void triggerExplosion(
        Simulation& simulation,
        int x,
        int y,
        int radius)
    {
        // Don't restart an existing explosion
        if (explosion.active)
            return;

        explosion.active = true;
        explosion.x = x;
        explosion.y = y;
        explosion.radius = radius;
        explosion.stage = ExplosionStage::White;
        explosion.startTime = std::chrono::steady_clock::now();

        drawTemporaryCircle(
            simulation,
            x,
            y,
            radius,
            whiteBombMaterial
        );
    }

    void updateExplosion(Simulation& simulation)
    {
        if (!explosion.active)
            return;

        float elapsed =
            std::chrono::duration<float>(
                std::chrono::steady_clock::now()
                - explosion.startTime
            ).count();

        // White stage finished
        if (explosion.stage == ExplosionStage::White &&
            elapsed >= whiteBombMaterial.lifetime)
        {
            explosion.stage = ExplosionStage::Red;

            drawTemporaryCircle(
                simulation,
                explosion.x,
                explosion.y,
                explosion.radius,
                redBombMaterial
            );
        }

        // Red stage finished
        if (explosion.stage == ExplosionStage::Red &&
            elapsed >=
                whiteBombMaterial.lifetime +
                redBombMaterial.lifetime)
        {
            explosion.stage = ExplosionStage::None;
            explosion.active = false;
        }
    }}

