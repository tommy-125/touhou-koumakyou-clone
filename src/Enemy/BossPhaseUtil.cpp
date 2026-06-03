#include "Enemy/BossPhaseUtil.hpp"

#include <algorithm>

#include "Audio/AudioManager.hpp"

namespace BossPhaseUtil {

void StartPhase(Enemy& enemy, const EnemySubCtx& ctx, const PhaseConfig& config) {
    enemy.m_BossTitle              = config.title;
    enemy.m_BossLifeCount          = config.lifeCount;
    enemy.m_BossTimer              = 0;
    enemy.m_TimerCallbackThreshold = config.timerFrames;
    enemy.m_TimerCallbackSub =
        config.timerSub >= 0 ? config.timerSub : config.deathSub;
    enemy.m_LifeCallbackThreshold = config.lifeThreshold;
    enemy.m_LifeCallbackSub       = config.lifeSub;
    enemy.m_DeathCallbackSub      = config.deathSub;
    enemy.m_InSpellcard           = config.spell;
    enemy.m_ShowSpellName         = config.showSpellName;
    enemy.m_SpellcardBonus        = config.spellBonus;
    enemy.m_CanTakeDamage         = config.canTakeDamage;

    if (config.life >= 0) {
        enemy.m_Life        = config.life;
        enemy.m_BossMaxLife = std::max(1, config.life);
    }

    if (config.cancelBullets) {
        ctx.BulletCancelIntoPointItems();
    }
    if (config.spell) {
        AudioManager::Instance().Play(SoundEffect::SpellStart);
    }
}

}  // namespace BossPhaseUtil
