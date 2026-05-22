#include "Scene/Stage3/Stage3Patterns.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace Stage3Detail {
constexpr glm::vec2 MEILING_SHOOT_OFFSET = {0.0f, -12.0f};
namespace ScriptUtil                     = EnemyScriptUtil;

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = MEILING_SHOOT_OFFSET) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetMeilingBossPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, Anm::STG3ENM.offset + 64, Anm::STG3ENM.offset + 65,
                             Anm::STG3ENM.offset + 65, Anm::STG3ENM.offset + 65,
                             Anm::STG3ENM.offset + 65);
}

void StartNonSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, int life, int lifeCount,
                        int timerFrames, int nextSub, int deathSub) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  "Hong Meiling",
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  deathSub,
                                  std::max(1200, life / 8),
                                  nextSub,
                                  false,
                                  false,
                                  0,
                                  false,
                                  true,
                              });
}

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int lifeCount,
                     int timerFrames, int nextSub) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  -1,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  nextSub,
                                  -1,
                                  -1,
                                  true,
                                  true,
                                  0,
                                  false,
                                  true,
                              });
    ctx.StartLerpTo(enemy, 192.0f, 64.0f, 120);
}

void SpawnRandomSpeedRange(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                           int count, float minSpeed, float maxSpeed,
                           bool rotateWithAngle = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, ScriptUtil::RandFloat(minSpeed, maxSpeed),
                                ScriptUtil::RandFloat(-PI, PI), false, 0.0f, 0, rotateWithAngle);
    }
}

void SpawnRandomVectorAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                            int count, float maxSpeed, float minSpeed, float accelSpeed,
                            float accelAngle, bool rotateWithAngle = true) {
    const glm::vec2 accel = {std::cos(accelAngle) * accelSpeed, std::sin(accelAngle) * accelSpeed};
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, ScriptUtil::RandFloat(minSpeed, maxSpeed),
                                ScriptUtil::RandFloat(-PI, PI), true, 0.0f, 0, rotateWithAngle,
                                accel, 9999, 12, 0.5f);
    }
}

void SpawnRandomDownAccel(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type, EBulletColor color,
                          int count, float maxSpeed, float minSpeed, bool rotateWithAngle = true) {
    SpawnRandomVectorAccel(pos, ctx, type, color, count, maxSpeed, minSpeed, 0.027f, Util::HALF_PI,
                           rotateWithAngle);
}

void InitStage3Sub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG3ENM.offset;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
            ctx.anm.SetScript(enemy.m_Vm, offset + (enemy.m_SubId == 0 ? 13 : 14), offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? 2.6179938f : 0.5235988f;
            enemy.m_Speed      = 4.5f;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            break;

        case 2:
        case 3:
            ctx.anm.SetScript(enemy.m_Vm, offset + 13, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = enemy.m_Mirrored ? -2.0943952f : -1.0471976f;
            enemy.m_Speed      = 4.0f;
            ScriptUtil::SetDeathEffects(enemy, 669, 678);
            break;

        case 4:
        case 7:
        case 8:
        case SUB_MEILING_SUPPORT_FAIRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 15, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = enemy.m_SubId == 8 ? 2.5f : 1.5f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 5:
            ctx.anm.SetScript(enemy.m_Vm, offset + 16, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 1.5f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case 6:
            ctx.anm.SetScript(enemy.m_Vm, offset + 0, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            enemy.m_Angle      = Util::HALF_PI;
            enemy.m_Speed      = 2.0f;
            ScriptUtil::SetDeathEffects(enemy, 670, 678);
            break;

        case SUB_MEILING_MIDBOSS_MAIN:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos                    = Util::GameFieldToScreen(352.0f, -96.0f);
            enemy.m_HitboxSize             = {56.0f, 56.0f};
            enemy.m_IsBoss                 = true;
            enemy.m_BlocksTimeline         = true;
            enemy.m_CanTakeDamage          = false;
            enemy.m_ItemDropCount          = 0;
            enemy.m_BossTitle              = "Hong Meiling";
            enemy.m_BossLifeCount          = 0;
            enemy.m_BossMaxLife            = std::max(1, enemy.m_Life);
            enemy.m_TimerCallbackThreshold = 1800;
            enemy.m_TimerCallbackSub       = SUB_MEILING_MIDBOSS_ESCAPE;
            enemy.m_LifeCallbackThreshold  = 1300;
            enemy.m_LifeCallbackSub        = SUB_MEILING_MIDBOSS_SPELL_A;
            enemy.m_DeathCallbackSub       = SUB_MEILING_MIDBOSS_DEATH;
            enemy.m_BoundsMin              = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax              = Util::GameFieldToScreen(352.0f, 144.0f);
            SetMeilingBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        case SUB_MEILING_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 64, offset);
            enemy.m_Pos              = Util::GameFieldToScreen(352.0f, -96.0f);
            enemy.m_HitboxSize       = {56.0f, 56.0f};
            enemy.m_IsBoss           = true;
            enemy.m_BlocksTimeline   = true;
            enemy.m_CanTakeDamage    = false;
            enemy.m_ItemDropCount    = 0;
            enemy.m_BossTitle        = "Hong Meiling";
            enemy.m_BossLifeCount    = 2;
            enemy.m_BoundsMin        = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax        = Util::GameFieldToScreen(352.0f, 144.0f);
            enemy.m_DeathCallbackSub = SUB_MEILING_DEATH;
            SetMeilingBossPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;

        default:
            break;
    }
}

void RunStage3Sub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;

    switch (enemy.m_SubId) {
        case 0:
        case 1:
            RunOpeningFairy(enemy, t);
            break;

        case 2:
        case 3:
            RunSideFairy(enemy, t);
            break;

        case 4:
            RunMediumKunai(enemy, ctx, t);
            break;

        case 5:
            RunMediumRing(enemy, ctx, t);
            break;

        case 6:
            RunBlueScatter(enemy, ctx, t);
            break;

        case 7:
            RunFixedDownKunaiFairy(enemy, ctx, t);
            break;

        case 8:
            RunWhiteRandomBalls(enemy, ctx, t);
            break;

        case SUB_MEILING_MIDBOSS_MAIN:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
            if (t == 100) enemy.m_CanTakeDamage = true;
            if (t >= 130) RunMeilingMidbossPattern(enemy, ctx, t - 130);
            break;

        case SUB_MEILING_MIDBOSS_SPELL_A:
        case SUB_MEILING_MIDBOSS_SPELL_B:
            RunMeilingMidbossSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_MIDBOSS_DEATH:
            if (t == 0) {
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Life);
                ctx.TransitionToSub(enemy, SUB_MEILING_MIDBOSS_ESCAPE);
            }
            break;

        case SUB_MEILING_MIDBOSS_ESCAPE:
            if (t == 0) {
                enemy.m_CanTakeDamage = false;
                enemy.m_ShowSpellName = false;
                enemy.m_InSpellcard   = false;
                ctx.BulletCancelIntoPointItems();
            }
            if (t == 130) {
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t == 190) enemy.m_Alive = false;
            break;

        case SUB_MEILING_SUPPORT_FAIRY:
            RunMeilingSupportFairy(enemy, ctx, t);
            break;

        case SUB_MEILING_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 150.0f, 100);
            if (t == 120) ctx.TransitionToSub(enemy, SUB_MEILING_FIRST_NONSPELL);
            break;

        case SUB_MEILING_FIRST_NONSPELL:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 14000, 2, 1800, SUB_MEILING_RAINBOW_WIND_CHIME,
                                   SUB_MEILING_SECOND_NONSPELL);
                enemy.m_LifeCallbackThreshold = 1900;
                enemy.m_LifeCallbackSub       = SUB_MEILING_RAINBOW_WIND_CHIME;
                enemy.m_CanTakeDamage         = true;
            }
            if (t >= 50) RunMeilingFirstNonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_RAINBOW_WIND_CHIME:
            if (t == 0) {
                StartSpellPhase(enemy, ctx, "Rainbow Wind Chime", 2, 1800,
                                SUB_MEILING_SECOND_NONSPELL);
                enemy.m_LockedShotAngle = 0.87266463f;
            }
            RunRainbowWindChimeSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_SECOND_NONSPELL:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 12000, 1, 2400, SUB_MEILING_FINAL_NONSPELL,
                                   SUB_MEILING_FINAL_NONSPELL);
                enemy.m_LifeCallbackThreshold = -1;
                enemy.m_LifeCallbackSub       = -1;
                ScriptUtil::DropPowerItems(enemy, ctx, 5);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 160) RunMeilingSecondNonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_FINAL_NONSPELL:
            if (t == 0) {
                StartNonSpellPhase(enemy, ctx, 16500, 0, 2400, SUB_MEILING_COLORFUL_RAIN_A,
                                   SUB_MEILING_DEATH);
                enemy.m_LifeCallbackThreshold = 3400;
                enemy.m_LifeCallbackSub       = SUB_MEILING_COLORFUL_RAIN_A;
                ScriptUtil::DropPowerItems(enemy, ctx, 5);
                enemy.m_CanTakeDamage = true;
            }
            if (t >= 160) RunMeilingFinalNonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_COLORFUL_RAIN_A:
        case SUB_MEILING_COLORFUL_RAIN_B:
            if (t == 0) {
                StartSpellPhase(enemy, ctx, "Colorful Rain", 0, 2160, SUB_MEILING_EXTREME_TYPHOON);
                enemy.m_LifeCallbackThreshold = 2000;
                enemy.m_LifeCallbackSub       = SUB_MEILING_EXTREME_TYPHOON;
            }
            RunColorfulRainSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_EXTREME_TYPHOON:
            RunGorgeousTyphoonSpell(enemy, ctx, t);
            break;

        case SUB_MEILING_DEATH:
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

}  // namespace Stage3Detail
