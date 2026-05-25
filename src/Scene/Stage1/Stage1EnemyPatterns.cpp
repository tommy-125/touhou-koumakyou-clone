#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyLaserManager.hpp"
#include "Item/ItemManager.hpp"
#include "Scene/Stage1/Stage1Patterns.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Util/Math.hpp"

namespace Stage1Detail {
namespace {
namespace StageUtil = StageScriptUtil;

}  // namespace
void InitStage1SmallFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage1SmallFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void InitStage1MediumFairy(Enemy& enemy, EnemySubCtx& ctx) {
    StageUtil::EnemyInitConfig config = StageUtil::LoadEnemyInitConfig(StageUtil::ConfigId::EnemyInit::Stage1MediumFairy);
    StageUtil::InitEnemy(enemy, ctx, config);
}

void RunStage1SmallFairyA(Enemy& enemy, int t) {
    const float dir = enemy.m_Mirrored ? -1.0f : 1.0f;
    if (t == 40) enemy.m_AngularVelocity = dir * -0.024543693f;
    if (t == 120) enemy.m_AngularVelocity = dir * 0.019634955f;
    if (t == 220) enemy.m_AngularVelocity = 0.0f;
    if (t >= 10000) enemy.m_Alive = false;
}

void RunStage1SmallFairyB(Enemy& enemy, int t) {
    const float dir = enemy.m_Mirrored ? -1.0f : 1.0f;
    if (t == 100) enemy.m_AngularVelocity = dir * 0.019634955f;
    if (t == 200) enemy.m_AngularVelocity = 0.0f;
    if (t >= 10000) enemy.m_Alive = false;
}

void RunStage1MediumFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool shoots) {
    const int   offset = Anm::STG1ENM.offset;
    const float dir    = enemy.m_Mirrored ? -1.0f : 1.0f;
    if (t == 60) {
        ctx.anm.SetScript(enemy.m_Vm, offset + 5, offset);
        enemy.m_Speed = 0.0f;
    }
    if (shoots && t == 70) {
        glm::vec2 shootPos = enemy.m_Pos + glm::vec2{12.0f, -12.0f};
        ctx.bullets.SpawnFanAimed(shootPos, ctx.playerPos, EBulletType::RingBall, EBulletColor::Red,
                                  7, 1.4f, 0.0f, 0.62831855f, true);
    }
    if (t == 130) {
        enemy.m_Acceleration    = 0.05f;
        enemy.m_AngularVelocity = dir * 0.05235988f;
    }
    if (t == 190) enemy.m_AngularVelocity = 0.0f;
    if (t >= 10000) enemy.m_Alive = false;
}

}  // namespace Stage1Detail
