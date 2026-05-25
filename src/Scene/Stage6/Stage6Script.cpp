#include "Scene/Stage6/Stage6Script.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Scene/Stage6/Stage6Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"

namespace {
namespace StageUtil = StageScriptUtil;
using namespace Stage6Detail;
}  // namespace

Stage6Script::Stage6Script() {
    AddTimedRunOnlyPattern(SUB_STAGE_EFFECTS, [](Enemy& enemy, EnemySubCtx&, int t) {
        if (t >= 2160) enemy.m_Alive = false;
    });

    AddTimedPattern(
        {SUB_FAIRY_ARC_BLUE_L, SUB_FAIRY_ARC_RED_L}, InitStage6ArcFairyLeft,
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            RunArcFairy(enemy, ctx, t, enemy.m_SubId == SUB_FAIRY_ARC_RED_L,
                        enemy.m_SubId == SUB_FAIRY_ARC_BLUE_L);
        });

    AddTimedPattern(
        {SUB_FAIRY_ARC_BLUE_R, SUB_FAIRY_ARC_RED_R}, InitStage6ArcFairyRight,
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            RunArcFairy(enemy, ctx, t, enemy.m_SubId == SUB_FAIRY_ARC_RED_R,
                        enemy.m_SubId == SUB_FAIRY_ARC_BLUE_R);
        });

    AddTimedPattern(
        {SUB_FAIRY_SIDE_BURST, SUB_FAIRY_TOP_BURST}, InitStage6BurstFairy,
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            RunBurstFairy(enemy, ctx, t, enemy.m_SubId == SUB_FAIRY_TOP_BURST);
        });

    AddTimedPattern(
        SUB_FAIRY_DROP_8, InitStage6FairyDropProxy,
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (t == 0) StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power8Die);
        });

    AddTimedPattern(
        SUB_SAKUYA_ENTRY, InitStage6SakuyaEntry,
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_SAKUYA_MAIN);
        });

    AddTimedRunOnlyPattern(SUB_SAKUYA_MAIN, RunSakuyaMain);
    AddTimedRunOnlyPattern(SUB_SAKUYA_ETERNAL_MEEK, RunEternalMeek);

    AddTimedRunOnlyPattern(SUB_SAKUYA_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            ctx.SetTimeStopped(false);
            StageUtil::ApplyReward(enemy, ctx, StageUtil::ConfigId::Reward::Power20BombCancelDie);
        }
    });

    AddTimedRunOnlyPattern(SUB_SAKUYA_EXIT, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) {
            ctx.SetTimeStopped(false);
            ctx.BulletCancelIntoPointItems();
            ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
        }
        if (t >= 60) enemy.m_Alive = false;
    });

    AddTimedPattern(
        SUB_REMILIA_ENTRY, InitStage6RemiliaEntry,
        [](Enemy& enemy, EnemySubCtx& ctx, int t) {
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
            if (t == 120) ctx.TransitionToSub(enemy, SUB_REMILIA_WAIT);
        });

    AddTimedRunOnlyPattern(SUB_REMILIA_WAIT, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) ctx.TransitionToSub(enemy, SUB_REMILIA_NONSPELL_1);
    });

    AddTimedRunOnlyPattern(SUB_REMILIA_NONSPELL_1, RunRemiliaNonSpell1);
    AddTimedRunOnlyPattern(SUB_REMILIA_STAR, RunStarOfDavid);
    AddTimedRunOnlyPattern(SUB_REMILIA_NONSPELL_2, RunRemiliaNonSpell2);
    AddTimedRunOnlyPattern(SUB_REMILIA_SCARLET, RunScarletNetherworld);
    AddTimedRunOnlyPattern(SUB_REMILIA_NONSPELL_3, RunRemiliaNonSpell3);
    AddTimedRunOnlyPattern(SUB_REMILIA_VLAD, RunVlad);
    AddTimedRunOnlyPattern(SUB_REMILIA_NONSPELL_4, RunRemiliaNonSpell4);

    AddTimedPattern(
        SUB_REMILIA_AFTERIMAGE, InitStage6RemiliaAfterimage,
        [](Enemy& enemy, EnemySubCtx&, int t) {
            if (t == 30) {
                enemy.m_AngularVelocity = RandFloat(-0.3926991f, 0.3926991f) / 20.0f;
            }
            if (t >= 150) enemy.m_Alive = false;
        });

    AddTimedRunOnlyPattern(SUB_REMILIA_SHOOT, RunScarletShoot);
    AddTimedRunOnlyPattern(SUB_REMILIA_RED_MAGIC, RunRedMagic);
    AddTimedRunOnlyPattern(SUB_REMILIA_DEATH, [](Enemy& enemy, EnemySubCtx& ctx, int t) {
        if (t == 0) StageUtil::FinishBossDeath(enemy, ctx);
    });
}

void Stage6Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG6ENM.folder, Anm::STG6ENM.txt, Anm::STG6ENM.offset);
    anm.LoadAnm(Anm::STG6ENM2.folder, Anm::STG6ENM2.txt, Anm::STG6ENM2.offset);
}
