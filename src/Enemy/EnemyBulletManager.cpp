#include "Enemy/EnemyBulletManager.hpp"

#include <cmath>
#include <cstdlib>

#include "Audio/AudioManager.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace {

bool UsesEtama4(EBulletType type) {
    return type == EBulletType::Bubble;
}

int BubbleSpriteOffset(EBulletColor color) {
    switch (color) {
        case EBulletColor::DarkRed:
        case EBulletColor::Red:
            return 1;
        case EBulletColor::DarkBlue:
        case EBulletColor::Blue:
        case EBulletColor::DarkCyan:
        case EBulletColor::Cyan:
            return 2;
        case EBulletColor::DarkPurple:
        case EBulletColor::Purple:
            return 3;
        default:
            return 0;
    }
}

int DaggerSpriteOffset(EBulletColor color) {
    switch (color) {
        case EBulletColor::DarkRed:
        case EBulletColor::Red:
            return 1;
        case EBulletColor::DarkPurple:
        case EBulletColor::Purple:
            return 3;
        case EBulletColor::DarkBlue:
        case EBulletColor::Blue:
            return 4;
        case EBulletColor::DarkGreen:
        case EBulletColor::Green:
        case EBulletColor::Lime:
            return 5;
        case EBulletColor::DarkYellow:
        case EBulletColor::Yellow:
            return 6;
        case EBulletColor::Orange:
            return 7;
        default:
            return 0;
    }
}

int BulletScriptIdx(EBulletType type) {
    if (UsesEtama4(type)) return Anm::ETAMA4.offset;
    return Anm::ETAMA3.offset + static_cast<int>(type);
}

int BulletSpriteOffset(EBulletType type, EBulletColor color) {
    if (UsesEtama4(type)) return Anm::ETAMA4.offset + BubbleSpriteOffset(color);
    if (type == EBulletType::Dagger) return Anm::ETAMA3.offset + DaggerSpriteOffset(color);
    return Anm::ETAMA3.offset + static_cast<int>(color);
}

glm::vec2 BulletHitboxSize(EBulletType type) {
    switch (type) {
    case EBulletType::Pellet:
    case EBulletType::Rice:
    case EBulletType::Shard:
        return {4.0f, 4.0f};
    case EBulletType::Kunai:
        return {5.0f, 5.0f};
    case EBulletType::RingBall:
    case EBulletType::Ball:
        return {6.0f, 6.0f};
    case EBulletType::BigBall:
        return {16.0f, 16.0f};
    case EBulletType::Fireball:
        return {11.0f, 11.0f};
    case EBulletType::Dagger:
        return {9.0f, 9.0f};
    case EBulletType::Bubble:
        return {32.0f, 32.0f};
    }
    return {5.0f, 5.0f};
}

bool AutoRotatesWithAngle(EBulletType type) {
    return type == EBulletType::Rice || type == EBulletType::Kunai ||
           type == EBulletType::Shard || type == EBulletType::Fireball ||
           type == EBulletType::Dagger;
}

void ResetBulletForSpawn(EnemyBullet& bullet) {
    auto obj = bullet.m_Vm.obj;
    bullet = EnemyBullet{};
    bullet.m_Vm.obj = obj;
}

}  // namespace

EnemyBulletManager::EnemyBulletManager() {
    m_Anm.LoadAnm(Anm::ETAMA3.folder, Anm::ETAMA3.txt, Anm::ETAMA3.offset);
    m_Anm.LoadAnm(Anm::ETAMA4.folder, Anm::ETAMA4.txt, Anm::ETAMA4.offset);
    for (auto& bullet : m_Bullets) {
        bullet.m_Vm.obj = std::make_shared<Util::GameObject>(nullptr, 1.0f, glm::vec2{0, 0}, false);
    }
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
    }
    m_NextIdx = (m_NextIdx + 1) % MAX_BULLETS;
    return b;
}

void EnemyBulletManager::SpawnFanAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                       EBulletColor color, int count, float speed, float aimOffset,
                                       float spread, bool useDecay, bool rotateWithAngle) {
    AudioManager::Instance().Play(SoundEffect::EnemyShoot, 1);
    int scriptIdx = BulletScriptIdx(type);
    int sprOffset = BulletSpriteOffset(type, color);

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
        ResetBulletForSpawn(*b);
        b->m_Alive           = true;
        b->m_Pos             = pos;
        b->m_Type            = type;
        b->m_Color           = color;
        b->m_Angle           = aimAngle + delta;
        b->m_Speed           = speed;
        b->m_HitboxSize      = BulletHitboxSize(type);
        b->m_UseDecay        = useDecay;
        b->m_RotateWithAngle = rotateWithAngle || AutoRotatesWithAngle(type);

        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

void EnemyBulletManager::SpawnFanStack(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                       EBulletColor color, int ways, int stacks, float speed1,
                                       float speed2, float aimOffset, float spread,
                                       bool rotateWithAngle) {
    for (int s = 0; s < stacks; s++) {
        float speed = speed1 - (speed1 - speed2) * s / stacks;
        SpawnFanAimed(pos, playerPos, type, color, ways, speed, aimOffset, spread, false,
                      rotateWithAngle);
    }
}

void EnemyBulletManager::SpawnCircleAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                          EBulletColor color, int count, float speed,
                                          float aimOffset, bool useDecay, float acceleration,
                                          BulletCurve curve, bool rotateWithAngle,
                                          float angularVelocity, int angularVelocityFrames) {
    SpawnCircleAimed(pos, playerPos, type, color, count, speed, aimOffset, useDecay, acceleration,
                     0, curve, rotateWithAngle, angularVelocity, angularVelocityFrames);
}

void EnemyBulletManager::SpawnCircleAimed(glm::vec2 pos, glm::vec2 playerPos, EBulletType type,
                                          EBulletColor color, int count, float speed,
                                          float aimOffset, bool useDecay, float acceleration,
                                          int accelerationFrames, BulletCurve curve,
                                          bool rotateWithAngle, float angularVelocity,
                                          int angularVelocityFrames) {
    AudioManager::Instance().Play(SoundEffect::EnemyShoot, 1);
    int   scriptIdx = BulletScriptIdx(type);
    int   sprOffset = BulletSpriteOffset(type, color);
    float aimAngle  = std::atan2(playerPos.y - pos.y, playerPos.x - pos.x) + aimOffset;
    float step      = 2.0f * Util::HALF_PI * 2.0f / count;

    for (int i = 0; i < count; i++) {
        EnemyBullet* b         = AllocBullet();
        ResetBulletForSpawn(*b);
        b->m_Alive             = true;
        b->m_Pos               = pos;
        b->m_Type              = type;
        b->m_Color             = color;
        b->m_Angle             = aimAngle + i * step;
        b->m_Speed             = speed;
        b->m_AngularVelocity   = angularVelocity;
        b->m_AngularVelocityFrames = angularVelocityFrames;
        b->m_HitboxSize        = BulletHitboxSize(type);
        b->m_UseDecay          = useDecay;
        b->m_Acceleration      = acceleration;
        b->m_AccelerationFrames = accelerationFrames;
        b->m_DirChangeInterval = curve.at;
        b->m_DirChangeNumTimes = 0;
        b->m_DirChangeMaxTimes = curve.times;
        b->m_DirChangeAngle    = curve.angle;
        b->m_DirChangeSpeed    = curve.speed;
        b->m_DirChangeRelative = curve.relative;
        b->m_DirChangeAimAtPlayer = curve.aimAtPlayer;
        b->m_DirChangeStartupFrames = curve.startupFrames;
        b->m_DirChangeStartupSpeedScale = curve.startupSpeedScale;
        b->m_RotateWithAngle   = rotateWithAngle || AutoRotatesWithAngle(type);
        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

void EnemyBulletManager::SpawnCircle(glm::vec2 pos, EBulletType type, EBulletColor color, int count,
                                     float speed, float baseAngle, bool useDecay,
                                     float acceleration, int accelerationFrames,
                                     bool rotateWithAngle, glm::vec2 vectorAcceleration,
                                     int vectorAccelerationFrames, int spawnMoveFrames,
                                     float spawnMoveScale, BulletCurve curve,
                                     bool bounceTopAndSides, int bounceMax, float bounceSpeed,
                                     float angularVelocity, int angularVelocityFrames) {
    AudioManager::Instance().Play(SoundEffect::EnemyShoot, 1);
    int   scriptIdx = BulletScriptIdx(type);
    int   sprOffset = BulletSpriteOffset(type, color);
    float step      = 2.0f * Util::HALF_PI * 2.0f / count;

    for (int i = 0; i < count; i++) {
        EnemyBullet* b = AllocBullet();
        ResetBulletForSpawn(*b);
        b->m_Alive     = true;
        b->m_Pos       = pos;
        b->m_Type      = type;
        b->m_Color     = color;
        b->m_Angle     = baseAngle + i * step;
        b->m_Speed              = speed;
        b->m_HitboxSize         = BulletHitboxSize(type);
        b->m_UseDecay           = useDecay;
        b->m_Acceleration       = acceleration;
        b->m_AccelerationFrames = accelerationFrames;
        b->m_VectorAcceleration = vectorAcceleration;
        b->m_VectorAccelerationFrames = vectorAccelerationFrames;
        b->m_SpawnMoveFrames    = spawnMoveFrames;
        b->m_SpawnMoveScale     = spawnMoveScale;
        b->m_AngularVelocity    = angularVelocity;
        b->m_AngularVelocityFrames = angularVelocityFrames;
        b->m_DirChangeInterval = curve.at;
        b->m_DirChangeNumTimes = 0;
        b->m_DirChangeMaxTimes = curve.times;
        b->m_DirChangeAngle    = curve.angle;
        b->m_DirChangeSpeed    = curve.speed;
        b->m_DirChangeRelative = curve.relative;
        b->m_DirChangeAimAtPlayer = curve.aimAtPlayer;
        b->m_DirChangeStartupFrames = curve.startupFrames;
        b->m_DirChangeStartupSpeedScale = curve.startupSpeedScale;
        b->m_BounceTopAndSides = bounceTopAndSides;
        b->m_BounceMax         = bounceMax;
        b->m_BounceSpeed       = bounceSpeed >= 0.0f ? bounceSpeed : speed;
        b->m_RotateWithAngle    = rotateWithAngle || AutoRotatesWithAngle(type);
        m_Anm.SetScript(b->m_Vm, scriptIdx, sprOffset);
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
    }
}

void EnemyBulletManager::SpawnCircleStack(glm::vec2 pos, EBulletType type, EBulletColor color,
                                          int count, int stacks, float speed1, float speed2,
                                          float baseAngle, bool useDecay, bool rotateWithAngle) {
    if (stacks <= 1) {
        SpawnCircle(pos, type, color, count, speed1, baseAngle, useDecay, 0.0f, 0,
                    rotateWithAngle);
        return;
    }

    for (int s = 0; s < stacks; s++) {
        float speed = speed1 - (speed1 - speed2) * static_cast<float>(s) /
                                   static_cast<float>(stacks);
        SpawnCircle(pos, type, color, count, speed, baseAngle, useDecay, 0.0f, 0,
                    rotateWithAngle);
    }
}

bool EnemyBulletManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    if (m_TimeStopped) return false;

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

void EnemyBulletManager::FreezeAllBulletsAsWhite() {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;

        b.m_Color                    = EBulletColor::White;
        b.m_Speed                    = 0.0f;
        b.m_AngularVelocity          = 0.0f;
        b.m_AngularVelocityFrames    = 0;
        b.m_Acceleration             = 0.0f;
        b.m_AccelerationFrames       = 0;
        b.m_UseDecay                 = false;
        b.m_DirChangeInterval        = -1;
        b.m_DirChangeNumTimes        = 0;
        b.m_DirChangeMaxTimes        = 0;
        b.m_DirChangeStartupFrames   = 0;
        b.m_DirChangeStartupSpeedScale = 1.0f;
        b.m_DecayTimer               = 0;
        b.m_FrozenByPerfectFreeze    = true;

        m_Anm.SetScript(b.m_Vm, BulletScriptIdx(b.m_Type),
                        BulletSpriteOffset(b.m_Type, b.m_Color));
    }
}

void EnemyBulletManager::AccelerateFrozenBulletsRandom(float acceleration, int frames) {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;
        if (!b.m_FrozenByPerfectFreeze) continue;

        const float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        b.m_Angle              = r * 2.0f * Util::HALF_PI * 2.0f - Util::HALF_PI * 2.0f;
        b.m_Speed              = 0.0f;
        b.m_AngularVelocity    = 0.0f;
        b.m_AngularVelocityFrames = 0;
        b.m_Acceleration       = acceleration;
        b.m_AccelerationFrames = frames;
        b.m_UseDecay           = false;
        b.m_DecayTimer         = 0;
        b.m_FrozenByPerfectFreeze = false;
    }
}

int EnemyBulletManager::Stage6CreateSeedsFromLargeBullets() {
    int created = 0;
    for (auto& source : m_Bullets) {
        if (!source.m_Alive) continue;
        if (source.m_Type != EBulletType::Bubble && source.m_Type != EBulletType::BigBall &&
            source.m_Type != EBulletType::Dagger) {
            continue;
        }

        EnemyBullet* b = AllocBullet();
        ResetBulletForSpawn(*b);
        b->m_Alive     = true;
        b->m_Pos       = source.m_Pos;
        b->m_Type      = EBulletType::Ball;
        b->m_Color     = EBulletColor::DarkRed;
        b->m_Angle     = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) *
                         2.0f * Util::HALF_PI * 2.0f - Util::HALF_PI * 2.0f;
        b->m_Speed           = 0.0f;
        b->m_HitboxSize      = BulletHitboxSize(b->m_Type);
        b->m_RotateWithAngle = true;
        m_Anm.SetScript(b->m_Vm, BulletScriptIdx(b->m_Type),
                        BulletSpriteOffset(b->m_Type, b->m_Color));
        if (b->m_Vm.obj) {
            m_Renderer.AddChild(b->m_Vm.obj);
        }
        ++created;
    }
    return created;
}

void EnemyBulletManager::Stage6ReleaseStoppedSeeds(glm::vec2 origin, bool distancePattern) {
    const float sharedAngle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) *
                                  2.0f * Util::HALF_PI * 2.0f -
                              Util::HALF_PI * 2.0f;
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;
        if (b.m_Type == EBulletType::Bubble || b.m_Type == EBulletType::BigBall) continue;
        if (std::abs(b.m_Speed) > 0.0001f) continue;

        float accelAngle = sharedAngle;
        if (distancePattern) {
            const glm::vec2 delta    = origin - b.m_Pos;
            const float     distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            accelAngle = distance * Util::HALF_PI * 2.0f / 256.0f + sharedAngle;
        } else {
            accelAngle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) *
                             2.0f * Util::HALF_PI * 2.0f -
                         Util::HALF_PI * 2.0f;
        }

        b.m_Speed                    = 0.01f;
        b.m_Color                    = EBulletColor::Red;
        m_Anm.SetScript(b.m_Vm, BulletScriptIdx(b.m_Type), BulletSpriteOffset(b.m_Type, b.m_Color));
        b.m_VectorVelocity           = {0.0f, 0.0f};
        b.m_VectorAcceleration       = {std::cos(accelAngle) * 0.01f,
                                        std::sin(accelAngle) * 0.01f};
        b.m_VectorAccelerationFrames = 120;
    }
}

void EnemyBulletManager::ClearAll() {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;
        b.m_Alive = false;
        if (b.m_Vm.obj) {
            m_Renderer.RemoveChild(b.m_Vm.obj);
        }
    }
}

void EnemyBulletManager::TurnAllBulletsIntoPointItems(ItemManager& items) {
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;

        items.SpawnItem(b.m_Pos, ItemType::PointBullet, 1);
        b.m_Alive = false;
        if (b.m_Vm.obj) {
            m_Renderer.RemoveChild(b.m_Vm.obj);
        }
    }
}

void EnemyBulletManager::TurnBulletsIntoPointItemsInRadiusRange(ItemManager& items,
                                                                glm::vec2 center,
                                                                float innerRadius,
                                                                float outerRadius) {
    const float inner2 = innerRadius * innerRadius;
    const float outer2 = outerRadius * outerRadius;

    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;

        const glm::vec2 delta = b.m_Pos - center;
        const float     dist2 = delta.x * delta.x + delta.y * delta.y;
        if (dist2 < inner2 || dist2 > outer2) continue;

        items.SpawnItem(b.m_Pos, ItemType::PointBullet, 1);
        b.m_Alive = false;
        if (b.m_Vm.obj) {
            m_Renderer.RemoveChild(b.m_Vm.obj);
        }
    }
}

void EnemyBulletManager::Update(glm::vec2 playerPos) {
    if (m_TimeStopped) {
        for (auto& b : m_Bullets) {
            if (!b.m_Alive) continue;
            b.m_Vm.pos = b.m_Pos;
            if (b.m_RotateWithAngle) {
                b.m_Vm.rotation = Util::HALF_PI - b.m_Angle;
            }
            m_Anm.UpdateObjects(b.m_Vm);
        }
        Render();
        return;
    }

    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;

        float effectiveSpeed = b.m_Speed;
        const bool inSpawnMove = b.m_SpawnMoveFrames > 0;
        if (inSpawnMove) {
            effectiveSpeed *= b.m_SpawnMoveScale;
            --b.m_SpawnMoveFrames;
        } else if (b.m_UseDecay) {
            if (b.m_DecayTimer <= 16) {
                effectiveSpeed += 5.0f - b.m_DecayTimer * 5.0f / 16.0f;
            } else {
                b.m_UseDecay = false;
            }
        } else if (b.m_Acceleration != 0.0f && b.m_AccelerationFrames > 0) {
            b.m_Speed += b.m_Acceleration;
            --b.m_AccelerationFrames;
            effectiveSpeed = b.m_Speed;
        }

        if (b.m_AngularVelocity != 0.0f && b.m_AngularVelocityFrames > 0) {
            b.m_Angle += b.m_AngularVelocity;
            --b.m_AngularVelocityFrames;
        }

        // TH06 ex flag 0x40 behavior: within each interval, speed decays linearly to 0;
        // at interval boundary, angle/speed are reset and this can repeat multiple times.
        if (b.m_DirChangeInterval > 0 && b.m_DirChangeNumTimes < b.m_DirChangeMaxTimes) {
            if (b.m_DecayTimer < b.m_DirChangeStartupFrames) {
                effectiveSpeed = b.m_Speed * b.m_DirChangeStartupSpeedScale;
            } else {
                int nextTrigger =
                    b.m_DirChangeStartupFrames +
                    b.m_DirChangeInterval * (b.m_DirChangeNumTimes + 1);
                if (b.m_DecayTimer >= nextTrigger) {
                    b.m_DirChangeNumTimes++;
                    if (b.m_DirChangeAimAtPlayer) {
                        b.m_Angle =
                            std::atan2(playerPos.y - b.m_Pos.y, playerPos.x - b.m_Pos.x) +
                            b.m_DirChangeAngle;
                    } else {
                        b.m_Angle = b.m_DirChangeRelative ? b.m_Angle + b.m_DirChangeAngle
                                                          : b.m_DirChangeAngle;
                    }
                    b.m_Speed      = b.m_DirChangeSpeed;
                    effectiveSpeed = b.m_Speed;
                    if (b.m_DirChangeNumTimes >= b.m_DirChangeMaxTimes) {
                        b.m_DirChangeInterval = -1;
                    }
                } else {
                    int intervalStart = b.m_DirChangeInterval * b.m_DirChangeNumTimes;
                    float localTime = static_cast<float>(b.m_DecayTimer - b.m_DirChangeStartupFrames -
                                                         intervalStart);
                    float interval = static_cast<float>(b.m_DirChangeInterval);
                    effectiveSpeed = b.m_Speed - (localTime * b.m_Speed) / interval;
                    if (effectiveSpeed < 0.0f) effectiveSpeed = 0.0f;
                }
            }
        }

        b.m_DecayTimer++;

        if (!inSpawnMove && b.m_VectorAccelerationFrames > 0) {
            b.m_VectorVelocity += b.m_VectorAcceleration;
            --b.m_VectorAccelerationFrames;
        }

        const float dx = std::cos(b.m_Angle) * effectiveSpeed + b.m_VectorVelocity.x;
        const float dy = std::sin(b.m_Angle) * effectiveSpeed + b.m_VectorVelocity.y;
        b.m_Pos.x += dx;
        b.m_Pos.y += dy;

        bool bounced = false;
        if (b.m_BounceTopAndSides && b.m_BounceCount < b.m_BounceMax &&
            !Util::IsInGameBounds(b.m_Pos.x, b.m_Pos.y, 0, 0)) {
            if (b.m_Pos.x < Util::GAME_BOUNDS_LEFT || b.m_Pos.x >= Util::GAME_BOUNDS_RIGHT) {
                b.m_Angle = -b.m_Angle - Util::HALF_PI * 2.0f;
                bounced   = true;
            }
            if (b.m_Pos.y < Util::GAME_BOUNDS_TOP) {
                b.m_Angle = -b.m_Angle;
                bounced   = true;
            }
            if (bounced) {
                b.m_Speed = b.m_BounceSpeed;
                ++b.m_BounceCount;
                if (b.m_BounceCount >= b.m_BounceMax) b.m_BounceTopAndSides = false;
            }
        }

        b.m_Vm.pos = b.m_Pos;
        if (b.m_RotateWithAngle) {
            b.m_Vm.rotation = bounced ? Util::HALF_PI - b.m_Angle
                                      : Util::HALF_PI - std::atan2(dy, dx);
        }
        m_Anm.UpdateObjects(b.m_Vm);

        if (!bounced && !Util::IsInGameBounds(b.m_Pos.x, b.m_Pos.y, 0, 0)) {
            b.m_Alive = false;
            if (b.m_Vm.obj) {
                m_Renderer.RemoveChild(b.m_Vm.obj);
            }
        }
    }

    m_Renderer.Update();
}

void EnemyBulletManager::Render() {
    m_Renderer.Update();
}

void EnemyBulletManager::RedirectTimeStopBullets(glm::vec2 playerPos, int maxBullets) {
    int redirected = 0;
    for (auto& b : m_Bullets) {
        if (!b.m_Alive) continue;
        if (b.m_TimeStopRedirected) continue;
        if (b.m_Type != EBulletType::Dagger && b.m_Type != EBulletType::Fireball &&
            b.m_Type != EBulletType::BigBall && b.m_Type != EBulletType::Bubble) {
            continue;
        }
        if ((std::rand() % 4) != 0) continue;

        const glm::vec2 delta = b.m_Pos - playerPos;
        const float     dist2 = delta.x * delta.x + delta.y * delta.y;
        if (dist2 > 128.0f * 128.0f) {
            const float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            b.m_Angle = Util::HALF_PI * 0.5f + r * Util::HALF_PI * 1.5f;
        } else {
            const float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            b.m_Angle = std::atan2(delta.y, delta.x) + Util::HALF_PI +
                        (r * 2.0f - 1.0f) * Util::HALF_PI * 2.0f;
        }
        b.m_TimeStopRedirected = true;
        b.m_Color              = EBulletColor::Green;
        m_Anm.SetScript(b.m_Vm, BulletScriptIdx(b.m_Type), BulletSpriteOffset(b.m_Type, b.m_Color));
        b.m_RotateWithAngle = true;
        if (b.m_Vm.obj) b.m_Vm.rotation = Util::HALF_PI - b.m_Angle;
        redirected++;
        if (redirected >= maxBullets) break;
    }
}
