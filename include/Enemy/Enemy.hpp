#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <glm/glm.hpp>

#include "Anm/AnmTypes.hpp"

struct Enemy {
    // Animation
    Anm::Vm m_Vm;

    // Position & physics (th06 coordinates: top-left origin, y-down)
    glm::vec2 m_Pos             = {0, 0};
    float     m_Angle           = 0;  // movement direction (radians)
    float     m_Speed           = 0;  // movement speed (pixels/frame)
    float     m_AngularVelocity = 0;  // angle change per frame
    float     m_Acceleration    = 0;  // speed change per frame

    // Combat
    glm::vec2 m_HitboxSize = {12, 12};
    int       m_Life       = 1;
    int       m_Score      = 100;

    // State
    bool m_Alive      = false;
    int  m_SubId      = -1;
    int  m_FrameTimer = 0;
    bool m_Mirrored   = false;  // set by enemy_create_mirror
    int  m_ItemDrop   = -1;     // -1=random, 0=PowerSmall, 2=PowerBig
};

#endif  // ENEMY_HPP
