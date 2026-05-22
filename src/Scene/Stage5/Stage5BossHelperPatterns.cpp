#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyPatternUtil.hpp"
#include "Scene/Stage5/Stage5PatternCommon.hpp"
#include "Util/Math.hpp"

namespace Stage5Detail {
using EnemyPatternUtil::RandAngle;
using EnemyPatternUtil::SpawnFanAbsolute;

struct HelperPattern {
    float        moveAngle = 0.0f;
    EBulletType  type      = EBulletType::Rice;
    EBulletColor color     = EBulletColor::Blue;
    int          ways      = 5;
    int          stacks    = 1;
    float        speed1    = 1.6f;
    float        speed2    = 1.2f;
    float        spread    = 0.02617994f;
    float        angleInc  = 0.31415927f;
    int          interval  = 6;
    int          shots     = 40;
};

HelperPattern GetSakuyaHelperPattern(int subId) {
    switch (subId) {
        case SUB_SAKUYA_HELPER_1:
            return {0.7853982f, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f,
                    1.2f,       0.02617994f,       0.62831855f,        6, 40};
        case SUB_SAKUYA_HELPER_2:
            return {0.0f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f,
                    1.2f, 0.02617994f,       -0.31415927f,        6, 40};
        case SUB_SAKUYA_HELPER_3:
            return {2.3561945f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f,
                    1.2f,       0.02617994f,       -0.62831855f,        6, 40};
        case SUB_SAKUYA_HELPER_4:
            return {PI,   EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f,
                    1.2f, 0.02617994f,       0.31415927f,        6, 40};
        case SUB_SAKUYA_HELPER_5:
            return {1.1780972f, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f,
                    1.2f,       0.02617994f,       0.62831855f,        6, 40};
        case SUB_SAKUYA_HELPER_6:
            return {1.9634954f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f,
                    1.2f,       0.02617994f,       -0.62831855f,        6, 40};
        case SUB_SAKUYA_HELPER_7:
            return {0.3926991f, EBulletType::Rice, EBulletColor::Green, 5, 1, 1.6f,
                    1.2f,       0.02617994f,       -0.31415927f,        6, 40};
        case SUB_SAKUYA_HELPER_8:
            return {2.7488935f, EBulletType::Rice, EBulletColor::Blue, 5, 1, 1.6f,
                    1.2f,       0.02617994f,       0.31415927f,        6, 40};
        case SUB_SAKUYA_KUNAI_HELPER_1:
            return {0.7853982f, EBulletType::Kunai, EBulletColor::Red, 6,  1, 1.6f,
                    1.2f,       0.015707964f,       0.62831855f,       14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_2:
            return {0.0f, EBulletType::Kunai, EBulletColor::Blue, 6,  1, 1.6f,
                    1.2f, 0.015707964f,       -0.19634955f,       14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_3:
            return {2.3561945f, EBulletType::Kunai, EBulletColor::Blue, 6,  1, 1.6f,
                    1.2f,       0.015707964f,       -0.62831855f,       14, 17};
        case SUB_SAKUYA_KUNAI_HELPER_4:
            return {PI,   EBulletType::Kunai, EBulletColor::Red, 6,  1, 1.6f,
                    1.2f, 0.015707964f,       0.19634955f,       14, 17};
        default:
            return {};
    }
}

void InitSakuyaHelper(Enemy& enemy, const HelperPattern& pattern) {
    enemy.m_Angle            = pattern.moveAngle;
    enemy.m_Speed            = 0.8f;
    enemy.m_Acceleration     = -0.8f / 370.0f;
    enemy.m_LockedShotAngle  = RandAngle();
    enemy.m_CanTakeDamage    = false;
    enemy.m_HitboxSize       = {0.0f, 0.0f};
    enemy.m_ItemDropCount    = 0;
    enemy.m_DeathCallbackSub = SUB_SAKUYA_HELPER_DEATH;
}

void InitSakuyaHelper(Enemy& enemy) {
    InitSakuyaHelper(enemy, GetSakuyaHelperPattern(enemy.m_SubId));
}

void RunSakuyaHelper(Enemy& enemy, EnemySubCtx& ctx, int t) {
    const HelperPattern pattern   = GetSakuyaHelperPattern(enemy.m_SubId);
    const int           fireStart = 50;
    const int           fireEnd   = fireStart + pattern.interval * pattern.shots;
    if (t >= fireStart && t < fireEnd && (t - fireStart) % pattern.interval == 0) {
        const int shot = (t - fireStart) / pattern.interval;
        SpawnFanAbsolute(ctx, enemy.m_Pos, pattern.type, pattern.color, pattern.ways,
                         pattern.stacks, RankedSpeed1(pattern.speed1), RankedSpeed2(pattern.speed2),
                         enemy.m_LockedShotAngle + pattern.angleInc * static_cast<float>(shot),
                         pattern.spread, pattern.type == EBulletType::Kunai);
    }
    if (t >= fireEnd + 60) enemy.m_Alive = false;
}

}  // namespace Stage5Detail