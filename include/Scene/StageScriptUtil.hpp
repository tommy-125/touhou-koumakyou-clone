#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Enemy/EnemySubCtx.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/StageConfigIds.hpp"
#include "Util/Math.hpp"

namespace StageScriptUtil {
struct VisualConfig {
    int       anmBase              = 0;
    int       script               = 0;
    glm::vec2 hitbox               = {0.0f, 0.0f};
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

struct EnemyInitConfig {
    EnemyInitConfig() = default;
    explicit EnemyInitConfig(VisualConfig visualConfig) : visual(visualConfig) {}

    VisualConfig         visual;
    std::optional<float> angle;
    std::optional<float> speed;
    std::optional<float> acceleration;
    std::optional<float> angularVelocity;
    std::optional<int>   itemDrop;
    std::optional<int>   itemDropCount;
    std::optional<int>   deathCallbackSub;
    std::optional<bool>  canTakeDamage;
    std::optional<bool>  rotateWithAngle;
};

inline void InitEnemy(Enemy& enemy, EnemySubCtx& ctx, const EnemyInitConfig& config) {
    InitVisual(enemy, ctx, config.visual);
    if (config.angle) enemy.m_Angle = *config.angle;
    if (config.speed) enemy.m_Speed = *config.speed;
    if (config.acceleration) enemy.m_Acceleration = *config.acceleration;
    if (config.angularVelocity) enemy.m_AngularVelocity = *config.angularVelocity;
    if (config.itemDrop) enemy.m_ItemDrop = *config.itemDrop;
    if (config.itemDropCount) enemy.m_ItemDropCount = *config.itemDropCount;
    if (config.deathCallbackSub) enemy.m_DeathCallbackSub = *config.deathCallbackSub;
    if (config.canTakeDamage) enemy.m_CanTakeDamage = *config.canTakeDamage;
    if (config.rotateWithAngle) enemy.m_RotateWithAngle = *config.rotateWithAngle;
}

EnemyInitConfig LoadEnemyInitConfig(const std::string& id);

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
    int                anmBase  = 0;
    int                script   = 0;
    glm::vec2          fieldPos = {0.0f, 0.0f};
    glm::vec2          hitbox   = {0.0f, 0.0f};
    std::string        title;
    bool               blocksTimeline       = true;
    int                lifeCount            = 0;
    glm::vec2          boundsMinField       = {32.0f, 48.0f};
    glm::vec2          boundsMaxField       = {352.0f, 120.0f};
    int                deathEffectPrimary   = 671;
    int                deathEffectSecondary = 676;
    std::optional<int> itemDrop;
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
    if (config.itemDrop) enemy.m_ItemDrop = *config.itemDrop;
    enemy.m_BossTitle     = config.title;
    enemy.m_BossLifeCount = config.lifeCount;
    enemy.m_BoundsMin     = Util::GameFieldToScreen(config.boundsMinField);
    enemy.m_BoundsMax     = Util::GameFieldToScreen(config.boundsMaxField);
}

BossEntryConfig            LoadBossEntryConfig(const std::string& id);
BossPhaseUtil::PhaseConfig LoadBossPhaseConfig(const std::string& id);

inline void StartBossPhase(Enemy& enemy, const EnemySubCtx& ctx, const std::string& id) {
    BossPhaseUtil::StartPhase(enemy, ctx, LoadBossPhaseConfig(id));
}

struct MovementEvent {
    int                  time = 0;
    std::optional<float> angle;
    std::optional<float> mirroredAngle;
    std::optional<float> speed;
    std::optional<float> acceleration;
    std::optional<float> angularVelocity;
    std::optional<float> mirroredAngularVelocity;
};

struct MovementProfile {
    std::vector<MovementEvent> events;
};

MovementProfile LoadMovementProfile(const std::string& id);
void            ApplyMovementProfile(Enemy& enemy, const MovementProfile& profile, int time);
void            ApplyMovementProfile(Enemy& enemy, const std::string& id, int time);

struct RewardConfig {
    int                   power = 0;
    std::vector<ItemType> items;
    bool                  bulletCancelToPoints = false;
    bool                  die                  = false;
};

RewardConfig LoadRewardConfig(const std::string& id);
void         ApplyReward(Enemy& enemy, EnemySubCtx& ctx, const RewardConfig& config);
void         ApplyReward(Enemy& enemy, EnemySubCtx& ctx, const std::string& id);

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
