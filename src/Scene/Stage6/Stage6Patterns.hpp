#pragma once

class Enemy;
struct EnemySubCtx;

namespace Stage6Detail {
constexpr float PI     = 3.14159265f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float REMILIA_RANK_SPEED = 0.35f;

constexpr int SUB_STAGE_EFFECTS       = 0;
constexpr int SUB_FAIRY_ARC_BLUE_L    = 1;
constexpr int SUB_FAIRY_ARC_BLUE_R    = 2;
constexpr int SUB_FAIRY_ARC_RED_L     = 3;
constexpr int SUB_FAIRY_ARC_RED_R     = 4;
constexpr int SUB_FAIRY_SIDE_BURST    = 5;
constexpr int SUB_FAIRY_DROP_8        = 6;
constexpr int SUB_FAIRY_TOP_BURST     = 7;

constexpr int SUB_SAKUYA_ENTRY        = 8;
constexpr int SUB_SAKUYA_MAIN         = 9;
constexpr int SUB_SAKUYA_DEATH        = 12;
constexpr int SUB_SAKUYA_EXIT         = 13;
constexpr int SUB_SAKUYA_ETERNAL_MEEK = 14;

constexpr int SUB_REMILIA_ENTRY       = 15;
constexpr int SUB_REMILIA_WAIT        = 16;
constexpr int SUB_REMILIA_NONSPELL_1  = 17;
constexpr int SUB_REMILIA_NONSPELL_2  = 19;
constexpr int SUB_REMILIA_NONSPELL_3  = 23;
constexpr int SUB_REMILIA_NONSPELL_4  = 26;
constexpr int SUB_REMILIA_AFTERIMAGE  = 29;
constexpr int SUB_REMILIA_STAR        = 30;
constexpr int SUB_REMILIA_SCARLET     = 32;
constexpr int SUB_REMILIA_VLAD        = 34;
constexpr int SUB_REMILIA_SHOOT       = 38;
constexpr int SUB_REMILIA_RED_MAGIC   = 43;
constexpr int SUB_REMILIA_DEATH       = 45;

float RandFloat(float min, float max);
void SetSakuyaPoses(Enemy& enemy);
void SetRemiliaPoses(Enemy& enemy);

void RunArcFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool red, bool highArc);
void RunBurstFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool top);
void RunSakuyaMain(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunEternalMeek(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunRemiliaNonSpell1(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunRemiliaNonSpell2(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunRemiliaNonSpell3(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunRemiliaNonSpell4(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunStarOfDavid(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunScarletNetherworld(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunVlad(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunScarletShoot(Enemy& enemy, EnemySubCtx& ctx, int t);
void RunRedMagic(Enemy& enemy, EnemySubCtx& ctx, int t);
}  // namespace Stage6Detail