#pragma once
#include "Simulation.h"

enum class ExplosionStage
{
    None,
    White,
    Red
};

enum class LaunchStage
{
    None,
    Flying,
    Explosion
};

namespace Movements {

    // ----- Explosion -----

    bool isExplosionActive();
    ExplosionStage getExplosionStage();
    int  getExplosionX();
    int  getExplosionY();
    int  getExplosionRadius();

    void drawCircle(Simulation& simulation,
            int centerX,
            int centerY,
            int radius,
            const Material& material);
    void triggerExplosion(
            Simulation& simulation,
            int x,
            int y,
            int radius);

    void updateExplosion(Simulation& simulation);

    // ----- Launch -----
    // void launchParticle( Simulation& simulation, int startX, int startY, int targetX, int targetY);
    void launchParticle( Simulation& simulation, int startX, int startY);
    void updateLaunch( Simulation& simulation, float deltaTime);
    bool isLaunchActive();
    int getLaunchX();
    int getLaunchY();
}
