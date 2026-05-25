#pragma once

class Enemy;
struct EnemySubCtx;

namespace Stage1Detail {
constexpr int SUB_MIDBOSS_MAIN         = 8;
constexpr int SUB_MIDBOSS_ESCAPE       = 7;
constexpr int SUB_MIDBOSS_DEATH        = 6;
constexpr int SUB_BOSS_ENTRY           = 10;
constexpr int SUB_BOSS_PHASE1_INIT     = 11;
constexpr int SUB_BOSS_PHASE1_ATTACK_A = 12;
constexpr int SUB_BOSS_PHASE1_ATTACK_B = 13;
constexpr int SUB_BOSS_PHASE1_ATTACK_C = 14;
constexpr int SUB_BOSS_PHASE1_ATTACK_D = 15;
constexpr int SUB_BOSS_PHASE1_SPELL    = 22;
constexpr int SUB_BOSS_PHASE2_INIT     = 16;
constexpr int SUB_BOSS_PHASE2_ATTACK_A = 18;
constexpr int SUB_BOSS_PHASE2_ATTACK_B = 19;
constexpr int SUB_BOSS_PHASE2_ATTACK_C = 20;
constexpr int SUB_BOSS_PHASE2_ATTACK_D = 21;
constexpr int SUB_BOSS_PHASE2_SPELL    = 23;
constexpr int SUB_BOSS_DEATH           = 17;

void InitStage1SmallFairy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage1MediumFairy(Enemy& enemy, EnemySubCtx& ctx);
void RunStage1SmallFairyA(Enemy& enemy, int t);
void RunStage1SmallFairyB(Enemy& enemy, int t);
void RunStage1MediumFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool shoots);
void InitRumiaMidbossSub(Enemy& enemy, EnemySubCtx& ctx);
void RunRumiaMidbossSub(Enemy& enemy, EnemySubCtx& ctx, int t);
void InitRumiaBossSub(Enemy& enemy, EnemySubCtx& ctx);
void RunRumiaBossSub(Enemy& enemy, EnemySubCtx& ctx, int t);
}  // namespace Stage1Detail
