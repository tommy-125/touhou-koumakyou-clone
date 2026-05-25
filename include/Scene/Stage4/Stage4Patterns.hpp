#pragma once

class Enemy;
struct EnemySubCtx;

namespace Stage4Detail {
constexpr float PI = 3.14159265f;

constexpr int SUB_LIBRARY_FAIRY_BURST       = 1;
constexpr int SUB_KOAKUMA_MIDBOSS           = 21;
constexpr int SUB_KOAKUMA_DEATH             = 24;
constexpr int SUB_KOAKUMA_ESCAPE            = 25;
constexpr int SUB_PATCHOULI_ENTRY           = 26;
constexpr int SUB_PATCHOULI_FIRST_NONSPELL  = 27;
constexpr int SUB_PATCHOULI_SECOND_NONSPELL = 37;
constexpr int SUB_PATCHOULI_FINAL_NONSPELL  = 39;
constexpr int SUB_PATCHOULI_PHASE_OUT       = 40;
constexpr int SUB_PATCHOULI_PRINCESS_UNDINE = 45;
constexpr int SUB_PATCHOULI_SYLPHY_HORN_ADV = 48;
constexpr int SUB_PATCHOULI_WATER_ELF       = 56;
constexpr int SUB_PATCHOULI_DEATH           = 60;

void SetPatchouliBossPoses(Enemy& enemy);
void InitStage4BookFairy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage4SideBookFairy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage4BurstFamiliar(Enemy& enemy, EnemySubCtx& ctx);
void InitStage4SmallFairyEnemy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage4RingCaster(Enemy& enemy, EnemySubCtx& ctx);
void InitStage4DropProxy(Enemy& enemy, EnemySubCtx& ctx);
void InitStage4CrystalFairy(Enemy& enemy, EnemySubCtx& ctx);
void InitKoakumaMidboss(Enemy& enemy, EnemySubCtx& ctx);
void InitPatchouliEntry(Enemy& enemy, EnemySubCtx& ctx);
void RunStage4SmallFairy(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunBookFairy(Enemy& enemy, EnemySubCtx& ctx, int t, int burstSub);
void RunBurstFamiliar(Enemy& enemy, EnemySubCtx& ctx, int t, bool laser, bool denseKunai);
void RunRingCaster(Enemy& enemy, EnemySubCtx& ctx, int t, bool sideEntry);
void RunCrystalFairy(Enemy& enemy, EnemySubCtx& ctx, int t, int maxVolleys, bool green,
                     bool largeBlue);
void RunKoakumaMidboss(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunPatchouliFirstNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunPatchouliSecondNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunPrincessUndine(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunSylphyHornAdvanced(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunFinalNonSpell(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunMercuryPoison(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunWaterElf(Enemy& enemy, EnemySubCtx& ctx, int t);
}  // namespace Stage4Detail
