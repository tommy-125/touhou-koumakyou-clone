#include "Scene/Stage5/Stage5Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage5/Stage5PatternCommon.hpp"
#include "Scene/Stage5/Stage5Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil  = StageScriptUtil;
using namespace Stage5Detail;
}  // namespace

Stage5Script::Stage5Script() {
    constexpr int offset = Anm::STG5ENM.offset;

    AddTimedPattern(
        {0, 2, 3, 4, 5},
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::InitVisual(enemy, ctx, {offset, 12, {28.0f, 28.0f}, 670, 678});
            if (enemy.m_SubId == 2) {
                StageUtil::SetDropCallback(enemy, SUB_MAID_DROP_4);
            } else if (enemy.m_SubId >= 3 && enemy.m_SubId <= 5) {
                StageUtil::SetDropCallback(enemy, SUB_MAID_DROP_6);
            }
        },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
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
                default:
                    break;
            }
        });

    AddTimedPattern(
        {1, 9},
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::InitVisual(enemy, ctx, {offset, 10, {28.0f, 28.0f}, 670, 678});
        },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) { RunSideMaid(enemy, ctx, t, enemy.m_SubId); });

    AddTimedPattern(
        {6, 10},
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::InitVisual(enemy, ctx, {offset, 11, {28.0f, 28.0f}, 670, 678});
        },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (enemy.m_SubId == 6) {
                RunRingMaid(enemy, ctx, t);
            } else {
                RunSideMaid(enemy, ctx, t, enemy.m_SubId);
            }
        });

    AddTimedPattern(
        11,
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::InitVisual(enemy, ctx, {offset, 12, {28.0f, 28.0f}, 670, 678});
        },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) { RunSideMaid(enemy, ctx, t, enemy.m_SubId); });

    AddTimedPattern(
        SUB_MAID_DROP_4,
        [](Enemy& enemy, EnemySubCtx& ctx) { StageUtil::InitDropProxy(enemy, ctx, offset, 16); },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (t == 0) StageUtil::DropPowerAndDie(enemy, ctx, 4);
        });

    AddTimedPattern(
        SUB_MAID_DROP_6,
        [](Enemy& enemy, EnemySubCtx& ctx) { StageUtil::InitDropProxy(enemy, ctx, offset, 16); },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (t == 0) StageUtil::DropPowerAndDie(enemy, ctx, 6);
        });

    AddTimedPattern(
        {SUB_SAKUYA_HELPER_1, SUB_SAKUYA_HELPER_2, SUB_SAKUYA_HELPER_3, SUB_SAKUYA_HELPER_4,
         SUB_SAKUYA_HELPER_5, SUB_SAKUYA_HELPER_6, SUB_SAKUYA_HELPER_7, SUB_SAKUYA_HELPER_8,
         SUB_SAKUYA_KUNAI_HELPER_1, SUB_SAKUYA_KUNAI_HELPER_2, SUB_SAKUYA_KUNAI_HELPER_3,
         SUB_SAKUYA_KUNAI_HELPER_4},
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::InitVisual(enemy, ctx, {offset, 16, {0.0f, 0.0f}});
            InitSakuyaHelper(enemy);
        },
        RunSakuyaHelper);

    AddTimedPattern(
        SUB_SAKUYA_HELPER_DEATH,
        [](Enemy& enemy, EnemySubCtx& ctx) { StageUtil::InitDropProxy(enemy, ctx, offset, 16); },
        [](Enemy& enemy, EnemySubCtx&, int t) {
            if (t >= 60) enemy.m_Alive = false;
        });

    AddTimedPattern(
        {SUB_SAKUYA_MIDBOSS_ENTRY, SUB_SAKUYA_ENTRY},
        [](Enemy& enemy, EnemySubCtx& ctx) {
            StageUtil::InitBossEntry(enemy, ctx,
                                     {Anm::STG5ENM2.offset,
                                      128,
                                      {-32.0f, 128.0f},
                                      {40.0f, 56.0f},
                                      "Sakuya Izayoi",
                                      enemy.m_SubId == SUB_SAKUYA_ENTRY,
                                      enemy.m_SubId == SUB_SAKUYA_ENTRY ? 2 : 0,
                                      {32.0f, 48.0f},
                                      {352.0f, 132.0f}});
            SetSakuyaBossPoses(enemy);
        },
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) {
                ctx.TransitionToSub(enemy, enemy.m_SubId == SUB_SAKUYA_MIDBOSS_ENTRY
                                               ? SUB_SAKUYA_MIDBOSS_MAIN
                                               : SUB_SAKUYA_FIRST_NONSPELL);
            }
        });

    AddTimedRunOnlyPattern(SUB_SAKUYA_MIDBOSS_MAIN, RunMidbossMain);
    AddTimedRunOnlyPattern(SUB_SAKUYA_MISDIRECTION, RunMisdirection);
    AddTimedRunOnlyPattern(SUB_SAKUYA_MIDBOSS_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            ctx.SetTimeStopped(false);
            ctx.items.SpawnItem(enemy.m_Pos, ItemType::Life);
            ScriptUtil::DropPowerItems(enemy, ctx, 10);
            ctx.BulletCancelIntoPointItems();
            ctx.TransitionToSub(enemy, SUB_SAKUYA_MIDBOSS_EXIT);
        }
    });
    AddTimedRunOnlyPattern(SUB_SAKUYA_MIDBOSS_EXIT, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            ctx.SetTimeStopped(false);
            ctx.BulletCancelIntoPointItems();
            StageUtil::HideBossForExit(enemy);
        }
        if (t == 130) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
        if (t >= 190) enemy.m_Alive = false;
    });

    AddTimedRunOnlyPattern(SUB_SAKUYA_FIRST_NONSPELL, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunSakuyaNonSpell(enemy, ctx, t, 0);
    });
    AddTimedRunOnlyPattern(SUB_SAKUYA_CLOCK_CORPSE, RunClockCorpse);
    AddTimedRunOnlyPattern(SUB_SAKUYA_SECOND_NONSPELL, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunSakuyaNonSpell(enemy, ctx, t, 1);
    });
    AddTimedRunOnlyPattern(SUB_SAKUYA_LUNA_CLOCK, RunLunaClock);
    AddTimedRunOnlyPattern(SUB_SAKUYA_FINAL_NONSPELL, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        RunSakuyaNonSpell(enemy, ctx, t, 2);
    });
    AddTimedRunOnlyPattern(SUB_SAKUYA_FINAL_SPELL, RunFinalSpell);
    AddTimedRunOnlyPattern(SUB_SAKUYA_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) StageUtil::FinishBossDeath(enemy, ctx);
    });
}

void Stage5Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG5ENM.folder, Anm::STG5ENM.txt, Anm::STG5ENM.offset);
    anm.LoadAnm(Anm::STG5ENM2.folder, Anm::STG5ENM2.txt, Anm::STG5ENM2.offset);
}
