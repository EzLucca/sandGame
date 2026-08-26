#pragma once
#include "Material.h"

inline const Material fireMaterial{
    0.0f,
    1.0f,
    0.0f,
    false,
    -1,      // vertical diretion
    -200.0f,
    true,

    1.5f,       // lifetime
    1.0f,
    0.2f,
    0.0f,
    1.0f,

    true
};

inline const Material sandMaterial{
    0.8f,   // viscosity
    1.5f,   // density
    0.0f,   // spread
    true,   // affectedByGravity
    1,      // vertical diretion
    500.0f, // gravityValue
    true,   // movable

    -1.0f,  // lifetime
    1.0f,   // r
    0.8f,   // g
    0.1f,   // b
    1.0f,   // a
    false

};

inline const Material stoneMaterial{
    0.0f,   // viscosity
    2.5f,   // density
    0.0f,   // spread
    false,  // affectedByGravity
    0,      // vertical diretion
    0.0f,   // gravityValue
    false,  // movable

    -1.0f,  // lifetime
    0.5f,   // r
    0.5f,   // g
    0.5f,   // b
    1.0f,   // a
    false
};

inline const Material waterMaterial{
    0.2f,   // viscosity
    1.0f,   // density
    1.0f,   // spread
    true,   // affectedByGravity
    1,      // vertical diretion
    500.0f, // gravityValue
    true,   // movable

    -1.0f,  // lifetime
    0.0f,   // r
    0.4f,   // g
    1.0f,   // b
    1.0f,   // a
    false
};

inline const Material smokeMaterial{
    0.8f,   // viscosity
    0.1f,   // density
    2.5f,   // spread
    true,   // affectedByGravity
    -1,      // vertical diretion
    -200.0f, // gravityValue
    true,   // movable

    -1.0f,       // lifetime
    0.5f,   // r
    0.5f,   // g
    0.5f,   // b
    1.0f,   // a
    false
};
