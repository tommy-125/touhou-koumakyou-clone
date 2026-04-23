#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <glm/glm.hpp>

#include "Anm/AnmTypes.hpp"

struct Enemy {
    // Animation
    Anm::Vm m_Vm;

    // Position & physics (screen coords: field top-left = (32,16))
    glm::vec2 m_Pos             = {0, 0};
    float     m_Angle           = 0;
    float     m_Speed           = 0;
    float     m_AngularVelocity = 0;
    float     m_Acceleration    = 0;

    // Combat
    glm::vec2 m_HitboxSize = {12, 12};
    int       m_Life       = 1;
    int       m_Score      = 100;

    // State
    bool m_Alive         = false;
    int  m_SubId         = -1;
    int  m_FrameTimer    = 0;
    bool m_Mirrored      = false;
    int  m_ItemDrop      = -1;
    int  m_ItemDropCount = 1;

    // Boss
    bool m_IsBoss           = false;
    bool m_CanTakeDamage    = true;
    bool m_InSpellcard      = false;  // damage /7 during spellcard (TH6 behavior)
    int  m_BossTimer        = 0;
    int  m_BossMaxLife      = 1;
    int  m_DeathCallbackSub = -1;

    int m_LifeCallbackThreshold  = -1;
    int m_LifeCallbackSub        = -1;
    int m_TimerCallbackThreshold = -1;
    int m_TimerCallbackSub       = -1;

    // Movement lerp (ease-out quad: pos = origin + (1-t²)*(target-origin), t: 1→0)
    bool      m_IsLerping   = false;
    glm::vec2 m_LerpOrigin  = {};
    glm::vec2 m_LerpTarget  = {};
    int       m_LerpFrames  = 0;
    int       m_LerpElapsed = 0;

    // Bounds for move_rand_in_bounds (screen coords)
    glm::vec2 m_BoundsMin = {64.0f, 64.0f};
    glm::vec2 m_BoundsMax = {384.0f, 160.0f};

    // When true, RunTimeline pauses while this enemy is alive (mid-boss / boss)
    bool m_BlocksTimeline = false;
};

#endif  // ENEMY_HPP
