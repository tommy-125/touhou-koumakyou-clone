#include "Scene/Stage5/Stage5Script.hpp"

#include "Stage5PatternCommon.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil = StageScriptUtil;
using namespace Stage5Detail;
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
            StageUtil::InitVisual(enemy, ctx, {offset, 12, {28.0f, 28.0f}, 670, 678});
            if (enemy.m_SubId == 2) {
                StageUtil::SetDropCallback(enemy, SUB_MAID_DROP_4);
            } else if (enemy.m_SubId >= 3 && enemy.m_SubId <= 5) {
                StageUtil::SetDropCallback(enemy, SUB_MAID_DROP_6);
            }
            break;
        case 1:
        case 9:
            StageUtil::InitVisual(enemy, ctx, {offset, 10, {28.0f, 28.0f}, 670, 678});
            break;
        case 6:
        case 10:
            StageUtil::InitVisual(enemy, ctx, {offset, 11, {28.0f, 28.0f}, 670, 678});
            break;
        case 11:
            StageUtil::InitVisual(enemy, ctx, {offset, 12, {28.0f, 28.0f}, 670, 678});
            break;
        case SUB_MAID_DROP_4:
        case SUB_MAID_DROP_6:
            StageUtil::InitDropProxy(enemy, ctx, offset, 16);
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
            StageUtil::InitVisual(enemy, ctx, {offset, 16, {0.0f, 0.0f}});
            InitSakuyaHelper(enemy);
            break;

        case SUB_SAKUYA_HELPER_DEATH:
            StageUtil::InitDropProxy(enemy, ctx, offset, 16);
            break;

        case SUB_SAKUYA_MIDBOSS_ENTRY:
        case SUB_SAKUYA_ENTRY:
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
                StageUtil::DropPowerAndDie(enemy, ctx, 4);
            }
            break;
        case SUB_MAID_DROP_6:
            if (t == 0) {
                StageUtil::DropPowerAndDie(enemy, ctx, 6);
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
                ctx.TransitionToSub(enemy, SUB_SAKUYA_MIDBOSS_EXIT);
            }
            break;
        case SUB_SAKUYA_MIDBOSS_EXIT:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ctx.BulletCancelIntoPointItems();
                StageUtil::HideBossForExit(enemy);
            }
            if (t == 130) ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            if (t >= 190) enemy.m_Alive = false;
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
                StageUtil::FinishBossDeath(enemy, ctx);
            }
            break;

        default:
            break;
    }
}
