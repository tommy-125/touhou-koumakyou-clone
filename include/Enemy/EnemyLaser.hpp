#ifndef ENEMY_LASER_HPP
#define ENEMY_LASER_HPP

#include <glm/glm.hpp>
#include <memory>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

struct EnemyLaser {
    glm::vec2 m_Pos      = {};
    float     m_Angle    = 0;
    float     m_Length   = 500.0f;
    float     m_MaxWidth = 16.0f;
    float     m_CurWidth = 0.0f;

    // Timing (frames)
    int m_StartTime   = 120;  // grow phase
    int m_Duration    = 60;   // full-width phase
    int m_EndTime     = 14;   // shrink phase
    int m_HitboxStart = 16;
    int m_HitboxEnd   = 120;

    float m_AngularVelocity = 0.0f;
    int   m_Timer           = 0;
    bool  m_Alive           = false;

    std::shared_ptr<Util::GameObject> m_Obj;
    std::shared_ptr<Util::Image>      m_Img;
};

#endif  // ENEMY_LASER_HPP
