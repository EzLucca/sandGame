#pragma once
#include "Particle.h"

bool canDisplace(int particleIndex, int otherIndex);

void wakeNeighbors(int x, int y);

void moveParticle(int index, int newX, int newY);

void removeParticle(int index);

void updateParticle( Particle& p, int index, float deltaTime);
