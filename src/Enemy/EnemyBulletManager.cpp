#include "Enemy/EnemyBulletManager.hpp"

#include <cmath>

#include "Util/Math.hpp"

EnemyBulletManager::EnemyBulletManager() {
    m_Anm.LoadAnm(Anm::ETAMA3.folder, Anm::ETAMA3.txt, Anm::ETAMA3.offset);
}

EnemyBullet* EnemyBulletManager::AllocBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        int idx = (m_NextIdx + i) % MAX_BULLETS;
        if (!m_Bullets[idx].m_Alive) {
            m_NextIdx = (idx + 1) % MAX_BULLETS;
            return &m_Bullets[idx];
        }
    }
    // Pool full: overwrite oldest
    EnemyBullet* b = &m_Bullets[m_NextIdx];
    if (b->m_Vm.obj) {
        m_Renderer.RemoveChild(b->m_Vm.obj);
        b->m_Vm.obj = nullptr;
    }
    m_NextIdx = (m_NextIdx + 1) % MAX_BULLETS;
    return b;
}

void EnemyBulletManager::SpawnFanAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                       EBulletColor color, int count, float speed, float aimOffset,
                                       float spread, bool useDecay, bool rotateWithAngle) {
    int scriptIdx = Anm::ETAMA3.offset + static_cast<int>(type);
    int sprOffset = Anm::ETAMA3.offset + static_cast<int>(color);

    float aimAngle = std::atan2(playerPos.y - pos.y, playerPos.x - pos.x) + aimOffset;

    for (int i = 0; i < count; i++) {
        // Matches TH6 BulletManager: odd count centers on aim axis, even count straddles it.
        // Pairs bullets symmetrically: 0, ±1, ±2, ... (odd) or ±0.5, ±1.5, ... (even).
        // Integer division is intentional — gives TH6's symmetric pairing.
        int   step  = (count & 1) ? (i + 1) / 2 : i / 2;
        float delta = static_cast<float>(step) * spread;
        if (!(count & 1)) delta += spread * 0.5f;
        if (i & 1) delta = -delta;

        EnemyBullet* b       = AllocBullet();
        *b                   = EnemyBullet{};
        b->m_Alive           = true;
        b->m_Pos             = pos;
        b->m_Angle           = aimAngle + delta;
        b->m_Speed           = speed;
        b->m_UseDecay        = useDecay;
        b->m_RotateWithAngle = rotateWithAngle;

        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

void EnemyBulletManager::SpawnFanStack(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                       EBulletColor color, int ways, int stacks, float speed1,
                                       float speed2, float aimOffset, float spread) {
    for (int s = 0; s < stacks; s++) {
        float speed = speed1 - (speed1 - speed2) * s / stacks;
        SpawnFanAimed(pos, playerPos, type, color, ways, speed, aimOffset, spread);
    }
}

void EnemyBulletManager::SpawnCircleAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                          EBulletColor color, int count, float speed,
                                          float aimOffset, bool useDecay, float acceleration,
                                          BulletCurve curve, bool rotateWithAngle) {
    int   scriptIdx = Anm::ETAMA3.offset + static_cast<int>(type);
    int   sprOffset = Anm::ETAMA3.offset + static_cast<int>(color);
    float aimAngle  = std::atan2(playerPos.y - pos.y, playerPos.x - pos.x) + aimOffset;
    float step      = 2.0f * Util::HALF_PI * 2.0f / count;

    for (int i = 0; i < count; i++) {
        EnemyBullet* b         = AllocBullet();
        *b                     = EnemyBullet{};
        b->m_Alive             = true;
        b->m_Pos               = pos;
        b->m_Angle             = aimAngle + i * step;
        b->m_Speed             = speed;
        b->m_UseDecay          = useDecay;
        b->m_Acceleration      = acceleration;
        b->m_DirChangeAt       = curve.at;
        b->m_DirChangeAngle    = curve.angle;
        b->m_DirChangeSpeed    = curve.speed;
        b->m_DirChangeRelative = curve.relative;
        b->m_RotateWithAngle   = rotateWithAngle;
        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

void EnemyBulletManager::SpawnCircle(glm::vec2 pos, EBulletType type, EBulletColor color, int count,
                                     float speed, float baseAngle, bool useDecay) {
    int   scriptIdx = Anm::ETAMA3.offset + static_cast<int>(type);
    int   sprOffset = Anm::ETAMA3.offset + static_cast<int>(color);
    float step      = 2.0f * Util::HALF_PI * 2.0f / count;

    for (int i = 0; i < count; i++) {
        EnemyBullet* b = AllocBullet();
        *b             = EnemyBullet{};
        b->m_Alive     = true;
        b->m_Pos       = pos;
        b->m_Angle     = baseAngle + i * step;
        b->m_Speed     = speed;
        b->m_UseDecay  = useDecay;
        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

bool EnemyBulletManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;
        float dx = std::abs(b.m_Pos.x - playerPos.x);
        float dy = std::abs(b.m_Pos.y - playerPos.y);
        // bullet m_HitboxSize is full width (SetVecCorners), player hitbox is already a radius.
        if (dx < b.m_HitboxSize.x * 0.5f + playerHitboxSize.x &&
            dy < b.m_HitboxSize.y * 0.5f + playerHitboxSize.y) {
            return true;
        }
    }
    return false;
}

void EnemyBulletManager::ClearAll() {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;
        b.m_Alive = false;
        if (b.m_Vm.obj) {
            m_Renderer.RemoveChild(b.m_Vm.obj);
            b.m_Vm.obj = nullptr;
        }
    }
}

void EnemyBulletManager::Update() {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;

        if (b.m_DirChangeAt >= 0 && b.m_DecayTimer == b.m_DirChangeAt) {
            b.m_Angle = b.m_DirChangeRelative ? b.m_Angle + b.m_DirChangeAngle : b.m_DirChangeAngle;
            b.m_Speed = b.m_DirChangeSpeed;
            b.m_DirChangeAt = -1;
            b.m_UseDecay    = false;
        }

        float effectiveSpeed = b.m_Speed;
        if (b.m_UseDecay) {
            if (b.m_DecayTimer <= 16) {
                effectiveSpeed += 5.0f - b.m_DecayTimer * 5.0f / 16.0f;
            } else {
                b.m_UseDecay = false;
            }
        } else if (b.m_Acceleration != 0.0f) {
            b.m_Speed += b.m_Acceleration;
            effectiveSpeed = b.m_Speed;
        }
        b.m_DecayTimer++;

        b.m_Pos.x += std::cos(b.m_Angle) * effectiveSpeed;
        b.m_Pos.y += std::sin(b.m_Angle) * effectiveSpeed;

        b.m_Vm.pos = b.m_Pos;
        if (b.m_RotateWithAngle) b.m_Vm.rotation = b.m_Angle;
        m_Anm.UpdateObjects(b.m_Vm);

        if (!Util::IsInGameBounds(b.m_Pos.x, b.m_Pos.y, 0, 0)) {
            b.m_Alive = false;
            if (b.m_Vm.obj) {
                m_Renderer.RemoveChild(b.m_Vm.obj);
                b.m_Vm.obj = nullptr;
            }
        }
    }

    m_Renderer.Update();
}
