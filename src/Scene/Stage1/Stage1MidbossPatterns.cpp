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
}  // namespace

void InitRumiaMidbossSub(Enemy& enemy, EnemySubCtx& ctx) {
    switch (enemy.m_SubId) {
        case SUB_MIDBOSS_MAIN: {  // Mid-boss main pattern entry
            StageUtil::InitBossEntry(enemy, ctx,
                                     StageUtil::LoadBossEntryConfig(StageUtil::ConfigId::BossEntry::Stage1RumiaMidboss));
            ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
            break;
        }
        default:
            break;
    }
}

void RunRumiaMidbossSub(Enemy& enemy, EnemySubCtx& ctx, int t) {
    switch (enemy.m_SubId) {
        case SUB_MIDBOSS_MAIN: {  // Mid-boss main pattern (Sub4 scatter waves + Sub5 random +
                                  // circle3 rings)
            auto circle3 = [&](EBulletColor c) {
                constexpr float s1 = 2.0f, s2 = 1.5f;
                constexpr int   n = 3;
                for (int ring = 0; ring < n; ring++) {
                    float spd = s1 - (s1 - s2) * ring / n;
                    ctx.bullets.SpawnCircleAimed(enemy.m_Pos, ctx.playerPos, EBulletType::RingBall,
                                                 c, 16, spd, true);
                }
            };

            static constexpr EBulletColor kSub4Colors[5] = {
                EBulletColor::Blue, EBulletColor::Red, EBulletColor::Green, EBulletColor::Yellow,
                EBulletColor::Orange};
            static constexpr float kSub4Offsets[5] = {0.0f, 0.06544985f, 0.1308997f, 0.19634955f,
                                                      0.2617994f};
            auto                   sub4Wave        = [&](int wave) {
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Pellet, kSub4Colors[wave], 8,
                                                                 0.0f, kSub4Offsets[wave], false, 0.02f, 40);
            };

            auto randSpeed = [&]() { return (rand() % 1000) / 1000.0f * 3.0f + 0.5f; };
            auto randAngle = [&]() {
                return ((rand() % 1000) / 1000.0f * 2.0f - 1.0f) * 3.14159265f;
            };
            auto sub5Wave = [&](int wave) {
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Pellet, kSub4Colors[wave], 4,
                                        randSpeed(), randAngle(), false);
                ctx.bullets.SpawnCircle(enemy.m_Pos, EBulletType::Rice, kSub4Colors[wave], 4,
                                        randSpeed(), randAngle(), false);
            };

            if (t == 0) ctx.StartLerpTo(enemy, 320.0f, 128.0f, 60);
            if (t == 60) enemy.m_CanTakeDamage = true;
            if (t == 160) circle3(EBulletColor::Blue);
            for (int cycle = 0; cycle < 2; cycle++) {
                const int base = 192 + cycle * 648;
                if (t == base + 10) ctx.StartLerpTo(enemy, 192.0f, 64.0f, 60);
                for (int w = 0; w < 5; w++) {
                    if (t == base + 70 + w * 8) sub4Wave(w);
                }
                if (t == base + 192) ctx.StartLerpTo(enemy, 64.0f, 96.0f, 60);
                if (t == base + 222) circle3(EBulletColor::Green);
                if (t == base + 252) circle3(EBulletColor::Yellow);
                if (t == base + 334) ctx.StartLerpTo(enemy, 192.0f, 80.0f, 60);
                for (int w = 0; w < 5; w++) {
                    if (t == base + 394 + w * 8) sub5Wave(w);
                }
                if (t == base + 516) ctx.StartLerpTo(enemy, 320.0f, 96.0f, 60);
                if (t == base + 546) circle3(EBulletColor::Blue);
                if (t == base + 576) circle3(EBulletColor::Red);
            }
            if (t == 850) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            if (t == 910) enemy.m_Alive = false;
            break;
        }

        case SUB_MIDBOSS_ESCAPE: {  // Mid-boss escape (timer callback from Sub8)
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Speed = 0.0f;
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 60) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    ctx.renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }

        case SUB_MIDBOSS_DEATH: {  // Mid-boss death (death callback from Sub8)
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Speed = 0.0f;
                StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power5);
            }
            if (t == 40) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 120);
            if (t == 160) {
                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    ctx.renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
            break;
        }
        default:
            break;
    }
}

}  // namespace Stage1Detail
