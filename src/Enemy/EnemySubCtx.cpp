#include "Enemy/EnemySubCtx.hpp"

#include <cmath>
#include <cstdlib>

#include "Enemy/Enemy.hpp"
#include "Util/Math.hpp"

void EnemySubCtx::TransitionToSub(Enemy& e, int newSub) const {
    e.m_SubId      = newSub;
    e.m_FrameTimer = -1;
}

void EnemySubCtx::StartLerpTo(Enemy& e, float targetX, float targetY, int frames) const {
    e.m_IsLerping   = true;
    e.m_LerpOrigin  = e.m_Pos;
    e.m_LerpTarget  = Util::GameFieldToScreen(targetX, targetY);
    e.m_LerpFrames  = frames;
    e.m_LerpElapsed = 0;
}

void EnemySubCtx::StartLerpDir(Enemy& e, float speed, int frames) const {
    e.m_IsLerping  = true;
    e.m_LerpOrigin = e.m_Pos;
    e.m_LerpTarget =
        e.m_Pos + glm::vec2{std::cos(e.m_Angle), std::sin(e.m_Angle)} * (speed * frames / 2.0f);
    e.m_LerpFrames  = frames;
    e.m_LerpElapsed = 0;
    e.m_Speed       = 0;
}

void EnemySubCtx::MoveRandInBounds(Enemy& e) const {
    float angle =
        ((float)rand() / RAND_MAX) * 2.0f * (Util::HALF_PI * 2.0f) - (Util::HALF_PI * 2.0f);

    float mx = (e.m_BoundsMax.x - e.m_BoundsMin.x) * 0.25f;
    float my = (e.m_BoundsMax.y - e.m_BoundsMin.y) * 0.25f;

    if (e.m_Pos.x < e.m_BoundsMin.x + mx && std::cos(angle) < 0)
        angle = std::atan2(std::sin(angle), -std::cos(angle));
    if (e.m_Pos.x > e.m_BoundsMax.x - mx && std::cos(angle) > 0)
        angle = std::atan2(std::sin(angle), -std::cos(angle));
    if (e.m_Pos.y < e.m_BoundsMin.y + my && std::sin(angle) < 0)
        angle = std::atan2(-std::sin(angle), std::cos(angle));
    if (e.m_Pos.y > e.m_BoundsMax.y - my && std::sin(angle) > 0)
        angle = std::atan2(-std::sin(angle), std::cos(angle));

    e.m_Angle = angle;
}
