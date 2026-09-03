#pragma once
#include "Simulation.h"

namespace Movements {
    void drawTemporaryCircle(Simulation& simulation,
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
}
