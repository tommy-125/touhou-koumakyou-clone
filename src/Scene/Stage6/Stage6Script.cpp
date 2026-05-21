#include "Scene/Stage6/Stage6Script.hpp"

#include "Stage6PatternCommon.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace ScriptUtil = EnemyScriptUtil;
namespace StageUtil = StageScriptUtil;
using namespace Stage6Detail;
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
            StageUtil::InitVisual(enemy, ctx, {offset, 8, {28.0f, 28.0f}});
            break;
        case SUB_FAIRY_ARC_BLUE_R:
        case SUB_FAIRY_ARC_RED_R:
            StageUtil::InitVisual(enemy, ctx, {offset, 9, {28.0f, 28.0f}});
            break;
        case SUB_FAIRY_SIDE_BURST:
        case SUB_FAIRY_TOP_BURST:
            StageUtil::InitVisual(enemy, ctx, {offset, 11, {28.0f, 28.0f}});
            StageUtil::SetDropCallback(enemy, SUB_FAIRY_DROP_8);
            break;
        case SUB_FAIRY_DROP_8:
            StageUtil::InitDropProxy(enemy, ctx, offset, 11);
            break;
        case SUB_SAKUYA_ENTRY:
            StageUtil::InitBossEntry(enemy, ctx,
                                     {offset,
                                      128,
                                      {-32.0f, 128.0f},
                                      {40.0f, 56.0f},
                                      "Sakuya Izayoi"});
            SetSakuyaPoses(enemy);
            break;
        case SUB_REMILIA_ENTRY:
            StageUtil::InitBossEntry(enemy, ctx,
                                     {Anm::STG6ENM2.offset,
                                      160,
                                      {256.0f, -64.0f},
                                      {56.0f, 56.0f},
                                      "Remilia Scarlet"});
            SetRemiliaPoses(enemy);
            break;
        case SUB_REMILIA_AFTERIMAGE:
            StageUtil::InitDropProxy(enemy, ctx, Anm::STG6ENM2.offset, 165);
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
                StageUtil::DropPowerAndDie(enemy, ctx, 8);
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
                StageUtil::FinishBossDeath(enemy, ctx);
            }
            break;
        default:
            break;
    }
}
