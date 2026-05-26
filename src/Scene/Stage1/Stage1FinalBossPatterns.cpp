#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Scene/Stage1/Stage1Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage1Detail {
namespace {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;

void TransitionToRandomSub(Enemy& enemy, const EnemySubCtx& ctx, int subA, int subB, int subC) {
    const int roll = rand() % 3;
    ctx.TransitionToSub(enemy, roll == 0 ? subA : (roll == 1 ? subB : subC));
}

void StartRumiaPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* phaseId) {
    StageUtil::StartBossPhase(enemy, ctx, phaseId);
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* phaseId) {
    StartRumiaPhase(enemy, ctx, phaseId);
    ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
}

}  // namespace

void InitRumiaBossEntry(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::InitBossEntry(
        enemy, ctx, StageUtil::LoadBossEntryConfig(StageUtil::ConfigId::BossEntry::Stage1RumiaBoss));
    enemy.m_DeathCallbackSub = SUB_BOSS_PHASE2_INIT;
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

void RunRumiaBossEntry(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ctx.StartLerpTo(enemy, 192.0f, 96.0f, 60);
    }
    if (t == 65) {
        int off2 = Anm::STG1ENM2.offset;
        ctx.anm.SetScript(enemy.m_Vm, off2 + 133, off2);
        ctx.TransitionToSub(enemy, SUB_BOSS_PHASE1_INIT);
    }
}

void RunRumiaFirstNonspellInit(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ScriptUtil::SetDeathEffects(enemy, 671, 676);
        StartRumiaPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage1RumiaFirstNonspell);
    }
    if (t == 100) ctx.TransitionToSub(enemy, SUB_BOSS_PHASE1_ATTACK_A);
}

void RunRumiaFirstAttackA(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx);
    if (t >= 12 && t <= 60 && (t - 12) % 8 == 0) {
        const int burst = (t - 12) / 8;
        ctx.bullets.SpawnFanStack(
            enemy.m_Pos, ctx.playerPos, EBulletType::Ball,
            (burst & 1) == 0 ? EBulletColor::Red : EBulletColor::DarkRed, 1, 10,
            burst == 0 ? 3.0f : 4.0f, 1.0f, 0.0f, 0.09817477f);
    }
    if (t == 180)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE1_ATTACK_B, SUB_BOSS_PHASE1_ATTACK_C,
                              SUB_BOSS_PHASE1_ATTACK_D);
}

void RunRumiaFirstAttackB(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx);
    if (t == 60) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 12, 4.0f);
    }
    if (t == 68) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                     EBulletColor::DarkBlue, 16, 3.0f);
    }
    if (t == 76) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 12, 2.0f, -0.1308997f);
    }
    if (t == 84) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                     EBulletColor::DarkBlue, 16, 3.0f);
    }
    if (t == 92) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 12, 4.0f, 0.1308997f);
    }
    if (t == 100) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                     EBulletColor::DarkBlue, 16, 3.0f);
    }
    if (t == 108) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 12, 2.0f, -0.1308997f);
    }
    if (t == 228)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE1_ATTACK_A, SUB_BOSS_PHASE1_ATTACK_C,
                              SUB_BOSS_PHASE1_ATTACK_D);
}

void RunRumiaFirstAttackC(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx);
    if (t == 80) {
        ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                  EBulletColor::Red, 2, 16, 5.0f, 1.0f, 0.0f, 0.06544985f);
    }
    if (t == 110) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 16, 2.0f);
    }
    if (t == 200)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE1_ATTACK_B, SUB_BOSS_PHASE1_ATTACK_A,
                              SUB_BOSS_PHASE1_ATTACK_D);
}

void RunRumiaFirstAttackD(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ScriptUtil::StartRandomMove(enemy, ctx);
        enemy.m_Mirrored = (rand() % 2) == 0;
    }
    if (t >= 0 && t < 32 && t % 2 == 0) {
        int   step   = t / 2;
        float speed  = 1.0f + step * 0.25f;
        float offset = enemy.m_Mirrored ? (-0.2617994f + step * 0.06544985f)
                                        : (0.2617994f - step * 0.06544985f);
        ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                  EBulletColor::Green, 1, speed, offset, 0.0f);
    }
    if (t == 124)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE1_ATTACK_B, SUB_BOSS_PHASE1_ATTACK_C,
                              SUB_BOSS_PHASE1_ATTACK_A);
}

void RunNightBird(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) StartSpellPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage1NightBird);
    if (t == 120) enemy.m_CanTakeDamage = true;

    int loopT = (t >= 120) ? (t - 120) % 356 : -1;
    if (loopT < 0) return;

    auto shootSpiral = [&](int groupStart, EBulletColor color, float initOffset,
                           float angleStep) {
        if (loopT >= groupStart && loopT < groupStart + 32 && (loopT - groupStart) % 2 == 0) {
            int   step   = (loopT - groupStart) / 2;
            float speed  = 1.0f + step * 0.2f;
            float offset = initOffset + step * angleStep;
            ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall, color, 1,
                                      speed, offset, 0.06544985f);
        }
    };

    shootSpiral(0, EBulletColor::DarkBlue, -0.57119864f, +0.19634955f);
    shootSpiral(32, EBulletColor::DarkCyan, +0.57119864f, -0.19634955f);
    shootSpiral(64, EBulletColor::Blue, -0.7853982f, +0.2617994f);
    shootSpiral(96, EBulletColor::Cyan, +0.7853982f, -0.2617994f);
    shootSpiral(128, EBulletColor::DarkBlue, -0.57119864f, +0.19634955f);
    shootSpiral(160, EBulletColor::DarkCyan, +0.57119864f, -0.19634955f);
    shootSpiral(192, EBulletColor::Blue, -0.7853982f, +0.2617994f);
    shootSpiral(224, EBulletColor::Cyan, +0.7853982f, -0.2617994f);
    if (loopT == 256) {
        ctx.MoveRandInBounds(enemy);
        ctx.StartLerpDir(enemy, 2.0f, 120);
    }
}

void RunRumiaSecondNonspellInit(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ScriptUtil::SetDeathEffects(enemy, 671, 676);
        StartRumiaPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage1RumiaSecondNonspell);
        StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
    }
    if (t == 200) {
        enemy.m_CanTakeDamage = true;
        ctx.TransitionToSub(enemy, SUB_BOSS_PHASE2_ATTACK_A);
    }
}

void RunRumiaSecondAttackA(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx);
    if (t == 12) {
        ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                  EBulletColor::Green, 1, 8, 3.0f, 1.0f, 0.0f, 0.09817477f);
    }
    if (t >= 20 && t <= 60 && (t - 20) % 8 == 0) {
        ctx.lasers.SpawnAimed(enemy.m_Pos, ctx.playerPos, 500.0f, 16.0f, 120, 60, 14, 16,
                              120);
    }
    if (t == 224)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE2_ATTACK_B, SUB_BOSS_PHASE2_ATTACK_C,
                              SUB_BOSS_PHASE2_ATTACK_D);
}

void RunRumiaSecondAttackB(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx);
    if (t == 60) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                     EBulletColor::Green, 24, 2.0f);
    }
    if (t == 90) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Pellet,
                                     EBulletColor::Green, 28, 2.6f);
    }
    if (t == 120) {
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                     EBulletColor::Green, 24, 2.0f);
    }
    if (t == 240)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE2_ATTACK_A, SUB_BOSS_PHASE2_ATTACK_C,
                              SUB_BOSS_PHASE2_ATTACK_D);
}

void RunRumiaSecondAttackC(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) ScriptUtil::StartRandomMove(enemy, ctx);
    if (t == 60)
        ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                  EBulletColor::Yellow, 4, 2, 3.0f, 1.0f, 0.0f, 0.5235988f);
    if (t == 80)
        ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                  EBulletColor::Yellow, 5, 2, 3.0f, 1.0f, 0.0f, 0.5235988f);
    if (t == 100)
        ctx.bullets.SpawnFanStack(enemy.m_Pos, ctx.playerPos, EBulletType::Rice,
                                  EBulletColor::Yellow, 7, 2, 3.0f, 1.0f, 0.0f, 0.5235988f);
    if (t == 220)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE2_ATTACK_B, SUB_BOSS_PHASE2_ATTACK_A,
                              SUB_BOSS_PHASE2_ATTACK_D);
}

void RunRumiaSecondAttackD(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        ScriptUtil::StartRandomMove(enemy, ctx);
        enemy.m_Mirrored = (rand() % 2) == 0;
    }
    if (t >= 0 && t < 32 && t % 2 == 0) {
        int   step   = t / 2;
        float speed  = 1.0f + step * 0.25f;
        float offset = enemy.m_Mirrored ? (-0.7139983f + step * 0.14279966f)
                                        : (0.7139983f - step * 0.14279966f);
        ctx.bullets.SpawnFanAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                  EBulletColor::Green, 2, speed, offset, 0.14279966f);
    }
    if (t == 124)
        TransitionToRandomSub(enemy, ctx, SUB_BOSS_PHASE2_ATTACK_B, SUB_BOSS_PHASE2_ATTACK_C,
                              SUB_BOSS_PHASE2_ATTACK_A);
}

void RunDemarcation(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) StartSpellPhase(enemy, ctx, StageUtil::ConfigId::BossPhase::Stage1Demarcation);
    if (t == 120) enemy.m_CanTakeDamage = true;

    int loopT = (t >= 120) ? (t - 120) % 336 : -1;
    if (loopT < 0) return;

    const glm::vec2 ringAnchor = enemy.m_Pos + glm::vec2{1.0f, 0.0f};
    auto            spawnPair  = [&](EBulletColor color, bool upFirst) {
        float a1 = upFirst ? +Util::HALF_PI : -Util::HALF_PI;
        float a2 = upFirst ? -Util::HALF_PI : +Util::HALF_PI;
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ringAnchor, EBulletType::Rice, color, 12, 3.0f,
                                     0.0f, false, 0.0f, {40, a1, 1.5f, true}, true);
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ringAnchor, EBulletType::Rice, color, 12, 3.0f,
                                     0.2617994f, false, 0.0f, {40, a2, 1.5f, true}, true);
    };

    if (loopT == 0) spawnPair(EBulletColor::Blue, true);
    if (loopT == 60) spawnPair(EBulletColor::Green, false);
    if (loopT == 120) spawnPair(EBulletColor::Red, true);
    if (loopT == 180) {
        ctx.MoveRandInBounds(enemy);
        ctx.StartLerpDir(enemy, 2.0f, 120);
    }

    int spiralT = loopT - 180;
    if (spiralT >= 0 && spiralT < 96 && spiralT % 2 == 0) {
        int   phase  = (spiralT % 48) / 24;
        int   step   = (spiralT % 24) / 2;
        float speed  = 1.0f + step * 0.2f;
        float offset = (phase == 0 ? -0.57119864f + step * 0.14279966f
                                   : +0.57119864f - step * 0.14279966f);
        ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                     EBulletColor::Blue, 1, speed, offset, false, 0.0f,
                                     {40, 0.0f, 3.0f, false, true, 1, 12, 0.4f}, true);
    }
    if (loopT == 276) {
        ctx.MoveRandInBounds(enemy);
        ctx.StartLerpDir(enemy, 2.0f, 60);
    }
}

void RunRumiaBossDeath(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t != 0) return;
    enemy.m_CanTakeDamage  = false;
    enemy.m_InSpellcard    = false;
    enemy.m_ShowSpellName  = false;
    enemy.m_SpellcardBonus = 0;
    ctx.bullets.ClearAll();
    ctx.lasers.ClearAll();
    enemy.m_Alive = false;
}

}  // namespace Stage1Detail
