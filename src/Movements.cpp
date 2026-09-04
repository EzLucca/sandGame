#include "Simulation.h"
#include "Movements.h"

#include <algorithm>
#include <cmath>
#include <chrono>

#include "Movements.h"
#include "Material.h"


namespace Movements
{
    struct Explosion
    {
        bool active = false;

        int x = 0;
        int y = 0;
        int radius = 0;

        ExplosionStage stage = ExplosionStage::None;

        std::chrono::steady_clock::time_point stageStartTime;
    };

    static Explosion explosion;

    void triggerExplosion( Simulation& simulation, int x, int y, int radius)
    {
        // Don't restart an existing explosion
        if (explosion.active)
            return;

        explosion.active = true;
        explosion.x = x;
        explosion.y = y;
        explosion.radius = radius;

        explosion.stage = ExplosionStage::White;
        explosion.stageStartTime = std::chrono::steady_clock::now();
    }

    void updateExplosion(Simulation& simulation)
    {
        if (!explosion.active)
            return;

        float elapsed =
            std::chrono::duration<float>(
                    std::chrono::steady_clock::now()
                    - explosion.stageStartTime
                    ).count();

        if (explosion.stage == ExplosionStage::White)
        {
            if (elapsed >= whiteBombMaterial.lifetime)
            {
                explosion.stage = ExplosionStage::Red;

                explosion.stageStartTime =
                    std::chrono::steady_clock::now();
            }
        }
        else if (explosion.stage == ExplosionStage::Red)
        {
            if (elapsed >= redBombMaterial.lifetime)
            {
                simulation.removeParticlesInRadius(
                        explosion.x,
                        explosion.y,
                        explosion.radius
                        );

                explosion.stage = ExplosionStage::None;
                explosion.active = false;
            }
        }
    }


    bool isExplosionActive()
    {
        return explosion.active;
    }

    ExplosionStage getExplosionStage()
    {
        return explosion.stage;
    }

    int getExplosionX()
    {
        return explosion.x;
    }

    int getExplosionY()
    {
        return explosion.y;
    }

    int getExplosionRadius()
    {
        return explosion.radius;
    }

    struct Launch
    {
        bool active = false;

        float x = 0.0f;
        float y = 0.0f;

        float velocityX = 0.0f;
        float velocityY = 0.0f;
    };

    static Launch launch;

    void launchParticle( Simulation& simulation, int x, int y)
    {
        if (launch.active)
            return;

        launch.active = true;

        launch.x = static_cast<float>(x);
        launch.y = static_cast<float>(y);

        launch.velocityY = -300.0f;
    }

    void updateLaunch( Simulation& simulation, float deltaTime)
    {
        if (!launch.active)
            return;

        launch.y += launch.velocityY * deltaTime;

        // Slow down as it rises
        launch.velocityY += 200.0f * deltaTime;

        const int x = static_cast<int> (launch.x);
        const int y = static_cast<int> (launch.y);

        constexpr int collisionRadius = 4;
        bool collision = false;

        for (int dy = -collisionRadius; dy <= collisionRadius && !collision; ++dy)
        {
            for (int dx = -collisionRadius; dx <= collisionRadius; ++dx)
            {
                if (dx * dx + dy * dy > collisionRadius * collisionRadius)
                    continue;

                if (simulation.isOccupied(x + dx, y + dy))
                {
                    collision = true;
                    break;
                }
            }
        }

        if (collision)
        {
            launch.active = false;
            triggerExplosion( simulation, x, y, 50);

            return;
        }

        // Explode at highest point
        if (launch.velocityY >= 0.0f)
        {
            launch.active = false;
            triggerExplosion( simulation, x, y, 50);
        }
    }

    bool isLaunchActive()
    {
        return launch.active;
    }

    int getLaunchX()
    {
        return static_cast<int>(launch.x);
    }

    int getLaunchY()
    {
        return static_cast<int>(launch.y);
    }
}

