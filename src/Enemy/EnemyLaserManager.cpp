#include "Enemy/EnemyLaserManager.hpp"

#include <algorithm>
#include <cmath>

// Compromise: lasers rendered as white rectangles (Util::Image has no tint support).

static constexpr const char WHITE_PNG[] = GA_RESOURCE_DIR "/white.png";

EnemyLaser* EnemyLaserManager::AllocLaser() {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) return &l;
    }
    return &m_Lasers[0];  // overwrite oldest
}

static void InitLaser(EnemyLaser* l, glm::vec2 pos, float angle, float length, float maxWidth,
                      int startTime, int duration, int endTime, int hitboxStart, int hitboxEnd,
                      float angularVelocity) {
    *l                   = EnemyLaser{};
    l->m_Alive           = true;
    l->m_Pos             = pos;
    l->m_Angle           = angle;
    l->m_Length          = length;
    l->m_MaxWidth        = maxWidth;
    l->m_CurWidth        = 0.0f;
    l->m_StartTime       = startTime;
    l->m_Duration        = duration;
    l->m_EndTime         = endTime;
    l->m_HitboxStart     = hitboxStart;
    l->m_HitboxEnd       = hitboxEnd;
    l->m_AngularVelocity = angularVelocity;
}

void EnemyLaserManager::SpawnAimed(glm::vec2 pos, glm::vec2 playerPos, float length, float maxWidth,
                                   int startTime, int duration, int endTime, int hitboxStart,
                                   int hitboxEnd) {
    EnemyLaser* l = AllocLaser();
    if (l->m_Obj) m_Renderer.RemoveChild(l->m_Obj);
    float angle = std::atan2(playerPos.y - pos.y, playerPos.x - pos.x);
    InitLaser(l, pos, angle, length, maxWidth, startTime, duration, endTime, hitboxStart, hitboxEnd,
              0.0f);
    l->m_Img = std::make_shared<Util::Image>(WHITE_PNG);
    l->m_Obj = std::make_shared<Util::GameObject>(l->m_Img, 3.0f);
    l->m_Obj->SetVisible(false);
    m_Renderer.AddChild(l->m_Obj);
}

void EnemyLaserManager::SpawnAtAngle(glm::vec2 pos, float angle, float length, float maxWidth,
                                     int startTime, int duration, int endTime, int hitboxStart,
                                     int hitboxEnd, float angularVelocity) {
    EnemyLaser* l = AllocLaser();
    if (l->m_Obj) m_Renderer.RemoveChild(l->m_Obj);
    InitLaser(l, pos, angle, length, maxWidth, startTime, duration, endTime, hitboxStart, hitboxEnd,
              angularVelocity);
    l->m_Img = std::make_shared<Util::Image>(WHITE_PNG);
    l->m_Obj = std::make_shared<Util::GameObject>(l->m_Img, 3.0f);
    l->m_Obj->SetVisible(false);
    m_Renderer.AddChild(l->m_Obj);
}

void EnemyLaserManager::Update() {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;

        l.m_Timer++;
        int t  = l.m_Timer;
        int t1 = l.m_StartTime;
        int t2 = t1 + l.m_Duration;
        int t3 = t2 + l.m_EndTime;

        if (t < t1) {
            int rampStart = t1 - std::min(t1, 30);
            if (t >= rampStart) {
                l.m_CurWidth = l.m_MaxWidth * (float)t / (float)t1;
            } else {
                l.m_CurWidth = 1.2f;
            }
        } else if (t < t2) {
            l.m_CurWidth = l.m_MaxWidth;
        } else if (t < t3) {
            l.m_CurWidth = l.m_MaxWidth * (float)(t3 - t) / (float)l.m_EndTime;
        } else {
            l.m_Alive = false;
            if (l.m_Obj) {
                m_Renderer.RemoveChild(l.m_Obj);
                l.m_Obj = nullptr;
                l.m_Img = nullptr;
            }
            continue;
        }

        l.m_Angle += l.m_AngularVelocity;

        if (l.m_Obj) {
            // Center of laser in screen coords, then convert to PTSD
            float cx = l.m_Pos.x + std::cos(l.m_Angle) * l.m_Length * 0.5f;
            float cy = l.m_Pos.y + std::sin(l.m_Angle) * l.m_Length * 0.5f;
            float px = cx - 320.0f;
            float py = 240.0f - cy;

            l.m_Obj->SetVisible(true);
            l.m_Obj->m_Transform.translation = {px, py};
            l.m_Obj->m_Transform.rotation    = -l.m_Angle;
            l.m_Obj->m_Transform.scale       = {l.m_Length / 4.0f, l.m_CurWidth / 4.0f};
        }
    }
    m_Renderer.Update();
}

bool EnemyLaserManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;
        // Hitbox active window
        if (l.m_Timer < l.m_HitboxStart) continue;
        if (l.m_Timer >= l.m_StartTime + l.m_Duration) continue;
        if (l.m_CurWidth < 2.0f) continue;

        // OBB point test: rotate player pos into laser local space
        float dx   = playerPos.x - l.m_Pos.x;
        float dy   = playerPos.y - l.m_Pos.y;
        float cosA = std::cos(-l.m_Angle);
        float sinA = std::sin(-l.m_Angle);
        float lx   = dx * cosA - dy * sinA;
        float ly   = dx * sinA + dy * cosA;
        float hw   = l.m_Length * 0.5f + playerHitboxSize.x;
        float hh   = l.m_CurWidth * 0.5f + playerHitboxSize.y;
        if (std::abs(lx - l.m_Length * 0.5f) < hw && std::abs(ly) < hh) return true;
    }
    return false;
}

void EnemyLaserManager::ClearAll() {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;
        l.m_Alive = false;
        if (l.m_Obj) {
            m_Renderer.RemoveChild(l.m_Obj);
            l.m_Obj = nullptr;
            l.m_Img = nullptr;
        }
    }
}
