#include "Enemy/EnemySubCtx.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

void EnemySubCtx::TransitionToSub(Enemy& e, int newSub) const {
    e.m_SubId      = newSub;
    e.m_FrameTimer = -1;
}

Enemy* EnemySubCtx::SpawnEnemy(int subId, float x, float y, int life, int score, bool mirrored,
                               int itemDrop) const {
    if (!spawnEnemy) return nullptr;
    return spawnEnemy(subId, x, y, life, score, mirrored, itemDrop);
}

void EnemySubCtx::KillAllNonBossEnemies() const {
    if (killAllNonBossEnemies) killAllNonBossEnemies();
}

void EnemySubCtx::SetTimeStopped(bool stopped) const {
    if (setTimeStopped) setTimeStopped(stopped);
}

void EnemySubCtx::RedirectTimeStopBullets(int maxBullets) const {
    if (redirectTimeStopBullets) redirectTimeStopBullets(maxBullets);
}

void EnemySubCtx::BulletCancelIntoPointItems() const {
    bullets.TurnAllBulletsIntoPointItems(items);
    lasers.TurnAllLasersIntoPointItems(items);
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
    if (e.m_BoundsMax.x > e.m_BoundsMin.x && e.m_BoundsMax.y > e.m_BoundsMin.y) {
        e.m_LerpTarget.x = std::clamp(e.m_LerpTarget.x, e.m_BoundsMin.x, e.m_BoundsMax.x);
        e.m_LerpTarget.y = std::clamp(e.m_LerpTarget.y, e.m_BoundsMin.y, e.m_BoundsMax.y);
    }
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
