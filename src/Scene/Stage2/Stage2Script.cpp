#include "Scene/Stage2/Stage2Script.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace {
constexpr float PI     = 3.14159265f;
constexpr glm::vec2 CIRNO_SHOOT_OFFSET = {0.0f, -12.0f};

float RandFloat(float min, float max) {
    return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

float AimAngle(glm::vec2 from, glm::vec2 to) {
    const glm::vec2 d = to - from;
    return std::atan2(d.y, d.x);
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {}) {
    return enemy.m_Pos + offset;
}

void SetDeathEffects(Enemy& enemy, int primary, int secondary) {
    enemy.m_DeathEffectPrimary   = primary;
    enemy.m_DeathEffectSecondary = secondary;
}

void SetBossPoses(Enemy& enemy, int defaults, int farLeft, int farRight, int left, int right) {
    enemy.m_AnmDefault   = defaults;
    enemy.m_AnmFarLeft   = farLeft;
    enemy.m_AnmFarRight  = farRight;
    enemy.m_AnmLeft      = left;
    enemy.m_AnmRight     = right;
    enemy.m_AnmMoveState = 0xff;
}

void DropPowerItems(Enemy& enemy, EnemySubCtx& ctx, int count) {
    for (int i = 0; i < count; i++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
}

void StartRandomAttackMove(Enemy& enemy, const EnemySubCtx& ctx, float speed, int frames) {
    ctx.MoveRandInBounds(enemy);
    ctx.StartLerpDir(enemy, speed, frames);
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int timerFrames,
                     int deathCallbackSub, int lifeCallbackSub = -1,
                     int lifeCallbackThreshold = -1) {
    ctx.BulletCancelIntoPointItems();
    enemy.m_CanTakeDamage          = false;
    enemy.m_InSpellcard            = true;
    enemy.m_ShowSpellName          = true;
    enemy.m_BossTitle              = title;
    enemy.m_SpellcardBonus         = 0;
    enemy.m_BossTimer              = 0;
    enemy.m_TimerCallbackThreshold = timerFrames;
    enemy.m_TimerCallbackSub       = deathCallbackSub;
    enemy.m_LifeCallbackThreshold  = lifeCallbackThreshold;
    enemy.m_LifeCallbackSub        = lifeCallbackSub;
    enemy.m_DeathCallbackSub       = deathCallbackSub;
    ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
}

void SpawnRandomBullets(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                        int count, float speed, float speedVariance = 0.0f,
                        bool rotateWithAngle = false) {
    for (int i = 0; i < count; i++) {
        const float bulletSpeed = speed + RandFloat(-speedVariance, speedVariance);
        ctx.bullets.SpawnCircle(pos, type, color, 1, std::max(0.1f, bulletSpeed),
                                RandFloat(-PI, PI), false, 0.0f, 0, rotateWithAngle);
    }
}

void SpawnOneWayStackWithCurve(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type,
                               EBulletColor color, int stacks, float speed1, float speed2,
                               float baseAngle, BulletCurve curve, bool rotateWithAngle = false) {
    const glm::vec2 angleTarget = pos + glm::vec2{std::cos(baseAngle), std::sin(baseAngle)};
    for (int s = 0; s < stacks; s++) {
        const float ratio = stacks <= 1 ? 0.0f : static_cast<float>(s) / static_cast<float>(stacks);
        const float speed = speed1 - (speed1 - speed2) * ratio;
        ctx.bullets.SpawnCircleAimed(pos, angleTarget, type, color, 1, speed, 0.0f, false,
                                     0.0f, curve, rotateWithAngle);
    }
}

EBulletColor PerfectFreezeColor(int idx) {
    switch (idx % 5) {
        case 0: return EBulletColor::Blue;
        case 1: return EBulletColor::Red;
        case 2: return EBulletColor::Green;
        case 3: return EBulletColor::Yellow;
        default: return EBulletColor::Orange;
    }
}

void SpawnAtRandomArea(Enemy& enemy, EnemySubCtx& ctx, float width, int count) {
    const glm::vec2 pos = enemy.m_Pos + glm::vec2{
        RandFloat(-width * 0.5f, width * 0.5f),
        RandFloat(-width * 0.375f, width * 0.375f),
    };
    SpawnRandomBullets(pos, ctx, EBulletType::Shard, EBulletColor::Blue, count, 1.2f, 0.8f,
                       true);
}

void RunDaiyouseiMove(Enemy& enemy, EnemySubCtx& ctx, int local) {
    const int offset = Anm::STG2ENM.offset;
    if (local == 0) {
        const float currentX = enemy.m_Pos.x - Util::FIELD_OFFSET_X;
        const float targetX = RandFloat(32.0f, 352.0f);
        enemy.m_CanTakeDamage = false;
        enemy.m_LerpTarget.x  = targetX;
        ctx.anm.SetScript(enemy.m_Vm, offset + (currentX >= 192.0f ? 66 : 67), offset);
    } else if (local == 40) {
        const float targetX = enemy.m_LerpTarget.x;
        ctx.anm.SetScript(enemy.m_Vm, offset + (targetX >= 192.0f ? 68 : 69), offset);
        enemy.m_Pos           = Util::GameFieldToScreen(targetX, 96.0f);
        enemy.m_IsLerping     = false;
        enemy.m_CanTakeDamage = true;
    }
}

int DaiyouseiAttackDuration(int attackType) {
    return attackType == 2 ? 200 : 88;
}

void RunDaiyouseiAttack(Enemy& enemy, EnemySubCtx& ctx, int attackType, int local) {
    if (attackType == 0 || attackType == 1) {
        if (local < 40 || local >= 88) return;

        const int   step      = local - 40;
        const float speed1    = 1.5f + static_cast<float>(step) * 0.05f;
        const float speed2    = 1.4f - static_cast<float>(step) * 0.02f;
        const float direction = Util::HALF_PI +
                                static_cast<float>(step) *
                                    (attackType == 0 ? 0.1308997f : -0.1308997f);
        ctx.bullets.SpawnCircleStack(
            enemy.m_Pos, EBulletType::Kunai,
            attackType == 0 ? EBulletColor::Green : EBulletColor::Red, 1, 2, speed1, speed2,
            direction, false, true);
        return;
    }

    if (local < 40 || local >= 200 || (local - 40) % 10 != 0) return;

    ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Shard,
                              EBulletColor::White, 3, 2, 4.0f, 2.0f, 0.0f, 0.5235988f, true);
    ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Shard,
                              EBulletColor::Blue, 3, 2.5f, 0.0f, 0.5235988f, false, true);
}

void RunDaiyouseiPattern(Enemy& enemy, EnemySubCtx& ctx, int frame) {
    int cursor = 0;
    for (int cycle = 0; cycle < 8; cycle++) {
        const int attackType     = cycle % 3;
        const int attackDuration = DaiyouseiAttackDuration(attackType);

        if (frame >= cursor && frame < cursor + attackDuration) {
            RunDaiyouseiAttack(enemy, ctx, attackType, frame - cursor);
            return;
        }
        cursor += attackDuration;

        if (frame >= cursor && frame < cursor + 80) {
            RunDaiyouseiMove(enemy, ctx, frame - cursor);
            return;
        }
        cursor += 80;

        if (frame >= cursor && frame < cursor + 80) return;
        cursor += 80;
    }
}

}  // namespace

void Stage2Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG2ENM.folder, Anm::STG2ENM.txt, Anm::STG2ENM.offset);
    anm.LoadAnm(Anm::STG2ENM2.folder, Anm::STG2ENM2.txt, Anm::STG2ENM2.offset);
}

void Stage2Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG2ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: {
            static constexpr float angles[5] = {
                0.7853982f, 1.1780972f, Util::HALF_PI, 1.9634954f, 2.3561945f};
            const bool leftSprite = (enemy.m_SubId % 2) == 0;
            ctx.anm.SetScript(enemy.m_Vm, offset + (leftSprite ? 13 : 14), offset);
            enemy.m_HitboxSize  = {28.0f, 28.0f};
            enemy.m_Angle       = angles[enemy.m_SubId];
            enemy.m_Speed       = 3.0f;
            enemy.m_Acceleration = -0.015f;
            enemy.m_ItemDrop    = -1;
            SetDeathEffects(enemy, 669, 678);
            if (enemy.m_SubId == 0 || enemy.m_SubId == 2 || enemy.m_SubId == 4) {
                enemy.m_DeathCallbackSub = 5;
            }
            break;
        }

        case 6:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = AimAngle(enemy.m_Pos, ctx.playerPos);
            enemy.m_Speed      = 2.4f;
            enemy.m_ItemDrop   = -1;
            SetDeathEffects(enemy, 670, 678);
            break;

        case 7:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {16.0f, 16.0f};
            enemy.m_Angle      = RandFloat(0.7853982f, 2.3561945f);
            enemy.m_Speed      = 3.0f;
            enemy.m_ItemDrop   = -1;
            SetDeathEffects(enemy, 670, 678);
            break;

        case 8:
        case 9:
        case 10:
        case 11:
            ctx.anm.SetScript(enemy.m_Vm, offset + enemy.m_SubId, offset);
            enemy.m_HitboxSize = {22.0f, 22.0f};
            enemy.m_Angle      = RandFloat(0.7853982f, 2.3561945f);
            enemy.m_Speed      = 5.0f;
            enemy.m_ItemDrop   = -1;
            enemy.m_RotateWithAngle = true;
            SetDeathEffects(enemy, 670, 678);
            break;

        case 12:
        case 13:
            ctx.anm.SetScript(enemy.m_Vm, offset + 3, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            enemy.m_ItemDrop   = -1;
            SetDeathEffects(enemy, 669, 678);
            break;

        case 20:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(192.0f, -32.0f);
            enemy.m_HitboxSize             = {45.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_BossTitle              = "Daiyousei";
            enemy.m_BossLifeCount          = 0;
            enemy.m_TimerCallbackThreshold = 1920;
            enemy.m_TimerCallbackSub       = 19;
            enemy.m_DeathCallbackSub       = 18;
            SetDeathEffects(enemy, 671, 676);
            break;

        case 21: {
            const int bossOff = Anm::STG2ENM2.offset;
            ctx.anm.SetScript(enemy.m_Vm, bossOff + 132, bossOff);
            enemy.m_Pos           = Util::GameFieldToScreen(192.0f, 96.0f);
            enemy.m_HitboxSize    = {48.0f, 56.0f};
            enemy.m_IsBoss        = true;
            enemy.m_CanTakeDamage = false;
            enemy.m_ItemDropCount = 0;
            enemy.m_BossTitle     = "Cirno";
            enemy.m_BossLifeCount = 1;
            enemy.m_BlocksTimeline = true;
            enemy.m_BoundsMin     = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax     = Util::GameFieldToScreen(352.0f, 134.0f);
            enemy.m_DeathCallbackSub = 25;
            SetDeathEffects(enemy, 671, 676);
            SetBossPoses(enemy, 128, 129, 130, 129, 130);
            break;
        }

        default:
            break;
    }
}

void Stage2Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t      = enemy.m_FrameTimer;
    const int offset = Anm::STG2ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            if (t == 180 && (enemy.m_SubId == 0 || enemy.m_SubId == 2 || enemy.m_SubId == 4)) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Kunai,
                                             EBulletColor::Lime, 4, 2.0f, 0.0f, false, 0.0f, {},
                                             true);
            }
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 5:
            if (t == 0) {
                ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                             EBulletColor::Red, 4, 1.2f, PI * 0.25f, false, 0.0f,
                                             {},
                                             true);
                enemy.m_Alive = false;
            }
            break;

        case 6:
            if (t == 180) enemy.m_AngularVelocity = -0.024543693f;
            if (t == 280) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 12:
        case 13:
            if (t == 60) {
                ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
                enemy.m_Speed = 0.0f;
            }
            if (t == 70 && enemy.m_SubId == 12) {
                ctx.bullets.SpawnFanStack(enemy.m_Pos + glm::vec2{12.0f, -12.0f}, ctx.playerPos,
                                          EBulletType::RingBall, EBulletColor::DarkRed, 7, 2,
                                          1.4f, 0.8f, 0.0f, 0.62831855f);
            }
            if (t == 130) {
                enemy.m_Acceleration    = 0.05f;
                enemy.m_AngularVelocity = 0.05235988f;
            }
            if (t == 190) enemy.m_AngularVelocity = 0.0f;
            if (t >= 10000) enemy.m_Alive = false;
            break;

        case 20: {
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 60);
            if (t == 60) enemy.m_CanTakeDamage = true;

            if (t >= 130) RunDaiyouseiPattern(enemy, ctx, t - 130);
            break;
        }

        case 18:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                enemy.m_Alive = false;
            }
            break;

        case 19:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 60) enemy.m_Alive = false;
            break;

        case 21: {
            const int bossOff = Anm::STG2ENM2.offset;
            if (t == 1) ctx.anm.SetScript(enemy.m_Vm, bossOff + 132, bossOff);
            if (t == 32) {
                ctx.anm.SetScript(enemy.m_Vm, bossOff + 128, bossOff);
                ctx.TransitionToSub(enemy, 22);
            }
            break;
        }

        case 22:
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_InSpellcard            = false;
                enemy.m_ShowSpellName          = false;
                enemy.m_BossTitle              = "Cirno";
                enemy.m_BossLifeCount          = 1;
                enemy.m_Life                   = 10000;
                enemy.m_BossMaxLife            = 10000;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 1500;
                enemy.m_TimerCallbackSub       = 30;
                enemy.m_LifeCallbackThreshold  = 1500;
                enemy.m_LifeCallbackSub        = 30;
                enemy.m_DeathCallbackSub       = 25;
            }
            if (t == 20) enemy.m_CanTakeDamage = true;
            if (t == 70) ctx.TransitionToSub(enemy, 23);
            break;

        case 23: {
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 40);
            if (t == 41) ctx.anm.SetScript(enemy.m_Vm, Anm::STG2ENM2.offset + 131, Anm::STG2ENM2.offset);
            if (t >= 41 && t < 41 + 3 * 70) {
                const int rel   = t - 41;
                const int cycle = rel / 70;
                const int local = rel % 70;
                if (local <= 10 && local % 2 == 0) {
                    const int   fanIdx = local / 2;
                    const float spread = 0.05609987f + static_cast<float>(cycle) * 0.049087387f;
                    ctx.bullets.SpawnFanAimed(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos,
                                              EBulletType::Shard, EBulletColor::Blue, fanIdx + 1,
                                              5.0f - static_cast<float>(fanIdx) * 0.5f, 0.0f,
                                              spread, false, true);
                }
            }
            if (t == 251) ctx.TransitionToSub(enemy, 24);
            break;
        }

        case 24: {
            const int loopT = t % 100;
            if (loopT == 20 && t < 300) {
                StartRandomAttackMove(enemy, ctx, 3.0f, 60);
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos,
                                             EBulletType::RingBall, EBulletColor::Blue, 16, 2.0f);
            }
            if (loopT == 40 && t < 300) {
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos,
                                             EBulletType::Shard, EBulletColor::White, 24, 3.0f,
                                             0.0f, false, 0.0f, {}, true);
            }
            if (loopT == 60 && t < 300) {
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos,
                                             EBulletType::RingBall, EBulletColor::Blue, 14, 3.0f);
            }
            if (t == 300) ctx.TransitionToSub(enemy, 23);
            break;
        }

        case 30: {
            if (t == 0) {
                enemy.m_BossLifeCount = 1;
                StartSpellPhase(enemy, ctx, "Icicle Fall", 1800, 25);
            }
            if (t == 120) enemy.m_CanTakeDamage = true;
            if (t >= 120) {
                const int loopT = (t - 120) % 297;
                const int cycle = (t - 120) / 297;
                if (loopT < 297 && loopT % 27 == 0) {
                    const int burst = loopT / 27;
                    const int stacks = cycle < 3 ? 3 : (cycle < 6 ? 4 : 5);
                    const float jitter = RandFloat(0.0f, 0.049087387f);
                    const float rightBase = -0.19634955f + 0.09817477f * static_cast<float>(burst);
                    const float leftBase = -2.9452431f - 0.09817477f * static_cast<float>(burst);
                    const BulletCurve rightCurve{60, Util::HALF_PI, 1.6f, true, false, 1};
                    const BulletCurve leftCurve{60, -Util::HALF_PI, 1.4f, true, false, 1};
                    const glm::vec2 shootPos = ShootPos(enemy, CIRNO_SHOOT_OFFSET);
                    SpawnOneWayStackWithCurve(shootPos, ctx, EBulletType::Shard,
                                              EBulletColor::Blue, stacks, 6.5f, 0.5f,
                                              rightBase + jitter, rightCurve, true);
                    SpawnOneWayStackWithCurve(shootPos, ctx, EBulletType::Shard,
                                              EBulletColor::Blue, stacks, 6.5f, 0.5f,
                                              leftBase - jitter, leftCurve, true);

                    const float playerFieldY = ctx.playerPos.y - Util::FIELD_OFFSET_Y;
                    const bool fireAimed = playerFieldY < 192.0f ||
                                           (playerFieldY < 256.0f && burst % 2 == 0) ||
                                           (playerFieldY >= 256.0f && burst % 3 == 0);
                    if (fireAimed) {
                        ctx.bullets.SpawnFanAimed(shootPos, ctx.playerPos, EBulletType::Ball,
                                                  EBulletColor::Yellow, 5, 2.0f, 0.0f, 0.2617994f);
                    }
                }
            }
            break;
        }

        case 25:
            if (t == 0) {
                enemy.m_CanTakeDamage          = false;
                enemy.m_InSpellcard            = false;
                enemy.m_ShowSpellName          = false;
                enemy.m_BossTitle              = "Cirno";
                enemy.m_BossLifeCount          = 0;
                enemy.m_Life                   = 13000;
                enemy.m_BossMaxLife            = 13000;
                enemy.m_BossTimer              = 0;
                enemy.m_TimerCallbackThreshold = 3000;
                enemy.m_TimerCallbackSub       = 31;
                enemy.m_LifeCallbackThreshold  = 3200;
                enemy.m_LifeCallbackSub        = 31;
                enemy.m_DeathCallbackSub       = 28;
                DropPowerItems(enemy, ctx, 5);
            }
            if (t == 200) {
                enemy.m_CanTakeDamage = true;
                ctx.TransitionToSub(enemy, 26);
            }
            break;

        case 26: {
            if (t == 0) StartRandomAttackMove(enemy, ctx, 3.0f, 60);
            if (t >= 0 && t < 160 && t % 20 == 0) {
                const int count = std::clamp(8 + enemy.m_BossTimer / 600, 8, 14);
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos,
                                             EBulletType::RingBall, EBulletColor::Blue, count,
                                             3.0f);
            }
            if (t >= 10 && t < 170 && t % 20 == 10) {
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos,
                                             EBulletType::Pellet, EBulletColor::DarkBlue, 8, 1.3f,
                                             0.0f, false, 0.0f, {}, true);
            }
            if (t == 160) ctx.TransitionToSub(enemy, 27);
            break;
        }

        case 27: {
            if (t == 0) StartRandomAttackMove(enemy, ctx, 3.0f, 60);
            if (t == 0 || t == 50 || t == 100) {
                const glm::vec2 pos = ShootPos(enemy, CIRNO_SHOOT_OFFSET);
                ctx.lasers.SpawnAimed(pos, ctx.playerPos, 192.0f, 6.0f, 30, 60, 30, 0, 30,
                                      4.0f);
                ctx.lasers.SpawnAtAngle(pos, AimAngle(pos, ctx.playerPos) + 0.3926991f, 192.0f,
                                        6.0f, 30, 60, 30, 0, 30, 0.0f, 4.0f);
                ctx.lasers.SpawnAtAngle(pos, AimAngle(pos, ctx.playerPos) - 0.3926991f, 192.0f,
                                        6.0f, 30, 60, 30, 0, 30, 0.0f, 4.0f);
                ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, EBulletType::RingBall,
                                             EBulletColor::Blue, 16, 2.0f);
            }
            if (t == 150) ctx.TransitionToSub(enemy, 26);
            break;
        }

        case 31: {
            if (t == 0) {
                enemy.m_BossLifeCount = 0;
                StartSpellPhase(enemy, ctx, "Perfect Freeze", 2400, 28, 32, 1400);
                ctx.anm.SetScript(enemy.m_Vm, Anm::STG2ENM2.offset + 131, Anm::STG2ENM2.offset);
            }
            if (t == 120) enemy.m_CanTakeDamage = true;
            if (t >= 120) {
                const int loopT = (t - 120) % 595;
                if (loopT == 0 || loopT == 265) StartRandomAttackMove(enemy, ctx, 2.0f, 120);
                if (loopT >= 5 && loopT < 155 && (loopT - 5) % 5 == 0) {
                    const int cycle = (t - 120) / 595;
                    const int count = std::clamp(7 + cycle, 7, 18);
                    const int colorIdx = ((loopT - 5) / 5) % 5;
                    SpawnRandomBullets(ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx,
                                       EBulletType::RingBall, PerfectFreezeColor(colorIdx), count,
                                       4.0f);
                }
                if (loopT == 175) {
                    ctx.bullets.FreezeAllBulletsAsWhite();
                }
                if (loopT >= 275 && loopT < 335 && loopT % 10 == 5) {
                    const int cycle = (t - 120) / 595;
                    if (cycle < 3) {
                        ctx.bullets.SpawnFanStack(ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                                  ctx.playerPos, EBulletType::Ball,
                                                  EBulletColor::Blue, 3, 3, 4.0f, 2.0f, 0.0f,
                                                  0.3926991f);
                    } else if (cycle < 6) {
                        ctx.bullets.SpawnFanStack(ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                                  ctx.playerPos, EBulletType::Ball,
                                                  EBulletColor::Blue, 5, 3, 4.0f, 2.0f, 0.0f,
                                                  0.3926991f);
                    } else {
                        ctx.bullets.SpawnFanStack(ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                                  ctx.playerPos, EBulletType::Ball,
                                                  EBulletColor::Blue, 5, 3, 5.0f, 2.0f, 0.0f,
                                                  0.19634955f);
                    }
                }
                if (loopT == 385) {
                    ctx.bullets.AccelerateFrozenBulletsRandom(0.01f, 220);
                }
            }
            break;
        }

        case 32: {
            if (t == 0) {
                enemy.m_BossLifeCount = 0;
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                enemy.m_BossTitle     = "Cirno";
                enemy.m_BossTimer     = 0;
                DropPowerItems(enemy, ctx, 5);
                ctx.BulletCancelIntoPointItems();
            }
            if (t == 60) {
                StartSpellPhase(enemy, ctx, "Diamond Blizzard", 1980, 28);
                ctx.anm.SetScript(enemy.m_Vm, Anm::STG2ENM2.offset + 131, Anm::STG2ENM2.offset);
            }
            if (t == 180) enemy.m_CanTakeDamage = true;
            if (t >= 180) {
                const int elapsed = t - 180;
                const int loopT = elapsed % 120;
                if (loopT == 0) StartRandomAttackMove(enemy, ctx, 1.2f, 120);
                if (elapsed > 0 && elapsed % 10 == 0) {
                    const int cycle = elapsed / 120;
                    SpawnAtRandomArea(enemy, ctx, 128.0f, std::clamp(10 + cycle, 10, 18));
                }
            }
            break;
        }

        case 28:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Alive = false;
            }
            break;

        default:
            break;
    }
}
