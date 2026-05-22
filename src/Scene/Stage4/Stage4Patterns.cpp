#include "Scene/Stage4/Stage4Patterns.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace Stage4Detail {
constexpr glm::vec2 PATCHOULI_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil                       = EnemyScriptUtil;
using EnemyPatternUtil::RandAngle;
using EnemyPatternUtil::SpawnAimedCircleLinearStack;

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = PATCHOULI_SHOOT_OFFSET) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetPatchouliBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, Anm::STG4ENM.offset + 65, Anm::STG4ENM.offset + 69,
                             Anm::STG4ENM.offset + 70, Anm::STG4ENM.offset + 67,
                             Anm::STG4ENM.offset + 68);
}

void SpawnAtEnemyFieldPos(const Enemy& enemy, EnemySubCtx& ctx, int subId, int life, int score,
                          int itemDrop) {
    const glm::vec2 field = Util::ScreenToGameField(enemy.m_Pos);
    ctx.SpawnEnemy(subId, field.x, field.y, life, score, false, itemDrop);
}

void StartPatchouliPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
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

void StartPatchouliFinalPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                              int lifeThreshold, int lifeSub) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  0,
                                  2400,
                                  SUB_PATCHOULI_DEATH,
                                  SUB_PATCHOULI_DEATH,
                                  lifeThreshold,
                                  lifeSub,
                                  true,
                                  true,
                                  0,
                                  false,
                                  true,
                              });
}

void InitStage4Sub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG4ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 18:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 10:
            ctx.anm.SetScript(enemy.m_Vm, offset + 12, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? PI : 0.0f;
            enemy.m_Speed      = 2.0f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case SUB_LIBRARY_FAIRY_BURST:
        case 19:
        case 20:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_HitboxSize    = {24.0f, 24.0f};
            enemy.m_CanTakeDamage = false;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            ctx.anm.SetScript(enemy.m_Vm, offset + 8, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Speed      = 4.5f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 11:
        case 13:
            ctx.anm.SetScript(enemy.m_Vm, offset + 10, offset);
            enemy.m_HitboxSize       = {32.0f, 32.0f};
            enemy.m_Angle            = enemy.m_SubId == 13 ? 0.0f : Util::HALF_PI;
            enemy.m_Speed            = 2.0f;
            enemy.m_DeathCallbackSub = 12;
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case 12:
        case 15:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {1.0f, 1.0f};
            break;

        case 14:
        case 16:
        case 17:
            ctx.anm.SetScript(enemy.m_Vm, offset + (enemy.m_SubId == 16 ? 14 : 13), offset);
            enemy.m_HitboxSize    = {28.0f, 28.0f};
            enemy.m_CanTakeDamage = false;
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case SUB_KOAKUMA_MIDBOSS:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(-32.0f, 170.0f);
            enemy.m_HitboxSize             = {40.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_BlocksTimeline         = false;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossTitle              = "Koakuma";
            enemy.m_BossLifeCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_TimerCallbackThreshold = 2400;
            enemy.m_TimerCallbackSub       = SUB_KOAKUMA_ESCAPE;
            enemy.m_DeathCallbackSub       = SUB_KOAKUMA_DEATH;
            enemy.m_BoundsMin              = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax              = Util::GameFieldToScreen(352.0f, 176.0f);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case SUB_PATCHOULI_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 65, offset);
            enemy.m_Pos            = Util::GameFieldToScreen(64.0f, -32.0f);
            enemy.m_HitboxSize     = {48.0f, 56.0f};
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = true;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Patchouli Knowledge";
            enemy.m_BossLifeCount  = 2;
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 144.0f);
            SetPatchouliBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void RunStage4Sub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;

    switch (enemy.m_SubId) {
        case 0:
        case 10:
            RunBookFairy(enemy, ctx, t, SUB_LIBRARY_FAIRY_BURST);
            break;
        case 18:
            RunBookFairy(enemy, ctx, t, 20);
            break;

        case SUB_LIBRARY_FAIRY_BURST:
            RunBurstFamiliar(enemy, ctx, t, false, false);
            break;
        case 19:
            RunBurstFamiliar(enemy, ctx, t, false, true);
            break;
        case 20:
            RunBurstFamiliar(enemy, ctx, t, true, false);
            break;

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            RunStage4SmallFairy(enemy, ctx, t);
            break;

        case 11:
            RunRingCaster(enemy, ctx, t, false);
            break;
        case 13:
            RunRingCaster(enemy, ctx, t, true);
            break;
        case 12:
            if (t == 0) {
                ctx.BulletCancelIntoPointItems();
                ScriptUtil::DropPowerItems(enemy, ctx, 3);
                enemy.m_Alive = false;
            }
            break;

        case 14:
            RunCrystalFairy(enemy, ctx, t, 60, true, false);
            break;
        case 16:
            RunCrystalFairy(enemy, ctx, t, 10, false, false);
            break;
        case 17:
            RunCrystalFairy(enemy, ctx, t, 6, true, false);
            break;
        case 15:
            if (t >= 30) enemy.m_Alive = false;
            break;

        case SUB_KOAKUMA_MIDBOSS:
            RunKoakumaMidboss(enemy, ctx, t);
            break;
        case SUB_KOAKUMA_DEATH:
            if (t == 0) {
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                ScriptUtil::DropPowerItems(enemy, ctx, 8);
                ctx.TransitionToSub(enemy, SUB_KOAKUMA_ESCAPE);
            }
            break;
        case SUB_KOAKUMA_ESCAPE:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                ctx.BulletCancelIntoPointItems();
                ctx.lasers.ClearAll();
                enemy.m_Alive = false;
            }
            break;

        case SUB_PATCHOULI_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_PATCHOULI_FIRST_NONSPELL);
            break;
        case SUB_PATCHOULI_FIRST_NONSPELL:
            RunPatchouliFirstNonSpell(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_SECOND_NONSPELL:
            RunPatchouliSecondNonSpell(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_PRINCESS_UNDINE:
            RunPrincessUndine(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_SYLPHY_HORN_ADV:
            RunSylphyHornAdvanced(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_FINAL_NONSPELL:
            RunFinalNonSpell(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_PHASE_OUT:
            RunMercuryPoison(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_WATER_ELF:
            RunWaterElf(enemy, ctx, t);
            break;
        case SUB_PATCHOULI_DEATH:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                ctx.bullets.ClearAll();
                ctx.lasers.ClearAll();
                enemy.m_Alive = false;
            }
            break;

        default:
            break;
    }
}

}  // namespace Stage4Detail