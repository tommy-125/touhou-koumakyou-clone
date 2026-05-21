#include "Scene/Stage6/Stage6Script.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <initializer_list>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace {
constexpr float PI     = 3.14159265f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float REMILIA_RANK_SPEED = 0.35f;
namespace ScriptUtil = EnemyScriptUtil;

constexpr int SUB_STAGE_EFFECTS       = 0;
constexpr int SUB_FAIRY_ARC_BLUE_L    = 1;
constexpr int SUB_FAIRY_ARC_BLUE_R    = 2;
constexpr int SUB_FAIRY_ARC_RED_L     = 3;
constexpr int SUB_FAIRY_ARC_RED_R     = 4;
constexpr int SUB_FAIRY_SIDE_BURST    = 5;
constexpr int SUB_FAIRY_DROP_8        = 6;
constexpr int SUB_FAIRY_TOP_BURST     = 7;

constexpr int SUB_SAKUYA_ENTRY        = 8;
constexpr int SUB_SAKUYA_MAIN         = 9;
constexpr int SUB_SAKUYA_DEATH        = 12;
constexpr int SUB_SAKUYA_EXIT         = 13;
constexpr int SUB_SAKUYA_ETERNAL_MEEK = 14;

constexpr int SUB_REMILIA_ENTRY       = 15;
constexpr int SUB_REMILIA_WAIT        = 16;
constexpr int SUB_REMILIA_NONSPELL_1  = 17;
constexpr int SUB_REMILIA_NONSPELL_2  = 19;
constexpr int SUB_REMILIA_NONSPELL_3  = 23;
constexpr int SUB_REMILIA_NONSPELL_4  = 26;
constexpr int SUB_REMILIA_AFTERIMAGE  = 29;
constexpr int SUB_REMILIA_STAR        = 30;
constexpr int SUB_REMILIA_SCARLET     = 32;
constexpr int SUB_REMILIA_VLAD        = 34;
constexpr int SUB_REMILIA_SHOOT       = 38;
constexpr int SUB_REMILIA_RED_MAGIC   = 43;
constexpr int SUB_REMILIA_DEATH       = 45;

float RandFloat(float min, float max) {
    return ScriptUtil::RandFloat(min, max);
}

float RandAngle() {
    return RandFloat(-PI, PI);
}

float AimAngle(glm::vec2 from, glm::vec2 to) {
    return std::atan2(to.y - from.y, to.x - from.x);
}

float EclStackSpeed(int stack, int stacks, float speed1, float speed2) {
    if (stacks <= 1) return speed1;
    return speed1 - (speed1 - speed2) * static_cast<float>(stack) /
                        static_cast<float>(stacks);
}

float RankedSpeed(float speed, float rankSpeed) {
    return speed + rankSpeed;
}

float RankedLowSpeed(float speed, float rankSpeed) {
    return speed + rankSpeed * 0.5f;
}

float MirrorAngle(float angle, bool mirrored) {
    return mirrored ? PI - angle : angle;
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f}) {
    return ScriptUtil::ShootPos(enemy, offset);
}

glm::vec2 ScreenToGameField(glm::vec2 pos) {
    return {pos.x - Util::FIELD_OFFSET_X, pos.y - Util::FIELD_OFFSET_Y};
}

void SetSakuyaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

void SetRemiliaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 160, 163, 164, 161, 162);
}

void DisableBossPoses(Enemy& enemy) {
    enemy.m_AnmDefault   = -1;
    enemy.m_AnmFarLeft   = -1;
    enemy.m_AnmFarRight  = -1;
    enemy.m_AnmLeft      = -1;
    enemy.m_AnmRight     = -1;
    enemy.m_AnmMoveState = 0xff;
}

void SpawnRandomArc(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                    int count, float minSpeed, float maxSpeed, float minAngle, float maxAngle,
                    bool rotate = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, RandFloat(minSpeed, maxSpeed),
                                RandFloat(minAngle, maxAngle), false, 0.0f, 0, rotate);
    }
}

void SpawnFanAbs(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color, int ways,
                 int stacks, float speed1, float speed2, float baseAngle, float spread,
                 bool rotate = false) {
    for (int s = 0; s < stacks; s++) {
        const float speed = EclStackSpeed(s, stacks, speed1, speed2);
        const float start = baseAngle - spread * static_cast<float>(ways - 1) * 0.5f;
        for (int i = 0; i < ways; i++) {
            ctx.bullets.SpawnCircle(pos, type, color, 1, speed, start + spread * i, false, 0.0f,
                                    0, rotate);
        }
    }
}

void SpawnAimedStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                     int count, int stacks, float speed1, float speed2, float spread = 0.0f,
                     float aimOffset = 0.0f, bool rotate = false) {
    if (spread == 0.0f) {
        for (int s = 0; s < stacks; s++) {
            const float speed = EclStackSpeed(s, stacks, speed1, speed2);
            ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, type, color, count, speed,
                                         aimOffset, false, 0.0f, {}, rotate);
        }
        return;
    }
    SpawnFanAbs(ctx, pos, type, color, count, stacks, speed1, speed2,
                AimAngle(pos, ctx.playerPos) + aimOffset, spread, rotate);
}

void SpawnCircleStackAbs(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                         int count, int stacks, float speed1, float speed2, float baseAngle,
                         float stackAngleOffset = 0.0f, bool rotate = false,
                         float acceleration = 0.0f, int accelerationFrames = 0,
                         float angularVelocity = 0.0f, int angularVelocityFrames = 0,
                         bool bounceTopAndSides = false, int bounceMax = 0) {
    for (int s = 0; s < stacks; ++s) {
        const float speed = EclStackSpeed(s, stacks, speed1, speed2);
        ctx.bullets.SpawnCircle(pos, type, color, count, speed,
                                baseAngle + static_cast<float>(s) * stackAngleOffset,
                                false, acceleration, accelerationFrames, rotate,
                                {0.0f, 0.0f}, 0, 0, 1.0f, {}, bounceTopAndSides, bounceMax,
                                speed, angularVelocity, angularVelocityFrames);
    }
}

void SpawnVampireBurst(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle, bool dense) {
    ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::Gray, 1, 6.2f, baseAngle);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, dense ? 5 : 3, 4.0f,
                   6.0f, baseAngle - 0.18f, baseAngle + 0.18f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, dense ? 10 : 6, 1.0f,
                   3.5f, baseAngle - 0.65f, baseAngle + 0.65f);
}

void SpawnRemiliaBloodBurst(EnemySubCtx& ctx, glm::vec2 pos, float angle,
                            float rankSpeed = 0.0f) {
    ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::Gray, 1,
                            RankedSpeed(6.0f, rankSpeed), angle);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, 5,
                   RankedLowSpeed(5.0f, rankSpeed), RankedSpeed(6.0f, rankSpeed),
                   angle - 0.09817477f, angle + 0.09817477f);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, 5,
                   RankedLowSpeed(4.0f, rankSpeed), RankedLowSpeed(5.0f, rankSpeed),
                   angle - 0.15707964f, angle + 0.15707964f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, 5,
                   RankedLowSpeed(2.0f, rankSpeed), RankedLowSpeed(4.0f, rankSpeed),
                   angle - 0.31415927f, angle + 0.31415927f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, 5,
                   RankedLowSpeed(1.0f, rankSpeed), RankedLowSpeed(2.0f, rankSpeed),
                   angle - 0.7853982f, angle + 0.7853982f);
}

void SpawnScarletShootBurst(EnemySubCtx& ctx, glm::vec2 pos, float angle) {
    ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::Gray, 1, 6.2f, angle);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, 3, 4.0f, 6.0f,
                   angle - 0.09817477f, angle + 0.09817477f);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, 5, 3.0f, 4.0f,
                   angle - 0.15707964f, angle + 0.15707964f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, 5, 2.0f, 3.0f,
                   angle - 0.31415927f, angle + 0.31415927f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, 10, 1.0f, 2.0f,
                   angle - 0.5235988f, angle + 0.5235988f);
}

void SpawnRemiliaOpeningPacket(EnemySubCtx& ctx, glm::vec2 pos, int step, float purpleSeed,
                               float rankSpeed = 0.0f) {
    const float blueBase   = 0.049087387f + step * 0.62831855f;
    const float bubbleBase = -Util::HALF_PI - step * 0.3926991f;
    const float purpleBase = purpleSeed + step * 0.15707964f;

    SpawnFanAbs(ctx, pos, EBulletType::Ball, EBulletColor::DarkBlue, 3, 1,
                RankedSpeed(1.8f, rankSpeed), RankedSpeed(1.8f, rankSpeed), blueBase,
                0.09817477f);
    SpawnFanAbs(ctx, pos, EBulletType::Bubble, EBulletColor::Gray, 4, 1,
                RankedSpeed(3.5f, rankSpeed), RankedSpeed(3.5f, rankSpeed), bubbleBase,
                1.0471976f);
    SpawnFanAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 2, 1,
                RankedSpeed(2.5f, rankSpeed), RankedSpeed(2.5f, rankSpeed), purpleBase,
                0.7853982f);
    SpawnFanAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 2, 1,
                RankedSpeed(2.5f, rankSpeed), RankedSpeed(2.5f, rankSpeed), purpleBase + PI,
                0.7853982f);
}

void SpawnScarletShootVolley(EnemySubCtx& ctx, glm::vec2 pos, std::initializer_list<float> offsets,
                             bool dense) {
    const float aim = AimAngle(pos, ctx.playerPos);
    for (float off : offsets) {
        if (dense) {
            SpawnVampireBurst(ctx, pos, aim + off, true);
        } else {
            SpawnScarletShootBurst(ctx, pos, aim + off);
        }
    }
}

void SpawnStage6StarLasers(EnemySubCtx& ctx, glm::vec2 center, int attackType) {
    const float randomAngle = RandAngle();
    for (int outer = 0; outer < 2; ++outer) {
        const float startAngle = (outer == 0 ? -PI : -7.0f * PI / 8.0f) + randomAngle;
        const float angleDiff  = outer == 0 ? PI / 4.0f : -PI / 4.0f;
        std::array<glm::vec2, 8> origins{};
        float                    originAngle = startAngle;
        for (int i = 0; i < 8; ++i) {
            origins[i] = center + glm::vec2{std::cos(originAngle), std::sin(originAngle)} * 32.0f;
            originAngle += PI / 4.0f;
        }

        float laserAngle = startAngle;
        for (int layer = 0; layer < 3; ++layer) {
            const float length = layer < 2 ? 112.0f : 480.0f;
            for (int i = 0; i < 8; ++i) {
                const float width = attackType == 0 ? 28.0f : 20.0f;
                ctx.lasers.SpawnAtAngle(origins[i], laserAngle, length, width, layer * 16 + 60,
                                        90 - layer * 16, 16, 50, 16);
                laserAngle += PI / 4.0f;
            }
            laserAngle += angleDiff - TWO_PI;
        }
    }
}

void SpawnAcceleratingRing(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                           int count, float speed, float baseAngle, float acceleration,
                           int frames, bool rotate = false,
                           float angularVelocity = 0.0f, bool vectorAcceleration = false,
                           float vectorAccelerationAngle = 0.0f) {
    const glm::vec2 vectorAccel =
        vectorAcceleration
            ? glm::vec2{std::cos(vectorAccelerationAngle) * acceleration,
                        std::sin(vectorAccelerationAngle) * acceleration}
            : glm::vec2{0.0f, 0.0f};
    ctx.bullets.SpawnCircle(pos, type, color, count, speed, baseAngle, false,
                            vectorAcceleration ? 0.0f : acceleration,
                            vectorAcceleration ? 0 : frames, rotate, vectorAccel,
                            vectorAcceleration ? frames : 0, 0, 1.0f, {}, false, 0, speed,
                            angularVelocity, angularVelocity == 0.0f ? 0 : frames);
}

void StartRemiliaPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                       int lifeCount, int timerFrames, int nextSub, int lifeThreshold,
                       bool spell, int deathSub = -1) {
    const int actualDeathSub = deathSub >= 0 ? deathSub : nextSub;
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  actualDeathSub,
                                  lifeThreshold,
                                  nextSub,
                                  spell,
                                  spell,
                                  0,
                                  true,
                                  true,
                              });
}

void RunArcFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool red, bool highArc) {
    if (t == 0) {
        enemy.m_Angle = MirrorAngle(highArc ? 0.5235988f : -1.0471976f, enemy.m_Mirrored);
        enemy.m_Speed = highArc ? 4.5f : 4.0f;
    }
    if (t == 30) enemy.m_AngularVelocity = (enemy.m_Mirrored ? -1.0f : 1.0f) *
                                           (highArc ? -0.06544985f : 0.034906585f);
    if (t == (highArc ? 115 : 90)) enemy.m_AngularVelocity = 0.0f;
    if (t == 80) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        if (red) {
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Red, 9, 1.0f, 2.0f,
                           -PI, PI);
        } else {
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Blue, 6, 1.6f, 1.6f,
                           -PI, PI);
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Blue, 6, 1.0f, 1.0f,
                           -PI, PI);
        }
    }
}

void RunBurstFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool top) {
    if (t == 0) {
        enemy.m_Angle = top ? Util::HALF_PI : (enemy.m_Mirrored ? PI : 0.0f);
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) {
        enemy.m_Acceleration = -0.06666667f;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Rice,
                                     EBulletColor::Blue, 60, 1.6f);
    }
    if (t == 42) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Angle = RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.8f;
    }
}

void RunSakuyaMain(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        BossPhaseUtil::StartPhase(enemy, ctx,
                                  {
                                      "Sakuya Izayoi",
                                      -1,
                                      0,
                                      1020,
                                      SUB_SAKUYA_EXIT,
                                      SUB_SAKUYA_DEATH,
                                      1000,
                                      SUB_SAKUYA_ETERNAL_MEEK,
                                      false,
                                      false,
                                      0,
                                      false,
                                      true,
                                  });
        enemy.m_CanTakeDamage = true;
        ctx.SetTimeStopped(false);
    }
    if (t < 30) return;
    const int loopT = (t - 30) % 130;
    const auto pos  = ShootPos(enemy);
    if (loopT < 48 && loopT % 4 == 0) {
        SpawnFanAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f,
                    3.2f, (loopT / 4) * 0.2617994f, 0.2617994f, true);
    }
    if (loopT == 48) {
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
    if (loopT >= 48 && loopT < 64) {
        SpawnFanAbs(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 5, 1, 2.5f, 2.5f,
                    RandAngle(), 0.044879895f, true);
    }
    if (loopT >= 65 && loopT < 113 && (loopT - 65) % 4 == 0) {
        SpawnFanAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f,
                    3.2f, PI - ((loopT - 65) / 4) * 0.2617994f, 0.2617994f, true);
    }
    if (loopT >= 113 && loopT < 129) {
        SpawnFanAbs(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 5, 1, 2.5f, 2.5f,
                    RandAngle(), 0.044879895f, true);
    }
}

void RunEternalMeek(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        BossPhaseUtil::StartPhase(enemy, ctx,
                                  {
                                      "Conjuring \"Eternal Meek\"",
                                      -1,
                                      0,
                                      900,
                                      SUB_SAKUYA_EXIT,
                                      SUB_SAKUYA_DEATH,
                                      -1,
                                      -1,
                                      true,
                                      true,
                                      0,
                                      false,
                                      true,
                                  });
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 4;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0) {
        SpawnRandomArc(ctx, pos, EBulletType::Ball, EBulletColor::Blue, 4, 3.0f, 6.0f, 0.0f,
                       PI);
        SpawnRandomArc(ctx, pos, EBulletType::Ball, EBulletColor::Blue, 12, 3.0f, 5.0f, -PI,
                       0.0f);
    }
}

void RunRemiliaNonSpell1(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 13000, 4, 2700, SUB_REMILIA_STAR,
                          1200, false, SUB_REMILIA_NONSPELL_2);
    }
    if (t < 100) return;
    const int loopT = (t - 100) % 720;
    if (loopT == 0) enemy.m_LockedShotAngle = RandAngle();
    if (loopT < 256 && loopT % 8 == 0) {
        const auto pos = ShootPos(enemy);
        SpawnRemiliaOpeningPacket(ctx, pos, loopT / 8, enemy.m_LockedShotAngle,
                                  REMILIA_RANK_SPEED);
    }
    if (loopT == 256) {
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
    if (loopT == 360) enemy.m_SecondaryShotAngle = RandAngle();
    if (loopT >= 360 && loopT < 616 && loopT % 8 == 0) {
        const auto pos = ShootPos(enemy);
        SpawnRemiliaOpeningPacket(ctx, pos, (loopT - 360) / 8, enemy.m_SecondaryShotAngle,
                                  REMILIA_RANK_SPEED);
    }
}

void RunRemiliaNonSpell2(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 15500, 3, 2700, SUB_REMILIA_SCARLET,
                          1600, false, SUB_REMILIA_NONSPELL_3);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
    }
    if (t < 60) return;
    const int loopT = (t - 60) % 1120;
    const auto pos  = ShootPos(enemy);
    if (loopT >= 190 && loopT <= 270 && (loopT - 190) % 20 == 0) {
        SpawnRemiliaBloodBurst(ctx, pos, AimAngle(pos, ctx.playerPos), REMILIA_RANK_SPEED);
    }
    if (loopT >= 480 && loopT <= 585 && (loopT - 480) % 15 == 0) {
        SpawnRemiliaBloodBurst(ctx, pos, static_cast<float>(loopT - 480) * 0.3926991f / 15.0f,
                               REMILIA_RANK_SPEED);
    }
    if (loopT >= 790 && loopT <= 895 && (loopT - 790) % 15 == 0) {
        const float spin = PI - static_cast<float>(loopT - 790) * 0.3926991f / 15.0f;
        SpawnRemiliaBloodBurst(ctx, pos, spin, REMILIA_RANK_SPEED);
    }
    if (loopT >= 1010 && loopT <= 1090 && (loopT - 1010) % 20 == 0) {
        SpawnRemiliaBloodBurst(ctx, pos, AimAngle(pos, ctx.playerPos), REMILIA_RANK_SPEED);
    }
    if (loopT == 0 || loopT == 290 || loopT == 600) {
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
}

void RunRemiliaNonSpell3(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 11000, 2, 2700, SUB_REMILIA_VLAD,
                          1600, false, SUB_REMILIA_NONSPELL_4);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
    }
    if (t < 60) return;
    const int loopT = (t - 60) % 478;
    const auto pos  = ShootPos(enemy);
    if (loopT == 0) enemy.m_LockedShotAngle = RandFloat(-PI, PI);
    if (loopT < 120 && loopT % 2 == 0) {
        const float base =
            enemy.m_LockedShotAngle + static_cast<float>(loopT / 2) * 0.09817477f;
        SpawnCircleStackAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkRed, 4, 2,
                            3.0f, 1.2f, base, 0.0f, true);
    }
    if (loopT == 120) {
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
    if (loopT == 248) enemy.m_SecondaryShotAngle = RandFloat(-PI, PI);
    if (loopT >= 248 && loopT < 368 && loopT % 2 == 0) {
        const float base =
            enemy.m_SecondaryShotAngle - static_cast<float>((loopT - 248) / 2) * 0.09817477f;
        SpawnCircleStackAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 4, 2,
                            3.0f, 1.2f, base, 0.0f, true);
    }
}

void RunRemiliaNonSpell4(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 13000, 1, 3600, SUB_REMILIA_SHOOT,
                          1300, false, SUB_REMILIA_RED_MAGIC);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
        SetRemiliaPoses(enemy);
    }
    if (t < 60) return;
    const int loopT = (t - 60) % 430;
    const auto pos  = ShootPos(enemy);
    const int cycle = ((t - 60) / 430) % 3;
    if (loopT == 100) {
        DisableBossPoses(enemy);
        ctx.anm.SetScript(enemy.m_Vm, Anm::STG6ENM2.offset + 165, Anm::STG6ENM2.offset);
        enemy.m_CanTakeDamage = false;
    }
    if (loopT >= 100 && loopT < 130) {
        const glm::vec2 field = ScreenToGameField(enemy.m_Pos);
        ctx.SpawnEnemy(SUB_REMILIA_AFTERIMAGE, field.x, field.y, 100, 0, false, -99);
        ctx.SpawnEnemy(SUB_REMILIA_AFTERIMAGE, field.x, field.y, 100, 0, false, -99);
    }
    if (loopT >= 130 && loopT < 340) {
        if (cycle == 0 && (loopT - 130) % 8 == 0) {
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Red, 11, 1.0f, 2.0f, -PI,
                           PI);
        } else if (cycle == 1 && (loopT - 130) % 16 == 0) {
            SpawnAimedStack(ctx, pos, EBulletType::Bubble, EBulletColor::Gray, 9, 1, 5.0f,
                            5.0f, 0.34906584f, 0.0f);
        } else if (cycle == 2 && (loopT - 130) % 10 == 0) {
            SpawnAimedStack(ctx, pos, EBulletType::Fireball, EBulletColor::DarkRed, 12, 2,
                            5.0f, 2.0f, 0.0f, 0.0f, true);
        }
    }
    if (loopT >= 130 && loopT <= 310 && (loopT - 130) % 30 == 0) {
        ScriptUtil::StartRandomMove(enemy, ctx, ((loopT - 130) / 30) % 2 == 0 ? 4.0f : 7.0f,
                                    30);
    }
    if (loopT == 340) {
        SetRemiliaPoses(enemy);
        enemy.m_CanTakeDamage = true;
    }
}

void RunStarOfDavid(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Heaven's Punishment \"Star of David\"", -1, 4, 2400,
                          SUB_REMILIA_NONSPELL_2, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 184;
    if (loopT == 64) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        SpawnStage6StarLasers(ctx, pos, 0);
        const float base = RandAngle();
        SpawnCircleStackAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 5, 1,
                            0.2f, 1.0f, base, 0.0f, false, 0.01f, 120);
        SpawnCircleStackAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 3, 1,
                            0.2f, 1.0f, base + PI / 5.0f, 0.0f, false, 0.01f, 120);
    }
    if (loopT == 124) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
}

void RunScarletNetherworld(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Nether Sign \"Scarlet Netherworld\"", -1, 3, 2400,
                          SUB_REMILIA_NONSPELL_3, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 231;
    if (loopT < 126) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        const int   phase = loopT % 21;
        const float base  = RandAngle() + static_cast<float>(loopT / 21) * 0.3926991f;
        if (phase == 0) {
            SpawnAcceleratingRing(ctx, pos, EBulletType::Rice, EBulletColor::Red, 24, 1.8f,
                                  base, 0.0f, 128, true, 0.024543693f);
        } else if (phase == 4) {
            SpawnAcceleratingRing(ctx, pos, EBulletType::Rice, EBulletColor::Red, 24, 1.8f,
                                  base + 0.09817477f, 0.0f, 128, true, -0.024543693f);
        } else if (phase == 8 || phase == 12) {
            SpawnAcceleratingRing(ctx, pos, EBulletType::Shard, EBulletColor::Red, 16, 2.2f,
                                  base + static_cast<float>(phase) * 0.024543693f, 0.02f,
                                  240, true, 0.0f, true, Util::HALF_PI);
        }
    }
    if (loopT == 141) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

void RunVlad(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Curse \"Curse of Vlad Tepes\"", -1, 2, 2400,
                          SUB_REMILIA_NONSPELL_4, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
        enemy.m_ScriptState   = 0;
        enemy.m_ScriptTimer   = 0;
        enemy.m_ScriptCounter = 0;
    }
    if (t == 120) {
        enemy.m_CanTakeDamage = true;
        enemy.m_ScriptState   = 0;
        enemy.m_ScriptTimer   = 0;
        enemy.m_ScriptCounter = 0;
    }
    if (t < 120) return;
    const auto pos = ShootPos(enemy, {0.0f, 0.0f});
    if (enemy.m_ScriptState == 0) {
        const int   count = std::min(18, 13 + enemy.m_ScriptCounter);
        const float base  = 0.0f;
        ctx.bullets.SpawnCircle(pos, EBulletType::Dagger, EBulletColor::DarkPurple, count, 3.0f,
                                base, false, 0.0f, 0, true, {0.0f, 0.0f}, 0, 0, 1.0f,
                                {}, false, 0, -1.0f,
                                enemy.m_ScriptCounter % 2 == 0 ? 0.019634955f : -0.019634955f,
                                108);
        enemy.m_ScriptState = 1;
        enemy.m_ScriptTimer = 0;
        return;
    }

    if (enemy.m_ScriptTimer % 14 == 0) {
        const int created = ctx.bullets.Stage6CreateSeedsFromLargeBullets();
        if (created == 0 && ctx.bombActive) {
            ++enemy.m_ScriptTimer;
            return;
        }
        if (created == 0 || enemy.m_ScriptTimer >= 448) {
            ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
            ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
            ++enemy.m_ScriptCounter;
            enemy.m_ScriptState = 0;
            enemy.m_ScriptTimer = 0;
            return;
        }
    }
    ++enemy.m_ScriptTimer;
}

void RunScarletShoot(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Scarlet Sign \"Scarlet Shoot\"", -1, 1, 1800,
                          SUB_REMILIA_RED_MAGIC, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 544;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0 || loopT == 60 || loopT == 270) {
        SpawnScarletShootVolley(ctx, pos, {0.0f, 0.7853982f, -0.7853982f, 1.5707964f,
                                           -1.5707964f},
                                false);
    }
    if (loopT == 150) {
        SpawnScarletShootVolley(ctx, pos, {0.0f, 0.07853982f, -0.07853982f}, false);
    }
    if (loopT == 300) {
        SpawnScarletShootVolley(ctx, pos, {0.0f, 1.0471976f, -1.0471976f, 2.0943952f,
                                           -2.0943952f},
                                false);
    }
    if (loopT == 60 || loopT == 180) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
}

void RunRedMagic(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "\"Red Magic\"", 4700, 0, 7200, SUB_REMILIA_DEATH, -1,
                          true);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 128.0f, 120);
    }
    if (t == 180) enemy.m_CanTakeDamage = true;
    if (t < 180) return;
    const int loopT = (t - 180) % 1056;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0) {
        ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::DarkRed, 10, 2.0f,
                                RandAngle());
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT < 170 && loopT % 17 == 0) {
        ctx.bullets.Stage6CreateSeedsFromLargeBullets();
    }
    if (loopT == 186) {
        ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
        ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::DarkRed, 12, 2.0f,
                                RandAngle());
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT >= 186 && loopT < 356 && (loopT - 186) % 17 == 0) {
        ctx.bullets.Stage6CreateSeedsFromLargeBullets();
    }
    if (loopT == 356) {
        ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
        ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::DarkRed, 8, 2.0f,
                                RandAngle());
    }
    if (loopT >= 356 && loopT < 526 && (loopT - 356) % 17 == 0) {
        ctx.bullets.Stage6CreateSeedsFromLargeBullets();
    }
    if (loopT == 526) {
        ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT == 576) {
        ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::DarkRed, 15, 2.0f,
                                RandAngle());
    }
    if (loopT >= 576 && loopT < 746 && (loopT - 576) % 17 == 0) {
        ctx.bullets.Stage6CreateSeedsFromLargeBullets();
    }
    if (loopT == 746) {
        ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT == 786) {
        SpawnCircleStackAbs(ctx, pos, EBulletType::Bubble, EBulletColor::DarkRed, 8, 2, 4.0f,
                            1.0f, RandAngle(), -0.3926991f);
    }
    if (loopT >= 786 && loopT < 956 && (loopT - 786) % 17 == 0) {
        ctx.bullets.Stage6CreateSeedsFromLargeBullets();
    }
    if (loopT == 956) {
        ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
    }
}

}  // namespace

void Stage6Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG6ENM.folder, Anm::STG6ENM.txt, Anm::STG6ENM.offset);
    anm.LoadAnm(Anm::STG6ENM2.folder, Anm::STG6ENM2.txt, Anm::STG6ENM2.offset);
}

void Stage6Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG6ENM.offset;
    switch (enemy.m_SubId) {
        case SUB_STAGE_EFFECTS:
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            enemy.m_ItemDropCount = 0;
            break;
        case SUB_FAIRY_ARC_BLUE_L:
        case SUB_FAIRY_ARC_RED_L:
            ctx.anm.SetScript(enemy.m_Vm, offset + 8, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            break;
        case SUB_FAIRY_ARC_BLUE_R:
        case SUB_FAIRY_ARC_RED_R:
            ctx.anm.SetScript(enemy.m_Vm, offset + 9, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            break;
        case SUB_FAIRY_SIDE_BURST:
        case SUB_FAIRY_TOP_BURST:
            ctx.anm.SetScript(enemy.m_Vm, offset + 11, offset);
            enemy.m_HitboxSize       = {28.0f, 28.0f};
            enemy.m_DeathCallbackSub = SUB_FAIRY_DROP_8;
            enemy.m_ItemDropCount    = 0;
            break;
        case SUB_FAIRY_DROP_8:
            ctx.anm.SetScript(enemy.m_Vm, offset + 11, offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            enemy.m_ItemDropCount = 0;
            break;
        case SUB_SAKUYA_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 128, offset);
            enemy.m_Pos            = Util::GameFieldToScreen(-32.0f, 128.0f);
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = true;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Sakuya Izayoi";
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 120.0f);
            enemy.m_HitboxSize     = {40.0f, 56.0f};
            SetSakuyaPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;
        case SUB_REMILIA_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, Anm::STG6ENM2.offset + 160, Anm::STG6ENM2.offset);
            enemy.m_Pos            = Util::GameFieldToScreen(256.0f, -64.0f);
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = true;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Remilia Scarlet";
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 120.0f);
            enemy.m_HitboxSize     = {56.0f, 56.0f};
            SetRemiliaPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;
        case SUB_REMILIA_AFTERIMAGE:
            ctx.anm.SetScript(enemy.m_Vm, Anm::STG6ENM2.offset + 165, Anm::STG6ENM2.offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            enemy.m_ItemDropCount = 0;
            enemy.m_ItemDrop      = -1;
            enemy.m_Angle         = RandFloat(0.0f, PI);
            enemy.m_Speed         = RandFloat(4.0f, 5.0f);
            break;
        default:
            break;
    }
}

void Stage6Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;
    switch (enemy.m_SubId) {
        case SUB_STAGE_EFFECTS:
            if (t >= 2160) enemy.m_Alive = false;
            break;
        case SUB_FAIRY_ARC_BLUE_L:
        case SUB_FAIRY_ARC_BLUE_R:
            RunArcFairy(enemy, ctx, t, false, true);
            break;
        case SUB_FAIRY_ARC_RED_L:
        case SUB_FAIRY_ARC_RED_R:
            RunArcFairy(enemy, ctx, t, true, false);
            break;
        case SUB_FAIRY_SIDE_BURST:
            RunBurstFairy(enemy, ctx, t, false);
            break;
        case SUB_FAIRY_TOP_BURST:
            RunBurstFairy(enemy, ctx, t, true);
            break;
        case SUB_FAIRY_DROP_8:
            if (t == 0) {
                ScriptUtil::DropPowerItems(enemy, ctx, 8);
                enemy.m_Alive = false;
            }
            break;
        case SUB_SAKUYA_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_SAKUYA_MAIN);
            break;
        case SUB_SAKUYA_MAIN:
            RunSakuyaMain(enemy, ctx, t);
            break;
        case SUB_SAKUYA_ETERNAL_MEEK:
            RunEternalMeek(enemy, ctx, t);
            break;
        case SUB_SAKUYA_DEATH:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ScriptUtil::DropPowerItems(enemy, ctx, 20);
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                ctx.BulletCancelIntoPointItems();
                enemy.m_Alive = false;
            }
            break;
        case SUB_SAKUYA_EXIT:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ctx.BulletCancelIntoPointItems();
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t >= 60) enemy.m_Alive = false;
            break;
        case SUB_REMILIA_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
            if (t == 120) ctx.TransitionToSub(enemy, SUB_REMILIA_WAIT);
            break;
        case SUB_REMILIA_WAIT:
            if (t == 0) ctx.TransitionToSub(enemy, SUB_REMILIA_NONSPELL_1);
            break;
        case SUB_REMILIA_NONSPELL_1:
            RunRemiliaNonSpell1(enemy, ctx, t);
            break;
        case SUB_REMILIA_STAR:
            RunStarOfDavid(enemy, ctx, t);
            break;
        case SUB_REMILIA_NONSPELL_2:
            RunRemiliaNonSpell2(enemy, ctx, t);
            break;
        case SUB_REMILIA_SCARLET:
            RunScarletNetherworld(enemy, ctx, t);
            break;
        case SUB_REMILIA_NONSPELL_3:
            RunRemiliaNonSpell3(enemy, ctx, t);
            break;
        case SUB_REMILIA_VLAD:
            RunVlad(enemy, ctx, t);
            break;
        case SUB_REMILIA_NONSPELL_4:
            RunRemiliaNonSpell4(enemy, ctx, t);
            break;
        case SUB_REMILIA_AFTERIMAGE:
            if (t == 30) enemy.m_AngularVelocity = RandFloat(-0.3926991f, 0.3926991f) / 20.0f;
            if (t >= 150) enemy.m_Alive = false;
            break;
        case SUB_REMILIA_SHOOT:
            RunScarletShoot(enemy, ctx, t);
            break;
        case SUB_REMILIA_RED_MAGIC:
            RunRedMagic(enemy, ctx, t);
            break;
        case SUB_REMILIA_DEATH:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                ctx.BulletCancelIntoPointItems();
                enemy.m_Alive = false;
            }
            break;
        default:
            break;
    }
}
