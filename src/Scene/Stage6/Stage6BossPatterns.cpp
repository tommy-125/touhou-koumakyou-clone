#include <algorithm>
#include <array>
#include <initializer_list>

#include "Anm/AnmDefs.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Scene/Stage6/Stage6PatternCommon.hpp"
#include "Util/Math.hpp"

namespace Stage6Detail {
using EnemyPatternUtil::AimAngle;
using EnemyPatternUtil::RandAngle;
using EnemyPatternUtil::RankedLowSpeed;
using EnemyPatternUtil::RankedSpeed;
using EnemyPatternUtil::SpawnAcceleratingRing;
using EnemyPatternUtil::SpawnAimedStack;
using EnemyPatternUtil::SpawnCircleStackAbs;
using EnemyPatternUtil::SpawnFanAbs;
using EnemyPatternUtil::SpawnRandomArc;
void SpawnVampireBurst(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle, bool dense) {
    ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::Gray, 1, 6.2f, baseAngle);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, dense ? 5 : 3, 4.0f, 6.0f,
                   baseAngle - 0.18f, baseAngle + 0.18f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, dense ? 10 : 6, 1.0f, 3.5f,
                   baseAngle - 0.65f, baseAngle + 0.65f);
}

void SpawnRemiliaBloodBurst(EnemySubCtx& ctx, glm::vec2 pos, float angle, float rankSpeed = 0.0f) {
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
                RankedSpeed(1.8f, rankSpeed), RankedSpeed(1.8f, rankSpeed), blueBase, 0.09817477f);
    SpawnFanAbs(ctx, pos, EBulletType::Bubble, EBulletColor::Gray, 4, 1,
                RankedSpeed(3.5f, rankSpeed), RankedSpeed(3.5f, rankSpeed), bubbleBase, 1.0471976f);
    SpawnFanAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 2, 1,
                RankedSpeed(2.5f, rankSpeed), RankedSpeed(2.5f, rankSpeed), purpleBase, 0.7853982f);
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
        const float              startAngle = (outer == 0 ? -PI : -7.0f * PI / 8.0f) + randomAngle;
        const float              angleDiff  = outer == 0 ? PI / 4.0f : -PI / 4.0f;
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

void StartRemiliaPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                       int lifeCount, int timerFrames, int nextSub, int lifeThreshold, bool spell,
                       int deathSub = -1) {
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

bool BeginRemiliaSpellAt(Enemy& enemy, EnemySubCtx& ctx, int t, const char* title, int lifeCount,
                         int timerFrames, int nextSub, glm::vec2 target, int warmup = 120) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, title, -1, lifeCount, timerFrames, nextSub, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, target.x, target.y, warmup);
    }
    if (t == warmup) enemy.m_CanTakeDamage = true;
    return t >= warmup;
}

void RunRemiliaNonSpell1(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 13000, 4, 2700, SUB_REMILIA_STAR, 1200,
                          false, SUB_REMILIA_NONSPELL_2);
    }
    if (t < 100) return;
    const int loopT = (t - 100) % 720;
    if (loopT == 0) enemy.m_LockedShotAngle = RandAngle();
    if (loopT < 256 && loopT % 8 == 0) {
        const auto pos = ShootPos(enemy);
        SpawnRemiliaOpeningPacket(ctx, pos, loopT / 8, enemy.m_LockedShotAngle, REMILIA_RANK_SPEED);
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
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 15500, 3, 2700, SUB_REMILIA_SCARLET, 1600,
                          false, SUB_REMILIA_NONSPELL_3);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
    }
    if (t < 60) return;
    const int  loopT = (t - 60) % 1120;
    const auto pos   = ShootPos(enemy);
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
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 11000, 2, 2700, SUB_REMILIA_VLAD, 1600,
                          false, SUB_REMILIA_NONSPELL_4);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
    }
    if (t < 60) return;
    const int  loopT = (t - 60) % 478;
    const auto pos   = ShootPos(enemy);
    if (loopT == 0) enemy.m_LockedShotAngle = RandFloat(-PI, PI);
    if (loopT < 120 && loopT % 2 == 0) {
        const float base = enemy.m_LockedShotAngle + static_cast<float>(loopT / 2) * 0.09817477f;
        SpawnCircleStackAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkRed, 4, 2, 3.0f, 1.2f,
                            base, 0.0f, true);
    }
    if (loopT == 120) {
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
    if (loopT == 248) enemy.m_SecondaryShotAngle = RandFloat(-PI, PI);
    if (loopT >= 248 && loopT < 368 && loopT % 2 == 0) {
        const float base =
            enemy.m_SecondaryShotAngle - static_cast<float>((loopT - 248) / 2) * 0.09817477f;
        SpawnCircleStackAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 4, 2, 3.0f,
                            1.2f, base, 0.0f, true);
    }
}

void RunRemiliaNonSpell4(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 13000, 1, 3600, SUB_REMILIA_SHOOT, 1300,
                          false, SUB_REMILIA_RED_MAGIC);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
        SetRemiliaPoses(enemy);
    }
    if (t < 60) return;
    const int  loopT = (t - 60) % 430;
    const auto pos   = ShootPos(enemy);
    const int  cycle = ((t - 60) / 430) % 3;
    if (loopT == 100) {
        ScriptUtil::DisableBossPoses(enemy);
        ctx.anm.SetScript(enemy.m_Vm, Anm::STG6ENM2.offset + 165, Anm::STG6ENM2.offset);
        enemy.m_CanTakeDamage = false;
    }
    if (loopT >= 100 && loopT < 130) {
        const glm::vec2 field = Util::ScreenToGameField(enemy.m_Pos);
        ctx.SpawnEnemy(SUB_REMILIA_AFTERIMAGE, field.x, field.y, 100, 0, false, -99);
        ctx.SpawnEnemy(SUB_REMILIA_AFTERIMAGE, field.x, field.y, 100, 0, false, -99);
    }
    if (loopT >= 130 && loopT < 340) {
        if (cycle == 0 && (loopT - 130) % 8 == 0) {
            SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Red, 11, 1.0f, 2.0f, -PI, PI);
        } else if (cycle == 1 && (loopT - 130) % 16 == 0) {
            SpawnAimedStack(ctx, pos, EBulletType::Bubble, EBulletColor::Gray, 9, 1, 5.0f, 5.0f,
                            0.34906584f, 0.0f);
        } else if (cycle == 2 && (loopT - 130) % 10 == 0) {
            SpawnAimedStack(ctx, pos, EBulletType::Fireball, EBulletColor::DarkRed, 12, 2, 5.0f,
                            2.0f, 0.0f, 0.0f, true);
        }
    }
    if (loopT >= 130 && loopT <= 310 && (loopT - 130) % 30 == 0) {
        ScriptUtil::StartRandomMove(enemy, ctx, ((loopT - 130) / 30) % 2 == 0 ? 4.0f : 7.0f, 30);
    }
    if (loopT == 340) {
        SetRemiliaPoses(enemy);
        enemy.m_CanTakeDamage = true;
    }
}

void RunStarOfDavid(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (!BeginRemiliaSpellAt(enemy, ctx, t, "Heaven's Punishment \"Star of David\"", 4, 2400,
                             SUB_REMILIA_NONSPELL_2, {192.0f, 112.0f})) {
        return;
    }
    const int loopT = (t - 120) % 184;
    if (loopT == 64) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        SpawnStage6StarLasers(ctx, pos, 0);
        const float base = RandAngle();
        SpawnCircleStackAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 5, 1, 0.2f,
                            1.0f, base, 0.0f, false, 0.01f, 120);
        SpawnCircleStackAbs(ctx, pos, EBulletType::BigBall, EBulletColor::DarkPurple, 3, 1, 0.2f,
                            1.0f, base + PI / 5.0f, 0.0f, false, 0.01f, 120);
    }
    if (loopT == 124) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
}

void RunScarletNetherworld(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (!BeginRemiliaSpellAt(enemy, ctx, t, "Nether Sign \"Scarlet Netherworld\"", 3, 2400,
                             SUB_REMILIA_NONSPELL_3, {192.0f, 144.0f})) {
        return;
    }
    const int loopT = (t - 120) % 231;
    if (loopT < 126) {
        const auto  pos   = ShootPos(enemy, {0.0f, 0.0f});
        const int   phase = loopT % 21;
        const float base  = RandAngle() + static_cast<float>(loopT / 21) * 0.3926991f;
        if (phase == 0) {
            SpawnAcceleratingRing(ctx, pos, EBulletType::Rice, EBulletColor::Red, 24, 1.8f, base,
                                  0.0f, 128, true, 0.024543693f);
        } else if (phase == 4) {
            SpawnAcceleratingRing(ctx, pos, EBulletType::Rice, EBulletColor::Red, 24, 1.8f,
                                  base + 0.09817477f, 0.0f, 128, true, -0.024543693f);
        } else if (phase == 8 || phase == 12) {
            SpawnAcceleratingRing(ctx, pos, EBulletType::Shard, EBulletColor::Red, 16, 2.2f,
                                  base + static_cast<float>(phase) * 0.024543693f, 0.02f, 240, true,
                                  0.0f, true, Util::HALF_PI);
        }
    }
    if (loopT == 141) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

void RunVlad(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const bool active = BeginRemiliaSpellAt(enemy, ctx, t, "Curse \"Curse of Vlad Tepes\"", 2, 2400,
                                            SUB_REMILIA_NONSPELL_4, {192.0f, 144.0f});
    if (t == 0 || t == 120) {
        enemy.m_ScriptState   = 0;
        enemy.m_ScriptTimer   = 0;
        enemy.m_ScriptCounter = 0;
    }
    if (!active) return;
    const auto pos = ShootPos(enemy, {0.0f, 0.0f});
    if (enemy.m_ScriptState == 0) {
        const int   count = std::min(18, 13 + enemy.m_ScriptCounter);
        const float base  = 0.0f;
        ctx.bullets.SpawnCircle(pos, EBulletType::Dagger, EBulletColor::DarkPurple, count, 3.0f,
                                base, false, 0.0f, 0, true, {0.0f, 0.0f}, 0, 0, 1.0f, {}, false, 0,
                                -1.0f,
                                enemy.m_ScriptCounter % 2 == 0 ? 0.019634955f : -0.019634955f, 108);
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
    if (!BeginRemiliaSpellAt(enemy, ctx, t, "Scarlet Sign \"Scarlet Shoot\"", 1, 1800,
                             SUB_REMILIA_RED_MAGIC, {192.0f, 112.0f})) {
        return;
    }
    const int  loopT = (t - 120) % 544;
    const auto pos   = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0 || loopT == 60 || loopT == 270) {
        SpawnScarletShootVolley(ctx, pos, {0.0f, 0.7853982f, -0.7853982f, 1.5707964f, -1.5707964f},
                                false);
    }
    if (loopT == 150) {
        SpawnScarletShootVolley(ctx, pos, {0.0f, 0.07853982f, -0.07853982f}, false);
    }
    if (loopT == 300) {
        SpawnScarletShootVolley(ctx, pos, {0.0f, 1.0471976f, -1.0471976f, 2.0943952f, -2.0943952f},
                                false);
    }
    if (loopT == 60 || loopT == 180) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
}

void RunRedMagic(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "\"Red Magic\"", 4700, 0, 7200, SUB_REMILIA_DEATH, -1, true);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 128.0f, 120);
    }
    if (t == 180) enemy.m_CanTakeDamage = true;
    if (t < 180) return;
    const int  loopT = (t - 180) % 1056;
    const auto pos   = ShootPos(enemy, {0.0f, 0.0f});
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
        SpawnCircleStackAbs(ctx, pos, EBulletType::Bubble, EBulletColor::DarkRed, 8, 2, 4.0f, 1.0f,
                            RandAngle(), -0.3926991f);
    }
    if (loopT >= 786 && loopT < 956 && (loopT - 786) % 17 == 0) {
        ctx.bullets.Stage6CreateSeedsFromLargeBullets();
    }
    if (loopT == 956) {
        ctx.bullets.Stage6ReleaseStoppedSeeds(pos, true);
    }
}
}  // namespace Stage6Detail
