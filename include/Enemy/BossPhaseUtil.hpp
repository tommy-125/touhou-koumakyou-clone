#ifndef ENEMY_BOSS_PHASE_UTIL_HPP
#define ENEMY_BOSS_PHASE_UTIL_HPP

#include <string>

#include "Enemy/Enemy.hpp"
#include "Enemy/EnemySubCtx.hpp"

namespace BossPhaseUtil {

struct PhaseConfig {
    std::string title;
    int life = -1;
    int lifeCount = 0;
    int timerFrames = -1;
    int timerSub = -1;
    int deathSub = -1;
    int lifeThreshold = -1;
    int lifeSub = -1;
    bool spell = false;
    bool showSpellName = false;
    int spellBonus = 0;
    bool canTakeDamage = false;
    bool cancelBullets = true;
};

void StartPhase(Enemy& enemy, const EnemySubCtx& ctx, const PhaseConfig& config);

}  // namespace BossPhaseUtil

#endif  // ENEMY_BOSS_PHASE_UTIL_HPP
