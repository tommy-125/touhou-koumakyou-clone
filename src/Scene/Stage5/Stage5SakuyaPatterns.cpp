#include "Stage5PatternCommon.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Util/Math.hpp"

namespace Stage5Detail {
using EnemyPatternUtil::EclStackSpeed;
using EnemyPatternUtil::SpawnAimedCircleStack;
using EnemyPatternUtil::SpawnCircleStackEcl;
using EnemyPatternUtil::SpawnFanAbsolute;
using EnemyPatternUtil::SpawnRandomCircle;
using EnemyPatternUtil::RandAngle;
void SpawnDownDaggerRing(EnemySubCtx& ctx, glm::vec2 pos, EBulletColor color, int count,
                         int stacks, float speed1, float speed2, float angleOffset) {
    const glm::vec2 downTarget = pos + glm::vec2{0.0f, 1.0f};
    for (int s = 0; s < stacks; s++) {
        const float speed = EclStackSpeed(s, stacks, speed1, speed2);
        ctx.bullets.SpawnCircleAimed(pos, downTarget, EBulletType::Dagger, color, count, speed,
                                     angleOffset, false, 0.0f, {}, true);
    }
}

void SpawnSakuyaHelper(Enemy& enemy, EnemySubCtx& ctx, int subId) {
    const glm::vec2 field = Util::ScreenToGameField(enemy.m_Pos);
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

void SetBossFieldPosition(Enemy& enemy, float x, float y) {
    enemy.m_Pos       = Util::GameFieldToScreen(x, y);
    enemy.m_IsLerping = false;
    enemy.m_Speed     = 0.0f;
}

bool BeginSakuyaSpellAt(Enemy& enemy, EnemySubCtx& ctx, int t, const char* title, int lifeCount,
                        int nextSub, glm::vec2 target, int timerFrames = 1800,
                        int warmup = 120) {
    if (t == 0) {
        StartSakuyaPhase(enemy, ctx, title, -1, lifeCount, timerFrames, nextSub, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(false);
        ctx.StartLerpTo(enemy, target.x, target.y, warmup);
    }
    if (t == warmup) enemy.m_CanTakeDamage = true;
    return t >= warmup;
}

void SpawnMidbossDaggerSweep(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle) {
    SpawnFanAbsolute(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f,
                     3.2f, baseAngle, 0.28559932f, true);
}

void SpawnMidbossKunaiSweep(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle) {
    SpawnFanAbsolute(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 4, 2, 2.0f, 1.0f,
                     baseAngle, 0.044879895f, true);
}

void SpawnMisdirectionOpeningKnives(EnemySubCtx& ctx, glm::vec2 pos) {
    SpawnAimedCircleStack(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 24, 3, 3.0f,
                          1.2f, 0.0f, true);
}

void SpawnMisdirectionDaggerVolley(EnemySubCtx& ctx, glm::vec2 pos) {
    ctx.bullets.SpawnFanStack(pos, ctx.playerPos, EBulletType::Dagger, EBulletColor::DarkPurple,
                              11, 4, 4.5f, 1.2f, 0.0f, 0.20943952f, true);
}

void SpawnSakuyaDisappearRice(EnemySubCtx& ctx, glm::vec2 pos) {
    SpawnRandomCircle(ctx, pos, EBulletType::Rice, EBulletColor::Blue, 128,
                      RankedSpeed2(1.2f), RankedSpeed1(4.0f));
}

void SpawnSakuyaLunaClockRing(EnemySubCtx& ctx, glm::vec2 pos) {
    SpawnCircleStackEcl(ctx, pos, EBulletType::Rice, EBulletColor::Blue, 32, 4,
                        RankedSpeed1(3.0f), RankedSpeed2(1.2f), 0.0f, true);
}

void SpawnFinalNonspellDaggerBurst(EnemySubCtx& ctx, glm::vec2 pos, EBulletColor color,
                                   int step, float turn, float baseAngle) {
    const int   count = color == EBulletColor::DarkRed ? 6 : 5;
    const float base  = baseAngle + turn * static_cast<float>(step) * 0.09817477f;
    SpawnCircleStackEcl(ctx, pos, EBulletType::Dagger, color, count, 2,
                        RankedSpeed1(1.8f), RankedSpeed2(1.2f), base, true, true, 1,
                        0.015707964f);
}

void SpawnFirstNonspellRedSweep(EnemySubCtx& ctx, glm::vec2 pos, int shot) {
    const float spread = 0.049087387f + static_cast<float>(shot) * 0.31415927f;
    SpawnFanAbsolute(ctx, pos, EBulletType::Dagger, EBulletColor::Red, 2, 8,
                     RankedSpeed1(4.0f), RankedSpeed2(1.2f), Util::HALF_PI, spread, true);
}

void SpawnSakuyaNonspellPurpleCircle(EnemySubCtx& ctx, glm::vec2 pos, int phase) {
    const float speed = phase == 0 ? 2.2f : 2.0f;
    SpawnAimedCircleStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 16, 1,
                          RankedSpeed1(speed), RankedSpeed2(1.0f), 0.15707964f, true);
}

void SpawnManipulatingDollPurple(EnemySubCtx& ctx, glm::vec2 pos) {
    const float aim = std::atan2(ctx.playerPos.y - pos.y, ctx.playerPos.x - pos.x);
    SpawnFanAbsolute(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 4, 2,
                     RankedSpeed1(2.0f), RankedSpeed2(1.0f), aim, 0.15707964f, true,
                     true, 1);
}

void SpawnManipulatingDollRed(EnemySubCtx& ctx, glm::vec2 pos) {
    const float aim = std::atan2(ctx.playerPos.y - pos.y, ctx.playerPos.x - pos.x);
    SpawnFanAbsolute(ctx, pos, EBulletType::Dagger, EBulletColor::DarkRed, 4, 3,
                     RankedSpeed1(2.8f), RankedSpeed2(1.0f), aim, 0.07853982f, true);
}

void SpawnTimeStopKnifeField(Enemy& enemy, EnemySubCtx& ctx, int patternPosition) {
    glm::vec2 toPlayer = ctx.playerPos - enemy.m_Pos;
    const float dist = std::max(1.0f, std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y));
    glm::vec2 line = toPlayer / dist;
    line *= (patternPosition & 1) ? -256.0f : 256.0f;

    glm::vec2 offset = toPlayer * (0.5f - static_cast<float>(patternPosition) * 0.5f / 9.0f) + line;
    line = -line;

    constexpr float ROT_START = 0.7853982f;
    constexpr float ROT_STEP  = -0.17453292f;
    const float     c0        = std::cos(ROT_START);
    const float     s0        = std::sin(ROT_START);
    glm::vec2       ray       = {line.x * c0 + line.y * s0, -line.x * s0 + line.y * c0};

    for (int i = 0; i < 9; ++i) {
        const float c = std::cos(ROT_STEP);
        const float s = std::sin(ROT_STEP);
        ray           = {ray.x * c + ray.y * s, -ray.x * s + ray.y * c};
        const glm::vec2 bulletPos = enemy.m_Pos + offset + ray;
        float angle = std::atan2(ctx.playerPos.y - bulletPos.y, ctx.playerPos.x - bulletPos.x);
        if (patternPosition & 1) {
            angle += -ROT_START + static_cast<float>(i) * 0.17453292f;
        }
        ctx.bullets.SpawnCircle(bulletPos, EBulletType::Dagger, EBulletColor::DarkPurple, 1,
                                2.0f, angle, false, 0.0f, 0, true);
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
            return {0.7853982f, EBulletType::Kunai, EBulletColor::Red, 6, 1, 1.6f, 1.2f,
                    0.015707964f, 0.62831855f, 14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_2:
            return {0.0f, EBulletType::Kunai, EBulletColor::Blue, 6, 1, 1.6f, 1.2f,
                    0.015707964f, -0.19634955f, 14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_3:
            return {2.3561945f, EBulletType::Kunai, EBulletColor::Blue, 6, 1, 1.6f, 1.2f,
                    0.015707964f, -0.62831855f, 14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_4:
            return {PI, EBulletType::Kunai, EBulletColor::Red, 6, 1, 1.6f, 1.2f,
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

void InitSakuyaHelper(Enemy& enemy) {
    InitSakuyaHelper(enemy, GetSakuyaHelperPattern(enemy.m_SubId));
}

void RunSakuyaHelper(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const HelperPattern pattern = GetSakuyaHelperPattern(enemy.m_SubId);
    const int fireStart = 50;
    const int fireEnd   = fireStart + pattern.interval * pattern.shots;
    if (t >= fireStart && t < fireEnd && (t - fireStart) % pattern.interval == 0) {
        const int shot = (t - fireStart) / pattern.interval;
        SpawnFanAbsolute(ctx, enemy.m_Pos, pattern.type, pattern.color, pattern.ways,
                         pattern.stacks, RankedSpeed1(pattern.speed1), RankedSpeed2(pattern.speed2),
                         enemy.m_LockedShotAngle + pattern.angleInc * static_cast<float>(shot),
                         pattern.spread, pattern.type == EBulletType::Kunai);
    }
    if (t >= fireEnd + 60) enemy.m_Alive = false;
}

void RunMisdirection(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const bool active = BeginSakuyaSpellAt(enemy, ctx, t, "Illusion Sign \"Misdirection\"", 0,
                                           SUB_SAKUYA_MIDBOSS_EXIT, {192.0f, 112.0f});
    if (t == 0) {
        enemy.m_DeathCallbackSub = SUB_SAKUYA_MIDBOSS_DEATH;
        enemy.m_LockedShotAngle = (std::rand() & 1) ? 1.0f : 0.0f;
    }
    if (!active) return;

    const int loopT = (t - 120) % 248;
    const int cycle = (t - 120) / 248;
    const bool leftFirst = (((cycle + static_cast<int>(enemy.m_LockedShotAngle)) & 1) == 0);

    if (loopT == 0) {
        ctx.StartLerpTo(enemy, leftFirst ? 96.0f : 288.0f, 144.0f, 40);
    }
    if (loopT < 30 && loopT % 6 == 0) {
        SpawnMisdirectionOpeningKnives(ctx, ShootPos(enemy, {0.0f, 0.0f}));
    }
    if (loopT == 60) {
        SetBossFieldPosition(enemy, leftFirst ? 288.0f : 96.0f, 96.0f);
    }
    if (loopT == 80 || loopT == 98 || loopT == 116) {
        SpawnMisdirectionDaggerVolley(ctx, ShootPos(enemy, {0.0f, 0.0f}));
    }
    if (loopT == 218) SetBossFieldPosition(enemy, 192.0f, 144.0f);
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

    const int loopT = (t - 30) % 281;
    const auto pos  = ShootPos(enemy, {0.0f, -12.0f});
    if (loopT < 80 && loopT % 8 == 0) {
        SpawnMidbossDaggerSweep(ctx, pos, (static_cast<float>(loopT) / 8.0f) * 0.31415927f);
    }
    if (loopT == 80) {
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
    if (loopT >= 80 && loopT < 110 && (loopT - 80) % 3 == 0) {
        SpawnMidbossKunaiSweep(ctx, pos,
                               PI - (static_cast<float>(loopT - 80) / 3.0f) * 0.31415927f);
    }
    if (loopT >= 170 && loopT < 250 && (loopT - 170) % 8 == 0) {
        SpawnMidbossDaggerSweep(ctx, pos,
                                PI - (static_cast<float>(loopT - 170) / 8.0f) * 0.31415927f);
    }
    if (loopT >= 250 && loopT < 280 && (loopT - 250) % 3 == 0) {
        SpawnMidbossKunaiSweep(ctx, pos,
                               (static_cast<float>(loopT - 250) / 3.0f) * 0.31415927f);
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
        if (loopT == 0) enemy.m_LockedShotAngle = RandAngle();
        if (loopT < 40 && loopT % 5 == 0) {
            SpawnFinalNonspellDaggerBurst(ctx, pos, EBulletColor::DarkPurple, loopT / 5,
                                          1.0f, enemy.m_LockedShotAngle);
        }
        if (loopT == 60) {
            ScriptUtil::StartRandomMove(enemy, ctx, 2.0f, 50);
        }
        if (loopT == 120) enemy.m_SecondaryShotAngle = RandAngle();
        if (loopT >= 120 && loopT < 160 && (loopT - 120) % 5 == 0) {
            SpawnFinalNonspellDaggerBurst(ctx, pos, EBulletColor::DarkRed, (loopT - 120) / 5,
                                          -1.0f, enemy.m_SecondaryShotAngle);
        }
        if (loopT == 160) {
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
    }
    if (loopT >= 60 && loopT < 180 && (loopT - 60) % 50 == 0) {
        SpawnSakuyaNonspellPurpleCircle(ctx, pos, phase);
    }
    if (loopT == 60) {
        ScriptUtil::StartRandomMove(enemy, ctx, phase == 1 ? 1.5f : 1.7f, 60);
    }
    if (loopT == 120 || loopT == 180) {
        ScriptUtil::StartRandomMove(enemy, ctx, phase == 1 ? 1.5f : 1.7f, 60);
    }
    if (phase == 0) {
        const int sweepT = loopT - 50;
        if (t >= 400 && sweepT >= 0 && sweepT < 96 && sweepT % 8 == 0) {
            SpawnFirstNonspellRedSweep(ctx, pos, sweepT / 8);
        }
    } else if (t >= 320 && loopT == 20) {
        SpawnDownDaggerRing(ctx, pos, EBulletColor::DarkPurple, 24, 1, 2.0f, 1.2f,
                            0.049087387f);
    }
}

void RunClockCorpse(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (!BeginSakuyaSpellAt(enemy, ctx, t, "Illusion Existence \"Clock Corpse\"", 2,
                            SUB_SAKUYA_SECOND_NONSPELL, {192.0f, 112.0f})) {
        return;
    }

    const int loopT = (t - 120) % 368;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 144) {
        SpawnSakuyaDisappearRice(ctx, pos);
    }
    if (loopT == 214) {
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(true);
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
    if (loopT >= 214 && loopT < 304 && (loopT - 214) % 9 == 0) {
        SpawnTimeStopKnifeField(enemy, ctx, (loopT - 214) / 9);
    }
    if (loopT == 338) {
        ctx.SetTimeStopped(false);
        enemy.m_CanTakeDamage = true;
    }
}

void RunLunaClock(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (!BeginSakuyaSpellAt(enemy, ctx, t, "Illusion Image \"Luna Clock\"", 1,
                            SUB_SAKUYA_FINAL_NONSPELL, {192.0f, 112.0f})) {
        return;
    }

    const int loopT = (t - 120) % 389;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 144) {
        SpawnSakuyaLunaClockRing(ctx, pos);
    }
    if (loopT == 194) {
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(true);
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
    if (loopT >= 194 && loopT < 254 && (loopT - 194) % 9 == 0) {
        SpawnTimeStopKnifeField(enemy, ctx, (loopT - 194) / 9);
    }
    if (loopT >= 254 && loopT < 284 && (loopT - 254) % 5 == 0) {
        ctx.RedirectTimeStopBullets();
    }
    if (loopT == 289) {
        ctx.SetTimeStopped(false);
        enemy.m_CanTakeDamage = true;
    }
}

void RunFinalSpell(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (!BeginSakuyaSpellAt(enemy, ctx, t, "Maid Secret Skill \"Manipulating Doll\"", 0,
                            SUB_SAKUYA_DEATH, {192.0f, 144.0f})) {
        return;
    }

    const int loopT = (t - 120) % 274;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT >= 0 && loopT < 4) {
        SpawnManipulatingDollPurple(ctx, pos);
    }
    if (loopT >= 25 && loopT < 30) {
        SpawnManipulatingDollRed(ctx, pos);
    }
    if (loopT == 48) {
        enemy.m_CanTakeDamage = false;
        ctx.SetTimeStopped(true);
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
}  // namespace Stage5Detail
