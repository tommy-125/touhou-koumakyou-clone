#pragma once

#include <glm/glm.hpp>

#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Util/Math.hpp"

namespace StageScriptUtil {
struct VisualConfig {
    int       anmBase = 0;
    int       script  = 0;
    glm::vec2 hitbox  = {0.0f, 0.0f};
    int       deathEffectPrimary   = -1;
    int       deathEffectSecondary = -1;
};

inline void InitVisual(Enemy& enemy, EnemySubCtx& ctx, const VisualConfig& config) {
    ctx.anm.SetScript(enemy.m_Vm, config.anmBase + config.script, config.anmBase);
    enemy.m_HitboxSize = config.hitbox;
    if (config.deathEffectPrimary >= 0 && config.deathEffectSecondary >= 0) {
        EnemyScriptUtil::SetDeathEffects(enemy, config.deathEffectPrimary,
                                         config.deathEffectSecondary);
    }
}

inline void SetDropCallback(Enemy& enemy, int callbackSub) {
    enemy.m_DeathCallbackSub = callbackSub;
    enemy.m_ItemDropCount    = 0;
}

inline void InitDropProxy(Enemy& enemy, EnemySubCtx& ctx, int anmBase, int script) {
    InitVisual(enemy, ctx, {anmBase, script, {0.0f, 0.0f}});
    enemy.m_CanTakeDamage = false;
    enemy.m_ItemDropCount = 0;
}

struct BossEntryConfig {
    int         anmBase = 0;
    int         script  = 0;
    glm::vec2   fieldPos = {0.0f, 0.0f};
    glm::vec2   hitbox   = {0.0f, 0.0f};
    const char* title    = "";
    bool        blocksTimeline = true;
    int         lifeCount = 0;
    glm::vec2   boundsMinField = {32.0f, 48.0f};
    glm::vec2   boundsMaxField = {352.0f, 120.0f};
    int         deathEffectPrimary   = 671;
    int         deathEffectSecondary = 676;
};

inline void InitBossEntry(Enemy& enemy, EnemySubCtx& ctx, const BossEntryConfig& config) {
    InitVisual(enemy, ctx,
               {config.anmBase, config.script, config.hitbox, config.deathEffectPrimary,
                config.deathEffectSecondary});
    enemy.m_Pos            = Util::GameFieldToScreen(config.fieldPos);
    enemy.m_IsBoss         = true;
    enemy.m_BlocksTimeline = config.blocksTimeline;
    enemy.m_CanTakeDamage  = false;
    enemy.m_ItemDropCount  = 0;
    enemy.m_BossTitle      = config.title;
    enemy.m_BossLifeCount  = config.lifeCount;
    enemy.m_BoundsMin      = Util::GameFieldToScreen(config.boundsMinField);
    enemy.m_BoundsMax      = Util::GameFieldToScreen(config.boundsMaxField);
}

inline void DropPowerAndDie(Enemy& enemy, EnemySubCtx& ctx, int count) {
    EnemyScriptUtil::DropPowerItems(enemy, ctx, count);
    enemy.m_Alive = false;
}

inline void HideBossForExit(Enemy& enemy) {
    enemy.m_IsBoss        = false;
    enemy.m_CanTakeDamage = false;
    enemy.m_InSpellcard   = false;
    enemy.m_ShowSpellName = false;
    enemy.m_HitboxSize    = {0.0f, 0.0f};
}

inline void FinishBossDeath(Enemy& enemy, EnemySubCtx& ctx) {
    ctx.SetTimeStopped(false);
    enemy.m_CanTakeDamage = false;
    enemy.m_InSpellcard   = false;
    enemy.m_ShowSpellName = false;
    ctx.BulletCancelIntoPointItems();
    enemy.m_Alive = false;
}
}  // namespace StageScriptUtil
