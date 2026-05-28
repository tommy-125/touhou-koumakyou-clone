#include "Enemy/EnemyLaserManager.hpp"

#include <algorithm>
#include <cmath>

#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

// Compromise: lasers rendered as white rectangles (Util::Image has no tint support).

static constexpr const char WHITE_PNG[] = GA_RESOURCE_DIR "/white.png";

static void RemoveLaserObjects(EnemyLaser& l, Util::Renderer& renderer) {
    if (l.m_Obj) {
        renderer.RemoveChild(l.m_Obj);
        l.m_Obj = nullptr;
    }
    if (l.m_CoreObj) {
        renderer.RemoveChild(l.m_CoreObj);
        l.m_CoreObj = nullptr;
    }
    l.m_Img = nullptr;
}

static bool LaserSegmentOverlapsGameBounds(glm::vec2 pos, float angle, float startOffset,
                                           float visibleLength) {
    if (visibleLength <= 1.0f) return true;

    const glm::vec2 dir = {std::cos(angle), std::sin(angle)};
    const glm::vec2 a   = pos + dir * startOffset;
    const glm::vec2 b   = pos + dir * (startOffset + visibleLength);
    const float margin  = 128.0f;
    const float minX    = std::min(a.x, b.x);
    const float maxX    = std::max(a.x, b.x);
    const float minY    = std::min(a.y, b.y);
    const float maxY    = std::max(a.y, b.y);

    return maxX >= Util::GAME_BOUNDS_LEFT - margin &&
           minX <= Util::GAME_BOUNDS_RIGHT + margin &&
           maxY >= Util::GAME_BOUNDS_TOP - margin &&
           minY <= Util::GAME_BOUNDS_BOTTOM + margin;
}

static float DistanceSquaredToSegment(glm::vec2 point, glm::vec2 a, glm::vec2 b) {
    const glm::vec2 ab  = b - a;
    const glm::vec2 ap  = point - a;
    const float     len = ab.x * ab.x + ab.y * ab.y;
    if (len <= 0.0f) {
        const glm::vec2 delta = point - a;
        return delta.x * delta.x + delta.y * delta.y;
    }

    const float t = std::clamp((ap.x * ab.x + ap.y * ab.y) / len, 0.0f, 1.0f);
    const glm::vec2 closest = a + ab * t;
    const glm::vec2 delta   = point - closest;
    return delta.x * delta.x + delta.y * delta.y;
}

static bool LaserSegmentIntersectsRadiusRange(const EnemyLaser& l, glm::vec2 center,
                                              float innerRadius, float outerRadius) {
    const float endOffset = l.m_Speed == 0.0f ? l.m_Length : l.m_StartOffset + l.m_Offset;
    const float visibleLength =
        std::max(0.0f, std::min(l.m_Length, std::max(0.0f, endOffset)) - l.m_StartOffset);
    const float startOffset =
        l.m_Speed == 0.0f ? l.m_StartOffset : std::max(l.m_StartOffset, endOffset - l.m_Length);
    const glm::vec2 dir = {std::cos(l.m_Angle), std::sin(l.m_Angle)};
    const glm::vec2 a   = l.m_Pos + dir * startOffset;
    const glm::vec2 b   = l.m_Pos + dir * (startOffset + visibleLength);

    const float outer = outerRadius + l.m_CurWidth * 0.5f;
    if (DistanceSquaredToSegment(center, a, b) > outer * outer) return false;

    const float inner = std::max(0.0f, innerRadius - l.m_CurWidth * 0.5f);
    if (inner <= 0.0f) return true;

    const glm::vec2 da = a - center;
    const glm::vec2 db = b - center;
    return da.x * da.x + da.y * da.y >= inner * inner ||
           db.x * db.x + db.y * db.y >= inner * inner;
}

EnemyLaser* EnemyLaserManager::AllocLaser() {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) return &l;
    }
    EnemyLaser* l = &m_Lasers[m_NextIdx];
    m_NextIdx = (m_NextIdx + 1) % MAX_LASERS;
    RemoveLaserObjects(*l, m_Renderer);
    return l;
}

static void InitLaser(EnemyLaser* l, glm::vec2 pos, float angle, float length, float maxWidth,
                      int startTime, int duration, int endTime, int hitboxStart, int hitboxEnd,
                      float angularVelocity, float speed, int angularVelocityFrames,
                      float startOffset) {
    *l                   = EnemyLaser{};
    l->m_Alive           = true;
    l->m_Pos             = pos;
    l->m_Angle           = angle;
    l->m_Length          = length;
    l->m_MaxWidth        = maxWidth;
    l->m_CurWidth        = 0.0f;
    l->m_CoreWidth       = 0.0f;
    l->m_StartTime       = startTime;
    l->m_Duration        = duration;
    l->m_EndTime         = endTime;
    l->m_HitboxStart     = hitboxStart;
    l->m_HitboxEnd       = hitboxEnd;
    l->m_AngularVelocity = angularVelocity;
    l->m_AngularVelocityFrames = angularVelocityFrames;
    l->m_Speed           = speed;
    l->m_Offset          = 0.0f;
    l->m_StartOffset     = startOffset;
}

void EnemyLaserManager::SpawnAimed(glm::vec2 pos, glm::vec2 playerPos, float length, float maxWidth,
                                   int startTime, int duration, int endTime, int hitboxStart,
                                   int hitboxEnd, float speed) {
    EnemyLaser* l = AllocLaser();
    RemoveLaserObjects(*l, m_Renderer);
    float angle = std::atan2(playerPos.y - pos.y, playerPos.x - pos.x);
    InitLaser(l, pos, angle, length, maxWidth, startTime, duration, endTime, hitboxStart,
              hitboxEnd, 0.0f, speed, -1, 0.0f);
    l->m_Img = std::make_shared<Util::Image>(WHITE_PNG);
    l->m_Obj = std::make_shared<Util::GameObject>(l->m_Img, 3.0f);
    l->m_CoreObj = std::make_shared<Util::GameObject>(l->m_Img, 3.1f);
    l->m_Obj->SetVisible(false);
    l->m_CoreObj->SetVisible(false);
    m_Renderer.AddChild(l->m_Obj);
    m_Renderer.AddChild(l->m_CoreObj);
}

void EnemyLaserManager::SpawnAtAngle(glm::vec2 pos, float angle, float length, float maxWidth,
                                     int startTime, int duration, int endTime, int hitboxStart,
                                     int hitboxEnd, float angularVelocity, float speed,
                                     int angularVelocityFrames, float startOffset) {
    EnemyLaser* l = AllocLaser();
    RemoveLaserObjects(*l, m_Renderer);
    InitLaser(l, pos, angle, length, maxWidth, startTime, duration, endTime, hitboxStart,
              hitboxEnd, angularVelocity, speed, angularVelocityFrames, startOffset);
    l->m_Img = std::make_shared<Util::Image>(WHITE_PNG);
    l->m_Obj = std::make_shared<Util::GameObject>(l->m_Img, 3.0f);
    l->m_CoreObj = std::make_shared<Util::GameObject>(l->m_Img, 3.1f);
    l->m_Obj->SetVisible(false);
    l->m_CoreObj->SetVisible(false);
    m_Renderer.AddChild(l->m_Obj);
    m_Renderer.AddChild(l->m_CoreObj);
}

void EnemyLaserManager::Update() {
    if (m_TimeStopped) {
        Render();
        return;
    }

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
            RemoveLaserObjects(l, m_Renderer);
            continue;
        }
        l.m_CoreWidth = std::max(2.0f, l.m_CurWidth * 0.45f);

        if (l.m_AngularVelocityFrames < 0 || l.m_Timer <= l.m_AngularVelocityFrames) {
            l.m_Angle += l.m_AngularVelocity;
        }
        l.m_Offset += l.m_Speed;
        const float endOffset = l.m_Speed == 0.0f ? l.m_Length : l.m_StartOffset + l.m_Offset;
        const float visibleLength =
            std::min(l.m_Length, std::max(0.0f, endOffset)) - l.m_StartOffset;
        const float startOffset =
            l.m_Speed == 0.0f ? l.m_StartOffset : std::max(l.m_StartOffset, endOffset - l.m_Length);
        const float clampedVisibleLength = std::max(0.0f, visibleLength);

        if (l.m_Speed != 0.0f &&
            !LaserSegmentOverlapsGameBounds(l.m_Pos, l.m_Angle, startOffset,
                                            clampedVisibleLength)) {
            l.m_Alive = false;
            RemoveLaserObjects(l, m_Renderer);
            continue;
        }

        if (l.m_Obj) {
            // Center of laser in screen coords, then convert to PTSD
            float cx = l.m_Pos.x + std::cos(l.m_Angle) * (startOffset + clampedVisibleLength * 0.5f);
            float cy = l.m_Pos.y + std::sin(l.m_Angle) * (startOffset + clampedVisibleLength * 0.5f);
            float px = cx - 320.0f;
            float py = 240.0f - cy;

            l.m_Obj->SetVisible(true);
            l.m_Obj->m_Transform.translation = {px, py};
            l.m_Obj->m_Transform.rotation    = -l.m_Angle;
            l.m_Obj->m_Transform.scale       = {std::max(1.0f, clampedVisibleLength) / 4.0f,
                                                l.m_CurWidth / 4.0f};
        }
        if (l.m_CoreObj) {
            float cx = l.m_Pos.x + std::cos(l.m_Angle) * (startOffset + clampedVisibleLength * 0.5f);
            float cy = l.m_Pos.y + std::sin(l.m_Angle) * (startOffset + clampedVisibleLength * 0.5f);
            float px = cx - 320.0f;
            float py = 240.0f - cy;

            l.m_CoreObj->SetVisible(true);
            l.m_CoreObj->m_Transform.translation = {px, py};
            l.m_CoreObj->m_Transform.rotation    = -l.m_Angle;
            l.m_CoreObj->m_Transform.scale       = {std::max(1.0f, clampedVisibleLength) / 4.0f,
                                                    l.m_CoreWidth / 4.0f};
        }
    }
    m_Renderer.Update();
}

void EnemyLaserManager::Render() {
    m_Renderer.Update();
}

bool EnemyLaserManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    if (m_TimeStopped) return false;

    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;
        // Hitbox active window
        if (l.m_Timer < l.m_HitboxStart) continue;
        const int hitboxEndFrame = l.m_StartTime + l.m_Duration + l.m_EndTime - l.m_HitboxEnd;
        if (l.m_Timer >= hitboxEndFrame) continue;
        if (l.m_CurWidth < 2.0f) continue;

        // OBB point test: rotate player pos into laser local space
        const float endOffset = l.m_Speed == 0.0f ? l.m_Length : l.m_StartOffset + l.m_Offset;
        const float visibleLength =
            std::max(0.0f, std::min(l.m_Length, std::max(0.0f, endOffset)) - l.m_StartOffset);
        const float startOffset =
            l.m_Speed == 0.0f ? l.m_StartOffset : std::max(l.m_StartOffset, endOffset - l.m_Length);
        const glm::vec2 origin = {
            l.m_Pos.x + std::cos(l.m_Angle) * startOffset,
            l.m_Pos.y + std::sin(l.m_Angle) * startOffset,
        };
        float dx   = playerPos.x - origin.x;
        float dy   = playerPos.y - origin.y;
        float cosA = std::cos(-l.m_Angle);
        float sinA = std::sin(-l.m_Angle);
        float lx   = dx * cosA - dy * sinA;
        float ly   = dx * sinA + dy * cosA;
        float hw   = visibleLength * 0.5f + playerHitboxSize.x;
        float hh   = l.m_CurWidth * 0.5f + playerHitboxSize.y;
        if (std::abs(lx - visibleLength * 0.5f) < hw && std::abs(ly) < hh) return true;
    }
    return false;
}

void EnemyLaserManager::ClearAll() {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;
        l.m_Alive = false;
        RemoveLaserObjects(l, m_Renderer);
    }
}

void EnemyLaserManager::TurnAllLasersIntoPointItems(ItemManager& items) {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;

        const float endOffset = l.m_Speed == 0.0f ? l.m_Length : l.m_StartOffset + l.m_Offset;
        const float visibleLength =
            std::max(0.0f, std::min(l.m_Length, std::max(0.0f, endOffset)) - l.m_StartOffset);
        const float startOffset =
            l.m_Speed == 0.0f ? l.m_StartOffset : std::max(l.m_StartOffset, endOffset - l.m_Length);
        const glm::vec2 origin = {
            l.m_Pos.x + std::cos(l.m_Angle) * startOffset,
            l.m_Pos.y + std::sin(l.m_Angle) * startOffset,
        };
        items.SpawnItem(origin, ItemType::PointBullet, 1);
        for (float offset = 0.0f; offset <= visibleLength; offset += 32.0f) {
            glm::vec2 itemPos = {
                origin.x + std::cos(l.m_Angle) * offset,
                origin.y + std::sin(l.m_Angle) * offset,
            };
            items.SpawnItem(itemPos, ItemType::PointBullet, 1);
        }

        l.m_Alive = false;
        RemoveLaserObjects(l, m_Renderer);
    }
}

void EnemyLaserManager::TurnLasersIntoPointItemsInRadiusRange(ItemManager& items,
                                                              glm::vec2 center,
                                                              float innerRadius,
                                                              float outerRadius) {
    for (auto& l : m_Lasers) {
        if (!l.m_Alive) continue;
        if (!LaserSegmentIntersectsRadiusRange(l, center, innerRadius, outerRadius)) continue;

        const float endOffset = l.m_Speed == 0.0f ? l.m_Length : l.m_StartOffset + l.m_Offset;
        const float visibleLength =
            std::max(0.0f, std::min(l.m_Length, std::max(0.0f, endOffset)) - l.m_StartOffset);
        const float startOffset =
            l.m_Speed == 0.0f ? l.m_StartOffset : std::max(l.m_StartOffset, endOffset - l.m_Length);
        const glm::vec2 origin = {
            l.m_Pos.x + std::cos(l.m_Angle) * startOffset,
            l.m_Pos.y + std::sin(l.m_Angle) * startOffset,
        };
        items.SpawnItem(origin, ItemType::PointBullet, 1);
        for (float offset = 0.0f; offset <= visibleLength; offset += 32.0f) {
            glm::vec2 itemPos = {
                origin.x + std::cos(l.m_Angle) * offset,
                origin.y + std::sin(l.m_Angle) * offset,
            };
            items.SpawnItem(itemPos, ItemType::PointBullet, 1);
        }

        l.m_Alive = false;
        RemoveLaserObjects(l, m_Renderer);
    }
}
