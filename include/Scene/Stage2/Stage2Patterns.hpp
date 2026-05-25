#pragma once

#include <glm/vec2.hpp>

#include "Enemy/EnemyBulletManager.hpp"

class Enemy;
struct EnemySubCtx;

namespace Stage2Detail {
constexpr float     PI                 = 3.14159265f;
constexpr glm::vec2 CIRNO_SHOOT_OFFSET = {0.0f, -12.0f};

constexpr int SUB_DAIYOUSEI_MAIN           = 20;
constexpr int SUB_DAIYOUSEI_DEATH          = 18;
constexpr int SUB_DAIYOUSEI_ESCAPE         = 19;
constexpr int SUB_CIRNO_ENTRY              = 21;
constexpr int SUB_CIRNO_NONSPELL_INIT      = 22;
constexpr int SUB_CIRNO_NONSPELL_ATTACK_A  = 23;
constexpr int SUB_CIRNO_NONSPELL_ATTACK_B  = 24;
constexpr int SUB_CIRNO_ICICLE_FALL        = 30;
constexpr int SUB_CIRNO_PHASE2_INIT        = 25;
constexpr int SUB_CIRNO_PREFREEZE_ATTACK_A = 26;
constexpr int SUB_CIRNO_PREFREEZE_ATTACK_B = 27;
constexpr int SUB_CIRNO_PERFECT_FREEZE     = 31;
constexpr int SUB_CIRNO_DIAMOND_BLIZZARD   = 32;
constexpr int SUB_CIRNO_DEATH              = 28;

void StartSpellPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* phaseId);
void SpawnOneWayStackWithCurve(glm::vec2 pos, EnemySubCtx& ctx, EBulletType type,
                               EBulletColor color, int stacks, float speed1, float speed2,
                               float baseAngle, BulletCurve curve, bool rotateWithAngle = false);
EBulletColor PerfectFreezeColor(int idx);
void         SpawnAtRandomArea(Enemy& enemy, EnemySubCtx& ctx, float width, int count);
void         InitStage2AngledFairy(Enemy& enemy, EnemySubCtx& ctx);
void         InitStage2AimedFairy(Enemy& enemy, EnemySubCtx& ctx);
void         InitStage2RandomFairy(Enemy& enemy, EnemySubCtx& ctx);
void         InitStage2ShardFairy(Enemy& enemy, EnemySubCtx& ctx);
void         InitStage2MediumFairy(Enemy& enemy, EnemySubCtx& ctx);
void         RunStage2AngledFairy(Enemy& enemy, EnemySubCtx& ctx, int t);
void         RunStage2DeathBurst(Enemy& enemy, EnemySubCtx& ctx, int t);
void         RunStage2AimedFairy(Enemy& enemy, int t);
void         RunStage2TimedDespawn(Enemy& enemy, int t);
void         RunStage2MediumFairy(Enemy& enemy, EnemySubCtx& ctx, int t);
void         InitDaiyouseiSub(Enemy& enemy, EnemySubCtx& ctx);
void         RunDaiyouseiSub(Enemy& enemy, EnemySubCtx& ctx, int t);
void         InitCirnoSub(Enemy& enemy, EnemySubCtx& ctx);
void         RunCirnoSub(Enemy& enemy, EnemySubCtx& ctx, int t);
}  // namespace Stage2Detail
