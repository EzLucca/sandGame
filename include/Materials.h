#pragma once
#include "Material.h"

inline const Material fireMaterial{
    0.0f,                       // viscosity
    1.0f,                       // density
    1.0f,                       // spread
    false,                      // affectedByGravity
    -1,                         // vertical diretion
    -2.0f,                      // gravityValue
    true,                       // movable
    1.0f,                       // lifetime
    1.0f, 0.2f, 0.0f, 1.0f,     // rgba
    true
};

inline const Material sandMaterial{
    0.8f,                       // viscosity
    1.5f,                       // density
    0.0f,                       // spread
    true,                       // affectedByGravity
    1,                          // vertical diretion
    1.0f,                       // gravityValue
    true,                       // movable
    -1.0f,                      // lifetime
    1.0f, 1.8f, 1.1f, 1.0f,     // rgba    
    false

};

inline const Material stoneMaterial{
    0.0f,                       // viscosity
    10.0f,                       // density
    0.0f,                       // spread
    false,                      // affectedByGravity
    0,                          // vertical diretion
    0.0f,                       // gravityValue
    false,                      // movable
    -1.0f,                      // lifetime
    0.5f, 0.5f, 0.5f, 1.0f,     // rgba
    false
};

inline const Material waterMaterial{
    0.0f,                       // viscosity
    1.0f,                       // density
    2.0f,                       // spread
    true,                       // affectedByGravity
    1,                          // vertical diretion
    1.0f,                       // gravityValue
    true,                       // movable
    -1.0f,                      // lifetime
    0.0f, 0.4f, 1.0f, 1.0f,     // rgba
    false
};

inline const Material smokeMaterial{
    0.8f,                       // viscosity
    0.1f,                       // density
    2.5f,                       // spread
    true,                       // affectedByGravity
    -1,                         // vertical diretion
    -2.0f,                    // gravityValue
    true,                       // movable
    -1.0f,                      // lifetime
    0.2f, 0.2f, 0.2f, 1.0f,     // rgba
    false
};
