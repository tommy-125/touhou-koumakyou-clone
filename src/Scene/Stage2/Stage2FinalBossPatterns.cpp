#include <algorithm>
#include <cmath>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage2/Stage2Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage2Detail {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;
using EnemyPatternUtil::AimAngle;
using EnemyPatternUtil::SpawnRandomVarianceCircle;
void StartCirnoPhase(Enemy& enemy, const EnemySubCtx& ctx,
                     StageUtil::ConfigId::BossPhaseId phaseId) {
    StageUtil::StartBossPhase(enemy, ctx, phaseId);
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx,
                     StageUtil::ConfigId::BossPhaseId phaseId) {
    StartCirnoPhase(enemy, ctx, phaseId);
    ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
}

void SpawnOneWayStackWithCurve(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type,
                               EBulletColor color, int stacks, float speed1, float speed2,
                               float baseAngle, BulletCurve curve, bool rotateWithAngle) {
    const glm::vec2 angleTarget = pos + glm::vec2{std::cos(baseAngle), std::sin(baseAngle)};
    for (int s = 0; s < stacks; s++) {
        const float ratio = stacks <= 1 ? 0.0f : static_cast<float>(s) / static_cast<float>(stacks);
        const float speed = speed1 - (speed1 - speed2) * ratio;
        ctx.bullets.SpawnCircleAimed(pos, angleTarget, type, color, 1, speed, 0.0f, false, 0.0f,
                                     curve, rotateWithAngle);
    }
}

EBulletColor PerfectFreezeColor(int idx) {
    switch (idx % 5) {
        case 0:
            return EBulletColor::Blue;
        case 1:
            return EBulletColor::Red;
        case 2:
            return EBulletColor::Green;
        case 3:
            return EBulletColor::Yellow;
        default:
            return EBulletColor::Orange;
    }
}

void SpawnAtRandomArea(Enemy& enemy, EnemySubCtx& ctx, float width, int count) {
    const glm::vec2 pos = enemy.m_Pos + glm::vec2{
                                            ScriptUtil::RandFloat(-width * 0.5f, width * 0.5f),
                                            ScriptUtil::RandFloat(-width * 0.375f, width * 0.375f),
                                        };
    SpawnRandomVarianceCircle(ctx, pos, EBulletType::Shard, EBulletColor::Blue, count, 1.2f, 0.8f,
                              true);
}

void InitCirnoEntry(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitBossEntry(
        enemy, ctx, StageUtil::LoadBossEntryConfig(StageUtil::ConfigId::BossEntry::Stage2Cirno));
    enemy.m_DeathCallbackSub = SUB_CIRNO_PHASE2_INIT;
    ScriptUtil::SetBossPoses(enemy, 128, 129, 130, 129, 130);
}

void RunCirnoEntry(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int bossOff = Anm::STG2ENM2.offset;
    if (t == 1) ctx.anm.SetScript(enemy.m_Vm, bossOff + 132, bossOff);
    if (t == 32) {
        ctx.anm.SetScript(enemy.m_Vm, bossOff + 128, bossOff);
        ctx.TransitionToSub(enemy, SUB_CIRNO_NONSPELL_INIT);
    }
}

void RunCirnoFirstNonspellInit(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartCirnoPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage2CirnoFirstNonspell);
    }
    if (t == 20) enemy.m_CanTakeDamage = true;
    if (t == 70) ctx.TransitionToSub(enemy, SUB_CIRNO_NONSPELL_ATTACK_A);
}

void RunCirnoNonspellAttackA(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 40);
    if (t == 41) ctx.anm.SetScript(enemy.m_Vm, Anm::STG2ENM2.offset + 131, Anm::STG2ENM2.offset);
    if (t >= 41 && t < 41 + 3 * 70) {
        const int rel   = t - 41;
        const int cycle = rel / 70;
        const int local = rel % 70;
        if (local <= 10 && local % 2 == 0) {
            const int   fanIdx = local / 2;
            const float spread = 0.05609987f + static_cast<float>(cycle) * 0.049087387f;
            ctx.bullets.SpawnFanAimed(
                ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET), ctx.playerPos, EBulletType::Shard,
                EBulletColor::Blue, fanIdx + 1, 5.0f - static_cast<float>(fanIdx) * 0.5f,
                0.0f, spread, false, true);
        }
    }
    if (t == 251) ctx.TransitionToSub(enemy, SUB_CIRNO_NONSPELL_ATTACK_B);
}

void RunCirnoNonspellAttackB(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const int loopT = t % 100;
    if (loopT == 20 && t < 300) {
        ScriptUtil::StartRandomMove(enemy, ctx, 3.0f, 60);
        ctx.bullets.SpawnCircleAimed(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                     ctx.playerPos, EBulletType::RingBall, EBulletColor::Blue, 16,
                                     2.0f);
    }
    if (loopT == 40 && t < 300) {
        ctx.bullets.SpawnCircleAimed(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                     ctx.playerPos, EBulletType::Shard, EBulletColor::White, 24,
                                     3.0f, 0.0f, false, 0.0f, {}, true);
    }
    if (loopT == 60 && t < 300) {
        ctx.bullets.SpawnCircleAimed(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                     ctx.playerPos, EBulletType::RingBall, EBulletColor::Blue, 14,
                                     3.0f);
    }
    if (t == 300) ctx.TransitionToSub(enemy, SUB_CIRNO_NONSPELL_ATTACK_A);
}

void RunIcicleFall(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) StartSpellPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage2IcicleFall);
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 297;
    const int cycle = (t - 120) / 297;
    if (loopT >= 297 || loopT % 27 != 0) return;

    const int   burst     = loopT / 27;
    const int   stacks    = cycle < 3 ? 3 : (cycle < 6 ? 4 : 5);
    const float jitter    = ScriptUtil::RandFloat(0.0f, 0.049087387f);
    const float rightBase = -0.19634955f + 0.09817477f * static_cast<float>(burst);
    const float leftBase  = -2.9452431f - 0.09817477f * static_cast<float>(burst);
    const BulletCurve rightCurve{60, Util::HALF_PI, 1.6f, true, false, 1};
    const BulletCurve leftCurve{60, -Util::HALF_PI, 1.4f, true, false, 1};
    const glm::vec2   shootPos = ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET);
    SpawnOneWayStackWithCurve(shootPos, ctx, EBulletType::Shard, EBulletColor::Blue, stacks, 6.5f,
                              0.5f, rightBase + jitter, rightCurve, true);
    SpawnOneWayStackWithCurve(shootPos, ctx, EBulletType::Shard, EBulletColor::Blue, stacks, 6.5f,
                              0.5f, leftBase - jitter, leftCurve, true);

    const float playerFieldY = ctx.playerPos.y - Util::FIELD_OFFSET_Y;
    const bool  fireAimed    = playerFieldY < 192.0f ||
                              (playerFieldY < 256.0f && burst % 2 == 0) ||
                              (playerFieldY >= 256.0f && burst % 3 == 0);
    if (fireAimed) {
        ctx.bullets.SpawnFanAimed(shootPos, ctx.playerPos, EBulletType::Ball,
                                  EBulletColor::Yellow, 5, 2.0f, 0.0f, 0.2617994f);
    }
}

void RunCirnoSecondNonspellInit(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartCirnoPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage2CirnoSecondNonspell);
        StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
    }
    if (t == 200) {
        enemy.m_CanTakeDamage = true;
        ctx.TransitionToSub(enemy, SUB_CIRNO_PREFREEZE_ATTACK_A);
    }
}

void RunCirnoPrefreezeAttackA(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx, 3.0f, 60);
    if (t >= 0 && t < 160 && t % 20 == 0) {
        const int count = std::clamp(8 + enemy.m_BossTimer / 600, 8, 14);
        ctx.bullets.SpawnCircleAimed(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                     ctx.playerPos, EBulletType::RingBall, EBulletColor::Blue,
                                     count, 3.0f);
    }
    if (t >= 10 && t < 170 && t % 20 == 10) {
        ctx.bullets.SpawnCircleAimed(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                     ctx.playerPos, EBulletType::Pellet, EBulletColor::DarkBlue, 8,
                                     1.3f, 0.0f, false, 0.0f, {}, true);
    }
    if (t == 160) ctx.TransitionToSub(enemy, SUB_CIRNO_PREFREEZE_ATTACK_B);
}

void RunCirnoPrefreezeAttackB(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx, 3.0f, 60);
    if (t == 0 || t == 50 || t == 100) {
        const glm::vec2 pos = ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET);
        ctx.lasers.SpawnAimed(pos, ctx.playerPos, 192.0f, 6.0f, 30, 60, 30, 0, 30, 4.0f);
        ctx.lasers.SpawnAtAngle(pos, AimAngle(pos, ctx.playerPos) + 0.3926991f, 192.0f, 6.0f,
                                30, 60, 30, 0, 30, 0.0f, 4.0f);
        ctx.lasers.SpawnAtAngle(pos, AimAngle(pos, ctx.playerPos) - 0.3926991f, 192.0f, 6.0f,
                                30, 60, 30, 0, 30, 0.0f, 4.0f);
        ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 16, 2.0f);
    }
    if (t == 150) ctx.TransitionToSub(enemy, SUB_CIRNO_PREFREEZE_ATTACK_A);
}

void RunPerfectFreeze(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartSpellPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage2PerfectFreeze);
        ctx.anm.SetScript(enemy.m_Vm, Anm::STG2ENM2.offset + 131, Anm::STG2ENM2.offset);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;

    const int loopT = (t - 120) % 595;
    if (loopT == 0 || loopT == 265) ScriptUtil::StartRandomMove(enemy, ctx, 2.0f, 120);
    if (loopT >= 5 && loopT < 155 && (loopT - 5) % 5 == 0) {
        const int cycle    = (t - 120) / 595;
        const int count    = std::clamp(7 + cycle, 7, 18);
        const int colorIdx = ((loopT - 5) / 5) % 5;
        SpawnRandomVarianceCircle(ctx, ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                  EBulletType::RingBall, PerfectFreezeColor(colorIdx), count, 4.0f);
    }
    if (loopT == 175) {
        ctx.bullets.FreezeAllBulletsAsWhite();
    }
    if (loopT >= 275 && loopT < 335 && loopT % 10 == 5) {
        const int cycle = (t - 120) / 595;
        if (cycle < 3) {
            ctx.bullets.SpawnFanStack(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                      ctx.playerPos, EBulletType::Ball, EBulletColor::Blue, 3, 3,
                                      4.0f, 2.0f, 0.0f, 0.3926991f);
        } else if (cycle < 6) {
            ctx.bullets.SpawnFanStack(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                      ctx.playerPos, EBulletType::Ball, EBulletColor::Blue, 5, 3,
                                      4.0f, 2.0f, 0.0f, 0.3926991f);
        } else {
            ctx.bullets.SpawnFanStack(ScriptUtil::ShootPos(enemy, CIRNO_SHOOT_OFFSET),
                                      ctx.playerPos, EBulletType::Ball, EBulletColor::Blue, 5, 3,
                                      5.0f, 2.0f, 0.0f, 0.19634955f);
        }
    }
    if (loopT == 385) {
        ctx.bullets.AccelerateFrozenBulletsRandom(0.01f, 220);
    }
}

void RunDiamondBlizzard(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        enemy.m_CanTakeDamage = false;
        enemy.m_InSpellcard   = false;
        enemy.m_ShowSpellName = false;
        enemy.m_BossTitle     = "Cirno";
        enemy.m_BossTimer     = 0;
        StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5Cancel);
    }
    if (t == 60) {
        StartSpellPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage2DiamondBlizzard);
        ctx.anm.SetScript(enemy.m_Vm, Anm::STG2ENM2.offset + 131, Anm::STG2ENM2.offset);
    }
    if (t == 180) enemy.m_CanTakeDamage = true;
    if (t < 180) return;

    const int elapsed = t - 180;
    const int loopT   = elapsed % 120;
    if (loopT == 0) ScriptUtil::StartRandomMove(enemy, ctx, 1.2f, 120);
    if (elapsed > 0 && elapsed % 10 == 0) {
        const int cycle = elapsed / 120;
        SpawnAtRandomArea(enemy, ctx, 128.0f, std::clamp(10 + cycle, 10, 18));
    }
}

void RunCirnoDeath(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t != 0) return;
    enemy.m_CanTakeDamage = false;
    enemy.m_InSpellcard   = false;
    enemy.m_ShowSpellName = false;
    ctx.bullets.ClearAll();
    ctx.lasers.ClearAll();
    enemy.m_Alive = false;
}

}  // namespace Stage2Detail
