#include "Scene/Stage3/Stage3Script.hpp"

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
constexpr float     PI                   = 3.14159265f;
constexpr glm::vec2 MEILING_SHOOT_OFFSET = {0.0f, -12.0f};

float RandFloat(float min, float max) {
    return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = MEILING_SHOOT_OFFSET) {
    return enemy.m_Pos + offset;
}

void SetDeathEffects(Enemy& enemy, int primary, int secondary) {
    enemy.m_DeathEffectPrimary   = primary;
    enemy.m_DeathEffectSecondary = secondary;
}

void SetBossPoses(Enemy& enemy) {
    enemy.m_AnmDefault   = Anm::STG3ENM.offset + 64;
    enemy.m_AnmFarLeft   = Anm::STG3ENM.offset + 65;
    enemy.m_AnmFarRight  = Anm::STG3ENM.offset + 65;
    enemy.m_AnmLeft      = Anm::STG3ENM.offset + 65;
    enemy.m_AnmRight     = Anm::STG3ENM.offset + 65;
    enemy.m_AnmMoveState = 0xff;
}

void DropPowerItems(Enemy& enemy, EnemySubCtx& ctx, int count) {
    for (int i = 0; i < count; i++) ctx.items.SpawnItem(enemy.m_Pos, ItemType::PowerSmall);
}

void StartRandomBossMove(Enemy& enemy, const EnemySubCtx& ctx, float speed, int frames) {
    ctx.MoveRandInBounds(enemy);
    ctx.StartLerpDir(enemy, speed, frames);
}

void StartNonSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, int life, int lifeCount,
                        int timerFrames, int nextSub, int deathSub) {
    ctx.BulletCancelIntoPointItems();
    enemy.m_CanTakeDamage          = false;
    enemy.m_InSpellcard            = false;
    enemy.m_ShowSpellName          = false;
    enemy.m_BossTitle              = "Hong Meiling";
    enemy.m_Life                   = life;
    enemy.m_BossMaxLife            = life;
    enemy.m_BossLifeCount          = lifeCount;
    enemy.m_BossTimer              = 0;
    enemy.m_TimerCallbackThreshold = timerFrames;
    enemy.m_TimerCallbackSub       = nextSub;
    enemy.m_LifeCallbackThreshold  = std::max(1200, life / 8);
    enemy.m_LifeCallbackSub        = nextSub;
    enemy.m_DeathCallbackSub       = deathSub;
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                     int lifeCount, int timerFrames, int nextSub) {
    ctx.BulletCancelIntoPointItems();
    enemy.m_CanTakeDamage          = false;
    enemy.m_InSpellcard            = true;
    enemy.m_ShowSpellName          = true;
    enemy.m_BossTitle              = title;
    enemy.m_Life                   = life;
    enemy.m_BossMaxLife            = life;
    enemy.m_BossLifeCount          = lifeCount;
    enemy.m_SpellcardBonus         = 0;
    enemy.m_BossTimer              = 0;
    enemy.m_TimerCallbackThreshold = timerFrames;
    enemy.m_TimerCallbackSub       = nextSub;
    enemy.m_LifeCallbackThreshold  = std::max(1200, life / 8);
    enemy.m_LifeCallbackSub        = nextSub;
    enemy.m_DeathCallbackSub       = nextSub;
    ctx.StartLerpTo(enemy, 192.0f, 64.0f, 120);
}

void SpawnRandomCircle(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                       int count, float speed, float variance = 0.0f,
                       bool rotateWithAngle = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1,
                                std::max(0.1f, speed + RandFloat(-variance, variance)),
                                RandFloat(-PI, PI), false, 0.0f, 0, rotateWithAngle);
    }
}

void SpawnAimedStack(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                     int ways, int stacks, float speed1, float speed2, float spread,
                     bool rotateWithAngle = false) {
    ctx.bullets.SpawnFanStack(pos, ctx.playerPos, type, color, ways, stacks, speed1, speed2, 0.0f,
                              spread, rotateWithAngle);
}

void RunOpeningFairy(Enemy& enemy, int t) {
    if (t == 30) {
        enemy.m_AngularVelocity = enemy.m_Mirrored ? 0.06544985f : -0.06544985f;
    }
    if (t == 115) enemy.m_AngularVelocity = 0.0f;
}

void RunSideFairy(Enemy& enemy, int t) {
    if (t == 30) {
        enemy.m_AngularVelocity = enemy.m_Mirrored ? -0.034906585f : 0.034906585f;
    }
    if (t == 90) enemy.m_AngularVelocity = 0.0f;
}

void RunMediumKunai(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        SpawnAimedStack(enemy.m_Pos, ctx, EBulletType::Kunai, EBulletColor::Blue, 5, 2, 2.2f, 1.2f,
                        0.5235988f, true);
    }
    if (t == 130) {
        enemy.m_Angle = RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
}

void RunMediumRing(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::DarkRed, 16, 1.7f, 0.0f, false, 0.0f, {}, true);
        SpawnAimedStack(enemy.m_Pos, ctx, EBulletType::RingBall, EBulletColor::Red, 7, 2, 2.0f,
                        0.9f, 0.62831855f);
    }
    if (t == 130) {
        enemy.m_Angle = RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.5f;
    }
}

void RunBlueScatter(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t >= 40 && t < 260 && t % 45 == 0) {
        ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                  EBulletColor::DarkRed, 5, 2, 2.2f, 1.0f, 0.0f, 0.3926991f, true);
    }
    if (t == 280) {
        enemy.m_Angle = enemy.m_Mirrored ? 2.3561945f : 0.7853982f;
        enemy.m_Speed = 2.5f;
    }
}

void RunBurstFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool whiteBurst) {
    if (t == 40) enemy.m_Acceleration = -0.05f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Speed        = 0.0f;
        if (whiteBurst) {
            SpawnRandomCircle(enemy.m_Pos, ctx, EBulletType::Ball, EBulletColor::White, 18, 2.4f,
                              0.8f);
        } else {
            ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Kunai, EBulletColor::Blue, 18, 1.8f,
                                    Util::HALF_PI, false, 0.0f, 0, true);
        }
    }
    if (t == 130) {
        enemy.m_Angle = enemy.m_Mirrored ? 2.3561945f : 0.7853982f;
        enemy.m_Speed = 1.5f;
    }
}

void RunMeilingMidbossPattern(Enemy& enemy, EnemySubCtx& ctx, int frame) {
    const int loopT = frame % 260;
    if (loopT == 0) StartRandomBossMove(enemy, ctx, 2.5f, 80);
    if (loopT >= 40 && loopT < 140 && loopT % 10 == 0) {
        const float base = (loopT / 10.0) * 0.09817477f;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                     EBulletColor::Blue, 16, 2.0f, base);
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                     EBulletColor::DarkBlue, 16, 1.2f, -base);
    }
    if (loopT >= 150 && loopT < 230 && loopT % 20 == 10) {
        SpawnAimedStack(ShootPos(enemy), ctx, EBulletType::Kunai, EBulletColor::Red, 7, 2, 3.2f,
                        1.6f, 0.44879895f, true);
    }
}

void RunColorfulNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 50) % 120;
    if (loopT == 0) {
        StartRandomBossMove(enemy, ctx, 5.0f, 40);
    }
    if (loopT >= 0 && loopT < 80 && loopT % 4 == 0) {
        const int   burst = loopT / 4;
        const int   count = std::clamp(5 + enemy.m_BossTimer / 360, 5, 10);
        const float drift = (burst % 2 == 0 ? 1.0f : -1.0f) * 0.09817477f * burst;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Shard,
                                     EBulletColor::Red, count, 2.0f + burst * 0.04f, drift, false,
                                     0.0f, {}, true);
    }
}

void RunRainbowSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120 || (t - 120) % 7 != 0) return;

    const int          step     = (t - 120) / 7;
    const float        base     = 0.87266463f + std::sin(step * 0.18f) * 1.2f;
    const glm::vec2    pos      = ShootPos(enemy, {0.0f, 0.0f});
    const EBulletColor colors[] = {EBulletColor::Red,   EBulletColor::Orange, EBulletColor::Yellow,
                                   EBulletColor::Lime,  EBulletColor::Cyan,   EBulletColor::Blue,
                                   EBulletColor::Purple};
    for (int i = 0; i < 7; i++) {
        const float angle = base + i * 0.1134464f;
        ctx.bullets.SpawnCircle(pos, EBulletType::Shard, colors[i], i == 4 ? 4 : 2, 2.6f, angle,
                                false, -0.02f, 60, true);
    }
}

void RunBlueBallNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 160) % 400;
    if (loopT == 0) StartRandomBossMove(enemy, ctx, 2.0f, 80);
    if (loopT == 0 || loopT == 70 || loopT == 140) {
        ctx.bullets.SpawnCircle(ShootPos(enemy), EBulletType::Ball, EBulletColor::DarkBlue, 16,
                                3.0f, RandFloat(-PI, PI));
    }
    if (loopT == 40 || loopT == 110 || loopT == 180) {
        SpawnRandomCircle(ShootPos(enemy), ctx, EBulletType::Ball, EBulletColor::Blue, 18, 2.0f,
                          0.3f);
    }
}

void RunFinalNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = (t - 160) % 280;
    if (loopT == 0) {
        ctx.bullets.SpawnCircleStack(ShootPos(enemy), EBulletType::Kunai, EBulletColor::Blue, 24, 4,
                                     4.6f, 2.0f, 0.0f, false, true);
    }
    if (loopT == 70) {
        ctx.bullets.SpawnCircleStack(ShootPos(enemy), EBulletType::Kunai, EBulletColor::Red, 24, 4,
                                     4.6f, 2.0f, PI / 24.0f, false, true);
    }
    if (loopT >= 90 && loopT < 170 && loopT % 2 == 0) {
        SpawnRandomCircle(ShootPos(enemy), ctx, EBulletType::Shard, EBulletColor::Blue, 2, 1.0f,
                          0.3f, true);
    }
    if (loopT == 180) StartRandomBossMove(enemy, ctx, 4.0f, 80);
}

void RunStarSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 180;
    if (loopT == 0) StartRandomBossMove(enemy, ctx, 3.0f, 80);
    if (loopT % 12 == 0) {
        const float base = static_cast<float>(t) * 0.045f;
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Dagger,
                                EBulletColor::Blue, 5, 3.2f, base, false, 0.0f, 0, true);
        ctx.bullets.SpawnCircle(ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Dagger,
                                EBulletColor::Red, 5, 2.2f, -base, false, 0.0f, 0, true);
    }
}

}  // namespace

void Stage3Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG3ENM.folder, Anm::STG3ENM.txt, Anm::STG3ENM.offset);
}

void Stage3Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG3ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
            ctx.anm.SetScript(enemy.m_Vm, offset + (enemy.m_SubId == 0 ? 13 : 14), offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? 2.6179938f : 0.5235988f;
            enemy.m_Speed      = 4.5f;
            SetDeathEffects(enemy, 669, 678);
            break;

        case 2:
        case 3:
            ctx.anm.SetScript(enemy.m_Vm, offset + 13, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? -2.0943952f : -1.0471976f;
            enemy.m_Speed      = 4.0f;
            SetDeathEffects(enemy, 669, 678);
            break;

        case 4:
        case 7:
        case 8:
            ctx.anm.SetScript(enemy.m_Vm, offset + 15, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = enemy.m_SubId == 8 ? 2.5f : 1.5f;
            SetDeathEffects(enemy, 670, 678);
            break;

        case 5:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 1.5f;
            SetDeathEffects(enemy, 670, 678);
            break;

        case 6:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            SetDeathEffects(enemy, 670, 678);
            break;

        case 9:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(352.0f, -96.0f);
            enemy.m_HitboxSize             = {56.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_BlocksTimeline         = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossTitle              = "Hong Meiling";
            enemy.m_BossLifeCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_TimerCallbackThreshold = 1800;
            enemy.m_TimerCallbackSub       = 16;
            enemy.m_LifeCallbackThreshold  = 1300;
            enemy.m_LifeCallbackSub        = 13;
            enemy.m_DeathCallbackSub       = 15;
            enemy.m_BoundsMin              = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax              = Util::GameFieldToScreen(352.0f, 144.0f);
            SetBossPoses(enemy);
            SetDeathEffects(enemy, 671, 676);
            break;

        case 17:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos              = Util::GameFieldToScreen(352.0f, -96.0f);
            enemy.m_HitboxSize       = {56.0f, 56.0f};
            enemy.m_IsBoss           = true;
            enemy.m_BlocksTimeline   = true;
            enemy.m_CanTakeDamage    = false;
            enemy.m_ItemDropCount    = 0;
            enemy.m_BossTitle        = "Hong Meiling";
            enemy.m_BossLifeCount    = 2;
            enemy.m_BoundsMin        = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax        = Util::GameFieldToScreen(352.0f, 144.0f);
            enemy.m_DeathCallbackSub = 34;
            SetBossPoses(enemy);
            SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void Stage3Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
            RunOpeningFairy(enemy, t);
            break;

        case 2:
        case 3:
            RunSideFairy(enemy, t);
            break;

        case 4:
            RunMediumKunai(enemy, ctx, t);
            break;

        case 5:
            RunMediumRing(enemy, ctx, t);
            break;

        case 6:
            RunBlueScatter(enemy, ctx, t);
            break;

        case 7:
            RunBurstFairy(enemy, ctx, t, false);
            break;

        case 8:
            RunBurstFairy(enemy, ctx, t, true);
            break;

        case 9:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
            if (t == 100) enemy.m_CanTakeDamage = true;
            if (t >= 140) RunMeilingMidbossPattern(enemy, ctx, t - 140);
            break;

        case 13:
        case 14:
        case 15:
        case 16:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                enemy.m_InSpellcard   = false;
                ctx.BulletCancelIntoPointItems();
                if (enemy.m_SubId == 15) ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 60) enemy.m_Alive = false;
            break;

        case 17:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
            if (t == 120) ctx.TransitionToSub(enemy, 18);
            break;

        case 18:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 14000, 2, 1800, 29, 20);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 50) RunColorfulNonSpell(enemy, ctx, t);
            break;

        case 29:
            if (t == 0) StartSpellPhase(enemy, ctx, "Colorful Rain", 12000, 2, 1800, 20);
            RunRainbowSpell(enemy, ctx, t);
            break;

        case 20:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 12000, 1, 2400, 26, 26);
                DropPowerItems(enemy, ctx, 5);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 160) RunBlueBallNonSpell(enemy, ctx, t);
            break;

        case 26:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 16500, 0, 2400, 31, 34);
                DropPowerItems(enemy, ctx, 5);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 160) RunFinalNonSpell(enemy, ctx, t);
            break;

        case 31:
        case 32:
            if (t == 0) StartSpellPhase(enemy, ctx, "Extreme Color Typhoon", 14000, 0, 2160, 34);
            RunStarSpell(enemy, ctx, t);
            break;

        case 34:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                ctx.StartLerpTo(enemy, 192.0f, 96.0f, 90);
            }
            if (t == 150) enemy.m_Alive = false;
            break;

        default:
            break;
    }
}
