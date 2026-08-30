#include "Simulation.h"
#include "Movements.h"

#include <algorithm>
#include <cmath>

#include "Movements.h"
#include "Material.h"

namespace Movements {
    void drawTemporaryCircle(Simulation& simulation, int centerX, int centerY, int radius, float durationSeconds)
    {
        Material whiteEffect;
        whiteEffect.r = 1.0f;
        whiteEffect.g = 1.0f;
        whiteEffect.b = 1.0f;
        whiteEffect.a = 1.0f;
        whiteEffect.movable = false;
        whiteEffect.affectedByGravity = false;
        whiteEffect.spread = 0.0f;
        whiteEffect.lifetime = durationSeconds;
        whiteEffect.isFire = false;

        // Note: If you need to temporarily override the selected material, 
        // you can add a public helper in Simulation or use public methods.
        // Alternatively, add a Simulation method like `simulation.spawnCustomParticle(x, y, whiteEffect);`
        
        for (int dy = -radius; dy <= radius; dy++)
        {
            for (int dx = -radius; dx <= radius; dx++)
            {
                if (dx * dx + dy * dy <= radius * radius)
                {
                    int x = centerX + dx;
                    int y = centerY + dy;

                    if (x >= 0 && x < Simulation::WIDTH && y >= 0 && y < Simulation::HEIGHT)
                    {
                        // Call your simulation methods here using the reference
                        // simulation.placeParticle(x, y); 
                    }
                }
            }
        }
    }
}
