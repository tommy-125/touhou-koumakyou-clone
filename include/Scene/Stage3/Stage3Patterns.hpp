#pragma once

class Enemy;
struct EnemySubCtx;

namespace Stage3Detail {
constexpr float PI = 3.14159265f;

constexpr int SUB_MEILING_MIDBOSS_MAIN       = 9;
constexpr int SUB_MEILING_MIDBOSS_SPELL_A    = 13;
constexpr int SUB_MEILING_MIDBOSS_SPELL_B    = 14;
constexpr int SUB_MEILING_MIDBOSS_DEATH      = 15;
constexpr int SUB_MEILING_MIDBOSS_ESCAPE     = 16;
constexpr int SUB_MEILING_SUPPORT_FAIRY      = 25;
constexpr int SUB_MEILING_ENTRY              = 17;
constexpr int SUB_MEILING_FIRST_NONSPELL     = 18;
constexpr int SUB_MEILING_RAINBOW_WIND_CHIME = 29;
constexpr int SUB_MEILING_SECOND_NONSPELL    = 20;
constexpr int SUB_MEILING_FINAL_NONSPELL     = 26;
constexpr int SUB_MEILING_COLORFUL_RAIN_A    = 31;
constexpr int SUB_MEILING_COLORFUL_RAIN_B    = 32;
constexpr int SUB_MEILING_EXTREME_TYPHOON    = 33;
constexpr int SUB_MEILING_DEATH              = 34;

void SetMeilingBossPoses(Enemy& enemy);
void StartNonSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, int life, int lifeCount,
                        int timerFrames, int nextSub, int deathSub);
void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int lifeCount,
                     int timerFrames, int nextSub);
void RunOpeningFairy(Enemy& enemy, int t);
void RunSideFairy(Enemy& enemy, int t);
void RunMediumKunai(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMediumRing(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunBlueScatter(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunFixedDownKunaiFairy(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunWhiteRandomBalls(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMeilingMidbossPattern(Enemy& enemy, EnemySubCtx& ctx, int frame);
void RunMeilingMidbossSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMeilingSupportFairy(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMeilingFirstNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunRainbowWindChimeSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMeilingSecondNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMeilingFinalNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunColorfulRainSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunGorgeousTyphoonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void InitStage3Sub(Enemy& enemy, EnemySubCtx& ctx);
void RunStage3Sub(Enemy& enemy, EnemySubCtx& ctx);
}  // namespace Stage3Detail