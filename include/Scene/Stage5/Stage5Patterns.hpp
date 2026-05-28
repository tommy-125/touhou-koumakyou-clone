#ifndef SCENE_STAGE5_STAGE5_PATTERNS_HPP
#define SCENE_STAGE5_STAGE5_PATTERNS_HPP

#include "Enemy/EnemyBulletManager.hpp"

class Enemy;
struct EnemySubCtx;

namespace Stage5Detail {
constexpr float PI              = 3.14159265f;
constexpr int   SUB_MAID_DROP_4 = 7;
constexpr int   SUB_MAID_DROP_6 = 8;

constexpr int SUB_SAKUYA_MIDBOSS_ENTRY = 12;
constexpr int SUB_SAKUYA_MIDBOSS_MAIN  = 13;
constexpr int SUB_SAKUYA_MIDBOSS_DEATH = 16;
constexpr int SUB_SAKUYA_MIDBOSS_EXIT  = 17;
constexpr int SUB_SAKUYA_MISDIRECTION  = 18;

constexpr int SUB_SAKUYA_ENTRY           = 21;
constexpr int SUB_SAKUYA_FIRST_NONSPELL  = 22;
constexpr int SUB_SAKUYA_HELPER_1        = 25;
constexpr int SUB_SAKUYA_HELPER_2        = 26;
constexpr int SUB_SAKUYA_HELPER_3        = 27;
constexpr int SUB_SAKUYA_HELPER_4        = 28;
constexpr int SUB_SAKUYA_HELPER_5        = 29;
constexpr int SUB_SAKUYA_HELPER_6        = 30;
constexpr int SUB_SAKUYA_HELPER_7        = 31;
constexpr int SUB_SAKUYA_HELPER_8        = 32;
constexpr int SUB_SAKUYA_HELPER_DEATH    = 33;
constexpr int SUB_SAKUYA_SECOND_NONSPELL = 34;
constexpr int SUB_SAKUYA_KUNAI_HELPER_1  = 36;
constexpr int SUB_SAKUYA_KUNAI_HELPER_2  = 37;
constexpr int SUB_SAKUYA_KUNAI_HELPER_3  = 38;
constexpr int SUB_SAKUYA_KUNAI_HELPER_4  = 39;
constexpr int SUB_SAKUYA_FINAL_NONSPELL  = 41;
constexpr int SUB_SAKUYA_CLOCK_CORPSE    = 44;
constexpr int SUB_SAKUYA_LUNA_CLOCK      = 46;
constexpr int SUB_SAKUYA_FINAL_SPELL     = 50;
constexpr int SUB_SAKUYA_DEATH           = 51;

constexpr float SAKUYA_FINAL_RANK_SPEED = 0.25f;

void SetSakuyaBossPoses(Enemy& enemy);
void InitSakuyaHelper(Enemy& enemy);
void InitStage5TopMaid(Enemy& enemy, EnemySubCtx& ctx);
void InitStage5SideMaid(Enemy& enemy, EnemySubCtx& ctx);
void InitStage5RingMaid(Enemy& enemy, EnemySubCtx& ctx);
void InitStage5MaidDropProxy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage5SakuyaHelperProxy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage5SakuyaHelperDeath(Enemy& enemy, EnemySubCtx& ctx);
void InitSakuyaEntry(Enemy& enemy, EnemySubCtx& ctx);

void RunSakuyaHelper(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunTopMaid(Enemy& enemy, EnemySubCtx& ctx, int t, EBulletColor color, bool dense,
                bool fastExit);
void RunSideMaid(Enemy& enemy, EnemySubCtx& ctx, int t, int variant);
void RunRingMaid(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMisdirection(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMidbossMain(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunSakuyaNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t, int phase);
void RunClockCorpse(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunLunaClock(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunFinalSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
}  // namespace Stage5Detail

#endif  // SCENE_STAGE5_STAGE5_PATTERNS_HPP
