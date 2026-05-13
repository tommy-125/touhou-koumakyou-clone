#include "Scene/Stage5/Stage5Script.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

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
constexpr float     PI                  = 3.14159265f;
constexpr glm::vec2 SAKUYA_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil = EnemyScriptUtil;

constexpr int SUB_MAID_DROP_4 = 7;
constexpr int SUB_MAID_DROP_6 = 8;

constexpr int SUB_SAKUYA_MIDBOSS_ENTRY = 12;
constexpr int SUB_SAKUYA_MIDBOSS_MAIN  = 13;
constexpr int SUB_SAKUYA_MIDBOSS_DEATH = 16;
constexpr int SUB_SAKUYA_MIDBOSS_EXIT  = 17;
constexpr int SUB_SAKUYA_MISDIRECTION  = 18;

constexpr int SUB_SAKUYA_ENTRY            = 21;
constexpr int SUB_SAKUYA_FIRST_NONSPELL   = 22;
constexpr int SUB_SAKUYA_HELPER_1         = 25;
constexpr int SUB_SAKUYA_HELPER_2         = 26;
constexpr int SUB_SAKUYA_HELPER_3         = 27;
constexpr int SUB_SAKUYA_HELPER_4         = 28;
constexpr int SUB_SAKUYA_HELPER_5         = 29;
constexpr int SUB_SAKUYA_HELPER_6         = 30;
constexpr int SUB_SAKUYA_HELPER_7         = 31;
constexpr int SUB_SAKUYA_HELPER_8         = 32;
constexpr int SUB_SAKUYA_HELPER_DEATH     = 33;
constexpr int SUB_SAKUYA_SECOND_NONSPELL  = 34;
constexpr int SUB_SAKUYA_KUNAI_HELPER_1   = 36;
constexpr int SUB_SAKUYA_KUNAI_HELPER_2   = 37;
constexpr int SUB_SAKUYA_KUNAI_HELPER_3   = 38;
constexpr int SUB_SAKUYA_KUNAI_HELPER_4   = 39;
constexpr int SUB_SAKUYA_FINAL_NONSPELL   = 41;
constexpr int SUB_SAKUYA_CLOCK_CORPSE     = 44;
constexpr int SUB_SAKUYA_LUNA_CLOCK       = 46;
constexpr int SUB_SAKUYA_FINAL_SPELL      = 50;
constexpr int SUB_SAKUYA_DEATH            = 51;

float RandAngle() {
    return ScriptUtil::RandFloat(-PI, PI);
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = SAKUYA_SHOOT_OFFSET) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetSakuyaBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

void StartSakuyaPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                      int lifeCount, int timerFrames, int nextSub, int lifeThreshold,
                      bool spell) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  nextSub,
                                  lifeThreshold,
                                  nextSub,
                                  spell,
                                  spell,
                                  0,
                                  false,
                                  true,
                              });
}

void SpawnRandomCircle(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                       int count, float minSpeed, float maxSpeed, bool rotate = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1,
                                ScriptUtil::RandFloat(minSpeed, maxSpeed), RandAngle(), false,
                                0.0f, 0, rotate);
    }
}

void SpawnAimedCircleStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                           int count, int stacks, float speed1, float speed2,
                           float aimOffset = 0.0f, bool rotate = false) {
    for (int s = 0; s < stacks; s++) {
        const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
        const float speed = speed1 + (speed2 - speed1) * lerp;
        ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, type, color, count, speed, aimOffset,
                                     false, 0.0f, {}, rotate);
    }
}

void SpawnDownDaggerRing(EnemySubCtx& ctx, glm::vec2 pos, EBulletColor color, int count,
                         int stacks, float speed1, float speed2, float angleOffset) {
    const glm::vec2 downTarget = pos + glm::vec2{0.0f, 1.0f};
    for (int s = 0; s < stacks; s++) {
        const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
        const float speed = speed1 + (speed2 - speed1) * lerp;
        ctx.bullets.SpawnCircleAimed(pos, downTarget, EBulletType::Dagger, color, count, speed,
                                     angleOffset, false, 0.0f, {}, true);
    }
}

glm::vec2 ScreenToGameField(glm::vec2 pos) {
    return {pos.x - Util::FIELD_OFFSET_X, pos.y - Util::FIELD_OFFSET_Y};
}

float FanDelta(int count, int index, float spread) {
    int   step  = (count & 1) ? (index + 1) / 2 : index / 2;
    float delta = static_cast<float>(step) * spread;
    if (!(count & 1)) delta += spread * 0.5f;
    return (index & 1) ? -delta : delta;
}

void SpawnFanAbsolute(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                      int ways, int stacks, float speed1, float speed2, float baseAngle,
                      float spread, bool rotateWithAngle = false) {
    for (int s = 0; s < stacks; s++) {
        const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
        const float speed = speed1 + (speed2 - speed1) * lerp;
        for (int i = 0; i < ways; i++) {
            ctx.bullets.SpawnCircle(pos, type, color, 1, speed, baseAngle + FanDelta(ways, i, spread),
                                    false, 0.0f, 0, rotateWithAngle);
        }
    }
}

void SpawnSakuyaHelper(Enemy& enemy, EnemySubCtx& ctx, int subId) {
    const glm::vec2 field = ScreenToGameField(enemy.m_Pos);
    ctx.SpawnEnemy(subId, field.x, field.y, 100, 0, false, -99);
}

void SpawnFirstNonspellHelpers(Enemy& enemy, EnemySubCtx& ctx) {
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_HELPER_1);
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_HELPER_2);
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_HELPER_3);
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_HELPER_4);
}

void SpawnSecondNonspellHelpers(Enemy& enemy, EnemySubCtx& ctx) {
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_KUNAI_HELPER_1);
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_KUNAI_HELPER_2);
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_KUNAI_HELPER_3);
    SpawnSakuyaHelper(enemy, ctx, SUB_SAKUYA_KUNAI_HELPER_4);
}

void SpawnTimeStopKnifeLattice(Enemy& enemy, EnemySubCtx& ctx, int patternPosition) {
    const glm::vec2 toPlayer = ctx.playerPos - enemy.m_Pos;
    float base = std::atan2(toPlayer.y, toPlayer.x);
    if ((patternPosition & 1) != 0) base += PI;

    const glm::vec2 side = {std::cos(base + Util::HALF_PI), std::sin(base + Util::HALF_PI)};
    const glm::vec2 dir  = {std::cos(base), std::sin(base)};
    const float sideOffset = (patternPosition & 1) ? -256.0f : 256.0f;
    const float forwardOffset = 32.0f + static_cast<float>(patternPosition) * 8.0f;
    const glm::vec2 anchor = enemy.m_Pos + side * sideOffset + dir * forwardOffset;

    for (int i = 0; i < 9; i++) {
        const float local = -PI / 4.0f + static_cast<float>(i) * (PI / 18.0f);
        const glm::vec2 pos = anchor + glm::vec2{std::cos(base + local), std::sin(base + local)} *
                                           96.0f;
        ctx.bullets.SpawnCircle(pos, EBulletType::Dagger, EBulletColor::DarkPurple, 1, 2.0f,
                                base + local, false, 0.0f, 0, true);
    }
}

struct HelperPattern {
    float        moveAngle = 0.0f;
    EBulletType type      = EBulletType::Rice;
    EBulletColor color    = EBulletColor::Blue;
    int          ways     = 5;
    int          stacks   = 1;
    float        speed1   = 1.6f;
    float        speed2   = 1.2f;
    float        spread   = 0.02617994f;
    float        angleInc = 0.31415927f;
    int          interval = 6;
    int          shots    = 40;
};

HelperPattern GetSakuyaHelperPattern(int subId) {
    switch (subId) {
        case SUB_SAKUYA_HELPER_1:
            return {0.7853982f, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f, 1.2f,
                    0.02617994f, 0.62831855f, 6, 40};
        case SUB_SAKUYA_HELPER_2:
            return {0.0f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f, 1.2f,
                    0.02617994f, -0.31415927f, 6, 40};
        case SUB_SAKUYA_HELPER_3:
            return {2.3561945f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f, 1.2f,
                    0.02617994f, -0.62831855f, 6, 40};
        case SUB_SAKUYA_HELPER_4:
            return {PI, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f, 1.2f,
                    0.02617994f, 0.31415927f, 6, 40};
        case SUB_SAKUYA_HELPER_5:
            return {1.1780972f, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f, 1.2f,
                    0.02617994f, 0.62831855f, 6, 40};
        case SUB_SAKUYA_HELPER_6:
            return {1.9634954f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f, 1.2f,
                    0.02617994f, -0.62831855f, 6, 40};
        case SUB_SAKUYA_HELPER_7:
            return {0.3926991f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f, 1.2f,
                    0.02617994f, -0.31415927f, 6, 40};
        case SUB_SAKUYA_HELPER_8:
            return {2.7488935f, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f, 1.2f,
                    0.02617994f, 0.31415927f, 6, 40};
        case SUB_SAKUYA_KUNAI_HELPER_1:
            return {0.7853982f, EBulletType::Kunai, EBulletColor::Red, 6, 1, 1.6f, 1.4f,
                    0.015707964f, 0.62831855f, 14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_2:
            return {0.0f, EBulletType::Kunai, EBulletColor::Blue, 6, 1, 1.6f, 1.4f,
                    0.015707964f, -0.19634955f, 14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_3:
            return {2.3561945f, EBulletType::Kunai, EBulletColor::Blue, 6, 1, 1.6f, 1.4f,
                    0.015707964f, -0.62831855f, 14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_4:
            return {PI, EBulletType::Kunai, EBulletColor::Red, 6, 1, 1.6f, 1.4f,
                    0.015707964f, 0.19634955f, 14, 17};
        default:
            return {};
    }
}

void InitSakuyaHelper(Enemy& enemy, const HelperPattern& pattern) {
    enemy.m_Angle            = pattern.moveAngle;
    enemy.m_Speed            = 0.8f;
    enemy.m_Acceleration     = -0.8f / 370.0f;
    enemy.m_LockedShotAngle  = RandAngle();
    enemy.m_CanTakeDamage    = false;
    enemy.m_HitboxSize       = {0.0f, 0.0f};
    enemy.m_ItemDropCount    = 0;
    enemy.m_DeathCallbackSub = SUB_SAKUYA_HELPER_DEATH;
}

void RunSakuyaHelper(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const HelperPattern pattern = GetSakuyaHelperPattern(enemy.m_SubId);
    const int fireStart = 50;
    const int fireEnd   = fireStart + pattern.interval * pattern.shots;
    if (t >= fireStart && t < fireEnd && (t - fireStart) % pattern.interval == 0) {
        const int shot = (t - fireStart) / pattern.interval;
        SpawnFanAbsolute(ctx, enemy.m_Pos, pattern.type, pattern.color, pattern.ways,
                         pattern.stacks, pattern.speed1, pattern.speed2,
                         enemy.m_LockedShotAngle + pattern.angleInc * static_cast<float>(shot),
                         pattern.spread, pattern.type == EBulletType::Kunai);
    }
    if (t >= fireEnd + 60) enemy.m_Alive = false;
}

void RunTopMaid(Enemy& enemy, EnemySubCtx& ctx, int t, EBulletColor color, bool dense,
                bool randomFan) {
    if (t == 0) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) enemy.m_Acceleration = -0.06666667f;
    if (t == 70) {
        enemy.m_Acceleration = 0.0f;
        const int bursts     = dense ? 5 : 8;
        for (int i = 0; i < bursts; i++) {
            const int   fireT = 70 + i * (dense ? 4 : 5);
            const float speed = 1.5f + i * (dense ? 0.55f : 0.38f);
            if (t == fireT) {
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                             color, dense ? 24 : 30, speed, 0.0f, false);
            }
        }
    }
    if (t > 70 && t < 115 && ((dense && (t - 70) % 4 == 0) || (!dense && (t - 70) % 5 == 0))) {
        const int   step  = (t - 70) / (dense ? 4 : 5);
        const float speed = 1.5f + step * (dense ? 0.55f : 0.38f);
        if (step < (dense ? 5 : 8)) {
            if (randomFan) {
                ctx.bullets.SpawnFanAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                          color, 24, speed, 0.1308997f, 0.0f);
            } else {
                ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Ball,
                                             color, dense ? 24 : 30, speed, 0.0f, false);
            }
        }
    }
    if (t == 115) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 1.8f;
    }
}

void RunSideMaid(Enemy& enemy, EnemySubCtx& ctx, int t, int variant) {
    if (t == 0) {
        enemy.m_Angle = enemy.m_Mirrored ? PI : 0.0f;
        enemy.m_Speed = 2.5f;
    }
    const int interval = variant == 11 ? 10 : (variant == 10 ? 30 : 40);
    if (t % interval != 0) return;

    const auto pos = ShootPos(enemy, {0.0f, 0.0f});
    if (variant == 10) {
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Kunai, EBulletColor::Red, 4,
                                  1.5f, 0.0f, 0.2617994f, false, true);
    } else if (variant == 11) {
        SpawnRandomCircle(pos, ctx, EBulletType::Ball, EBulletColor::Red, 4, 1.0f, 2.3f);
    } else {
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Pellet, EBulletColor::Blue, 6,
                                  1.5f, 0.0f, 0.19634955f);
    }
}

void RunRingMaid(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) enemy.m_Acceleration = -0.06666667f;
    if (t == 70) enemy.m_Acceleration = 0.0f;
    if (t >= 70 && t < 190 && (t - 70) % 3 == 0) {
        const float angle = ScriptUtil::RandFloat(3.5342917f, 5.9269907f);
        const float speed = ScriptUtil::RandFloat(1.0f, 2.0f);
        const auto  aim   = ShootPos(enemy) + glm::vec2{std::cos(angle), std::sin(angle)};
        ctx.bullets.SpawnFanAimed(ShootPos(enemy), aim, EBulletType::RingBall, EBulletColor::Blue,
                                  5, speed, 0.0f, 0.03926991f);
    }
    if (t == 190) {
        enemy.m_Angle = Util::HALF_PI;
        enemy.m_Speed = 1.8f;
    }
}

void RunMisdirection(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartSakuyaPhase(enemy, ctx, "Illusion Sign \"Misdirection\"", -1, 0, 1800,
                         SUB_SAKUYA_MIDBOSS_EXIT, -1, true);
        enemy.m_DeathCallbackSub = SUB_SAKUYA_MIDBOSS_DEATH;
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 248;
    const auto pos  = ShootPos(enemy);
    if (loopT == 0) {
        SpawnAimedCircleStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 24, 2,
                              2.4f, 1.2f, 0.0f, true);
        ScriptUtil::StartRandomMove(enemy, ctx, 1.8f, 90);
    }
    if (loopT == 90) {
        SpawnAimedCircleStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkRed, 24, 2, 2.2f,
                              1.0f, 0.15707964f, true);
    }
    if (loopT == 160) ctx.StartLerpTo(enemy, 192.0f, 144.0f, 60);
}

void RunMidbossMain(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_BossTitle              = "Sakuya Izayoi";
        enemy.m_BossLifeCount          = 0;
        enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
        enemy.m_BossTimer              = 0;
        enemy.m_TimerCallbackThreshold = 2400;
        enemy.m_TimerCallbackSub       = SUB_SAKUYA_MIDBOSS_EXIT;
        enemy.m_LifeCallbackThreshold  = 710;
        enemy.m_LifeCallbackSub        = SUB_SAKUYA_MISDIRECTION;
        enemy.m_DeathCallbackSub       = SUB_SAKUYA_MIDBOSS_DEATH;
        enemy.m_CanTakeDamage          = true;
        ctx.SetTimeStopped(false);
        ctx.BulletCancelIntoPointItems();
    }
    if (t < 30) return;

    const int loopT = (t - 30) % 211;
    const auto pos  = ShootPos(enemy);
    if (loopT == 0) {
        SpawnAimedCircleStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 18, 2,
                              2.5f, 1.2f, 0.0f, true);
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
    if (loopT == 90) {
        SpawnAimedCircleStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkRed, 18, 2, 2.4f,
                              1.2f, 0.15707964f, true);
    }
}

void RunSakuyaNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t, int phase) {
    if (t == 0) {
        if (phase == 0) {
            StartSakuyaPhase(enemy, ctx, "Sakuya Izayoi", 19000, 2, 2700,
                             SUB_SAKUYA_CLOCK_CORPSE, 1400, false);
            ctx.SetTimeStopped(false);
        } else if (phase == 1) {
            StartSakuyaPhase(enemy, ctx, "Sakuya Izayoi", 17000, 1, 2700,
                             SUB_SAKUYA_LUNA_CLOCK, 1400, false);
            ctx.SetTimeStopped(false);
            ScriptUtil::DropPowerItems(enemy, ctx, 12);
        } else {
            StartSakuyaPhase(enemy, ctx, "Sakuya Izayoi", 16000, 0, 2700,
                             SUB_SAKUYA_FINAL_SPELL, 1700, false);
            ctx.SetTimeStopped(false);
            ScriptUtil::DropPowerItems(enemy, ctx, 12);
        }
        enemy.m_CanTakeDamage = true;
    }

    const int loopLen = phase == 2 ? 230 : (phase == 1 ? 260 : 300);
    const int loopT   = (t - (phase == 0 ? 100 : 60) + loopLen) % loopLen;
    const auto pos    = ShootPos(enemy);

    if (phase == 2) {
        if (loopT == 0) {
            for (int i = 0; i < 8; i++) {
                SpawnDownDaggerRing(ctx, pos, EBulletColor::DarkPurple, 5, 2, 1.8f, 1.2f,
                                    RandAngle() + i * 0.09817477f);
            }
            ScriptUtil::StartRandomMove(enemy, ctx, 2.0f, 50);
        }
        if (loopT == 120) {
            for (int i = 0; i < 8; i++) {
                SpawnDownDaggerRing(ctx, pos, EBulletColor::DarkRed, 6, 2, 2.0f, 1.2f,
                                    RandAngle() - i * 0.09817477f);
            }
            ScriptUtil::StartRandomMove(enemy, ctx, 2.0f, 50);
        }
        return;
    }

    if (loopT == 0) {
        if (phase == 1) {
            SpawnSecondNonspellHelpers(enemy, ctx);
        } else {
            SpawnFirstNonspellHelpers(enemy, ctx);
        }
        SpawnAimedCircleStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 16, 2,
                              2.5f, 1.2f, 0.15707964f, true);
        ScriptUtil::StartRandomMove(enemy, ctx, phase == 1 ? 1.5f : 1.7f, 60);
    }
    if (loopT == 60 || loopT == 120) {
        ScriptUtil::StartRandomMove(enemy, ctx, phase == 1 ? 1.5f : 1.7f, 60);
    }
    if (loopT == (phase == 1 ? 180 : 220)) {
        SpawnDownDaggerRing(ctx, pos, phase == 1 ? EBulletColor::DarkPurple : EBulletColor::Red,
                            phase == 1 ? 24 : 12, phase == 1 ? 1 : 8, phase == 1 ? 2.0f : 3.0f,
                            phase == 1 ? 1.2f : 1.2f, 0.049087387f);
    }
}

void RunClockCorpse(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartSakuyaPhase(enemy, ctx, "Illusion Existence \"Clock Corpse\"", -1, 2, 1800,
                         SUB_SAKUYA_SECOND_NONSPELL, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 368;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 24) {
        SpawnRandomCircle(pos, ctx, EBulletType::Rice, EBulletColor::Blue, 96, 2.8f, 4.0f);
    }
    if (loopT == 94) {
        ctx.SetTimeStopped(true);
        enemy.m_CanTakeDamage = false;
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
    if (loopT >= 94 && loopT < 184 && (loopT - 94) % 9 == 0) {
        SpawnTimeStopKnifeLattice(enemy, ctx, (loopT - 94) / 9);
    }
    if (loopT == 218) {
        ctx.SetTimeStopped(false);
        enemy.m_CanTakeDamage = true;
    }
}

void RunLunaClock(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartSakuyaPhase(enemy, ctx, "Illusion Image \"Luna Clock\"", -1, 1, 1800,
                         SUB_SAKUYA_FINAL_NONSPELL, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 389;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 24) {
        SpawnDownDaggerRing(ctx, pos, EBulletColor::Blue, 32, 4, 3.0f, 1.2f, 0.0f);
    }
    if (loopT == 74) {
        ctx.SetTimeStopped(true);
        enemy.m_CanTakeDamage = false;
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT >= 74 && loopT < 134 && (loopT - 74) % 9 == 0) {
        SpawnTimeStopKnifeLattice(enemy, ctx, (loopT - 74) / 9);
    }
    if (loopT >= 134 && loopT < 164 && (loopT - 134) % 5 == 0) {
        ctx.RedirectTimeStopBullets();
    }
    if (loopT == 169) {
        ctx.SetTimeStopped(false);
        enemy.m_CanTakeDamage = true;
    }
}

void RunFinalSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartSakuyaPhase(enemy, ctx, "Maid Secret Skill \"Manipulating Doll\"", -1, 0, 1800,
                         SUB_SAKUYA_DEATH, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 274;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0) {
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Dagger,
                                  EBulletColor::DarkPurple, 4, 2.0f, 0.0f, 0.15707964f, false,
                                  true);
        ctx.bullets.SpawnFanAimed(pos, ctx.playerPos, EBulletType::Dagger, EBulletColor::DarkRed,
                                  4, 2.8f, 0.0f, 0.07853982f, false, true);
    }
    if (loopT == 48) {
        ctx.SetTimeStopped(true);
        enemy.m_CanTakeDamage = false;
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT >= 68 && loopT < 96 && (loopT - 68) % 4 == 0) {
        ctx.RedirectTimeStopBullets();
    }
    if (loopT == 126) {
        ctx.SetTimeStopped(false);
        enemy.m_CanTakeDamage = true;
    }
}

}  // namespace

void Stage5Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG5ENM.folder, Anm::STG5ENM.txt, Anm::STG5ENM.offset);
    anm.LoadAnm(Anm::STG5ENM2.folder, Anm::STG5ENM2.txt, Anm::STG5ENM2.offset);
}

void Stage5Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG5ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            if (enemy.m_SubId == 2) {
                enemy.m_DeathCallbackSub = SUB_MAID_DROP_4;
            } else if (enemy.m_SubId >= 3 && enemy.m_SubId <= 5) {
                enemy.m_DeathCallbackSub = SUB_MAID_DROP_6;
            }
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;
        case 1:
        case 9:
            ctx.anm.SetScript(enemy.m_Vm, offset + 10, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;
        case 6:
        case 10:
            ctx.anm.SetScript(enemy.m_Vm, offset + 11, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;
        case 11:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;
        case SUB_MAID_DROP_4:
        case SUB_MAID_DROP_6:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            break;

        case SUB_SAKUYA_HELPER_1:
        case SUB_SAKUYA_HELPER_2:
        case SUB_SAKUYA_HELPER_3:
        case SUB_SAKUYA_HELPER_4:
        case SUB_SAKUYA_HELPER_5:
        case SUB_SAKUYA_HELPER_6:
        case SUB_SAKUYA_HELPER_7:
        case SUB_SAKUYA_HELPER_8:
        case SUB_SAKUYA_KUNAI_HELPER_1:
        case SUB_SAKUYA_KUNAI_HELPER_2:
        case SUB_SAKUYA_KUNAI_HELPER_3:
        case SUB_SAKUYA_KUNAI_HELPER_4:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            InitSakuyaHelper(enemy, GetSakuyaHelperPattern(enemy.m_SubId));
            break;

        case SUB_SAKUYA_HELPER_DEATH:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            enemy.m_ItemDropCount = 0;
            break;

        case SUB_SAKUYA_MIDBOSS_ENTRY:
        case SUB_SAKUYA_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, Anm::STG5ENM2.offset + 128, Anm::STG5ENM2.offset);
            enemy.m_Pos            = Util::GameFieldToScreen(-32.0f, 128.0f);
            enemy.m_HitboxSize     = {40.0f, 56.0f};
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = enemy.m_SubId == SUB_SAKUYA_ENTRY;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Sakuya Izayoi";
            enemy.m_BossLifeCount  = enemy.m_SubId == SUB_SAKUYA_ENTRY ? 2 : 0;
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 132.0f);
            SetSakuyaBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void Stage5Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;

    switch (enemy.m_SubId) {
        case 0:
            RunTopMaid(enemy, ctx, t, EBulletColor::Blue, false, false);
            break;
        case 2:
            RunTopMaid(enemy, ctx, t, EBulletColor::Blue, true, false);
            break;
        case 3:
            RunTopMaid(enemy, ctx, t, EBulletColor::Blue, false, true);
            break;
        case 4:
            RunTopMaid(enemy, ctx, t, EBulletColor::Red, false, true);
            break;
        case 5:
            RunTopMaid(enemy, ctx, t, EBulletColor::Green, false, true);
            break;
        case 6:
            RunRingMaid(enemy, ctx, t);
            break;
        case 1:
        case 9:
        case 10:
        case 11:
            RunSideMaid(enemy, ctx, t, enemy.m_SubId);
            break;
        case SUB_MAID_DROP_4:
            if (t == 0) {
                ScriptUtil::DropPowerItems(enemy, ctx, 4);
                enemy.m_Alive = false;
            }
            break;
        case SUB_MAID_DROP_6:
            if (t == 0) {
                ScriptUtil::DropPowerItems(enemy, ctx, 6);
                enemy.m_Alive = false;
            }
            break;
        case SUB_SAKUYA_HELPER_1:
        case SUB_SAKUYA_HELPER_2:
        case SUB_SAKUYA_HELPER_3:
        case SUB_SAKUYA_HELPER_4:
        case SUB_SAKUYA_HELPER_5:
        case SUB_SAKUYA_HELPER_6:
        case SUB_SAKUYA_HELPER_7:
        case SUB_SAKUYA_HELPER_8:
        case SUB_SAKUYA_KUNAI_HELPER_1:
        case SUB_SAKUYA_KUNAI_HELPER_2:
        case SUB_SAKUYA_KUNAI_HELPER_3:
        case SUB_SAKUYA_KUNAI_HELPER_4:
            RunSakuyaHelper(enemy, ctx, t);
            break;
        case SUB_SAKUYA_HELPER_DEATH:
            if (t >= 60) enemy.m_Alive = false;
            break;

        case SUB_SAKUYA_MIDBOSS_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_SAKUYA_MIDBOSS_MAIN);
            break;
        case SUB_SAKUYA_MIDBOSS_MAIN:
            RunMidbossMain(enemy, ctx, t);
            break;
        case SUB_SAKUYA_MISDIRECTION:
            RunMisdirection(enemy, ctx, t);
            break;
        case SUB_SAKUYA_MIDBOSS_DEATH:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Life);
                ScriptUtil::DropPowerItems(enemy, ctx, 10);
                ctx.BulletCancelIntoPointItems();
                enemy.m_Alive = false;
            }
            break;
        case SUB_SAKUYA_MIDBOSS_EXIT:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ctx.BulletCancelIntoPointItems();
                enemy.m_Alive = false;
            }
            break;

        case SUB_SAKUYA_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_SAKUYA_FIRST_NONSPELL);
            break;
        case SUB_SAKUYA_FIRST_NONSPELL:
            RunSakuyaNonSpell(enemy, ctx, t, 0);
            break;
        case SUB_SAKUYA_CLOCK_CORPSE:
            RunClockCorpse(enemy, ctx, t);
            break;
        case SUB_SAKUYA_SECOND_NONSPELL:
            RunSakuyaNonSpell(enemy, ctx, t, 1);
            break;
        case SUB_SAKUYA_LUNA_CLOCK:
            RunLunaClock(enemy, ctx, t);
            break;
        case SUB_SAKUYA_FINAL_NONSPELL:
            RunSakuyaNonSpell(enemy, ctx, t, 2);
            break;
        case SUB_SAKUYA_FINAL_SPELL:
            RunFinalSpell(enemy, ctx, t);
            break;
        case SUB_SAKUYA_DEATH:
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
