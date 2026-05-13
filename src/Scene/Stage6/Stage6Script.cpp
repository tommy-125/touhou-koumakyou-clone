#include "Scene/Stage6/Stage6Script.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Enemy/BossPhaseUtil.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemyBulletManager.hpp"
#include "Enemy/EnemyScriptUtil.hpp"
#include "Item/ItemManager.hpp"
#include "Util/Math.hpp"

namespace {
constexpr float PI     = 3.14159265f;
constexpr float TWO_PI = PI * 2.0f;
namespace ScriptUtil = EnemyScriptUtil;

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
constexpr int SUB_REMILIA_STAR        = 30;
constexpr int SUB_REMILIA_SCARLET     = 32;
constexpr int SUB_REMILIA_VLAD        = 34;
constexpr int SUB_REMILIA_SHOOT       = 38;
constexpr int SUB_REMILIA_RED_MAGIC   = 43;
constexpr int SUB_REMILIA_DEATH       = 45;

float RandFloat(float min, float max) {
    return ScriptUtil::RandFloat(min, max);
}

float RandAngle() {
    return RandFloat(-PI, PI);
}

float AimAngle(glm::vec2 from, glm::vec2 to) {
    return std::atan2(to.y - from.y, to.x - from.x);
}

float MirrorAngle(float angle, bool mirrored) {
    return mirrored ? PI - angle : angle;
}

glm::vec2 ShootPos(const Enemy& enemy, glm::vec2 offset = {0.0f, -12.0f}) {
    return ScriptUtil::ShootPos(enemy, offset);
}

void SetSakuyaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 128, 131, 132, 129, 130);
}

void SetRemiliaPoses(Enemy& enemy) {
    ScriptUtil::SetBossPoses(enemy, 160, 163, 164, 161, 162);
}

void SpawnRandomArc(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                    int count, float minSpeed, float maxSpeed, float minAngle, float maxAngle,
                    bool rotate = false) {
    for (int i = 0; i < count; i++) {
        ctx.bullets.SpawnCircle(pos, type, color, 1, RandFloat(minSpeed, maxSpeed),
                                RandFloat(minAngle, maxAngle), false, 0.0f, 0, rotate);
    }
}

void SpawnFanAbs(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color, int ways,
                 int stacks, float speed1, float speed2, float baseAngle, float spread,
                 bool rotate = false) {
    for (int s = 0; s < stacks; s++) {
        const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
        const float speed = speed1 + (speed2 - speed1) * lerp;
        const float start = baseAngle - spread * static_cast<float>(ways - 1) * 0.5f;
        for (int i = 0; i < ways; i++) {
            ctx.bullets.SpawnCircle(pos, type, color, 1, speed, start + spread * i, false, 0.0f,
                                    0, rotate);
        }
    }
}

void SpawnAimedStack(EnemySubCtx& ctx, glm::vec2 pos, EBulletType type, EBulletColor color,
                     int count, int stacks, float speed1, float speed2, float spread = 0.0f,
                     float aimOffset = 0.0f, bool rotate = false) {
    if (spread == 0.0f) {
        for (int s = 0; s < stacks; s++) {
            const float lerp  = stacks <= 1 ? 0.0f : static_cast<float>(s) / (stacks - 1);
            const float speed = speed1 + (speed2 - speed1) * lerp;
            ctx.bullets.SpawnCircleAimed(pos, ctx.playerPos, type, color, count, speed,
                                         aimOffset, false, 0.0f, {}, rotate);
        }
        return;
    }
    SpawnFanAbs(ctx, pos, type, color, count, stacks, speed1, speed2,
                AimAngle(pos, ctx.playerPos) + aimOffset, spread, rotate);
}

void SpawnVampireBurst(EnemySubCtx& ctx, glm::vec2 pos, float baseAngle, bool dense) {
    ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::Gray, 1, 6.2f, baseAngle);
    SpawnRandomArc(ctx, pos, EBulletType::BigBall, EBulletColor::DarkRed, dense ? 5 : 3, 4.0f,
                   6.0f, baseAngle - 0.18f, baseAngle + 0.18f);
    SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, dense ? 10 : 6, 1.0f,
                   3.5f, baseAngle - 0.65f, baseAngle + 0.65f);
}

void StartRemiliaPhase(Enemy& enemy, const EnemySubCtx& ctx, const char* title, int life,
                       int lifeCount, int timerFrames, int nextSub, int lifeThreshold,
                       bool spell) {
    BossPhaseUtil::StartPhase(enemy, ctx,
                              {
                                  title,
                                  life,
                                  lifeCount,
                                  timerFrames,
                                  nextSub,
                                  nextSub,
                                  lifeThreshold,
                                  nextSub,
                                  spell,
                                  spell,
                                  0,
                                  false,
                                  true,
                              });
}

void RunArcFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool red, bool highArc) {
    if (t == 0) {
        enemy.m_Angle = MirrorAngle(highArc ? 0.5235988f : -1.0471976f, enemy.m_Mirrored);
        enemy.m_Speed = highArc ? 4.5f : 4.0f;
    }
    if (t == 30) enemy.m_AngularVelocity = (enemy.m_Mirrored ? -1.0f : 1.0f) *
                                           (highArc ? -0.06544985f : 0.034906585f);
    if (t == (highArc ? 115 : 90)) enemy.m_AngularVelocity = 0.0f;
    if (t % 50 == 0 && t <= 150) {
        SpawnRandomArc(ctx, ShootPos(enemy, {0.0f, 0.0f}), EBulletType::Rice,
                       red ? EBulletColor::Red : EBulletColor::Blue, red ? 9 : 6,
                       red ? 1.0f : 1.0f, red ? 2.0f : 1.6f, -PI, PI);
    }
}

void RunBurstFairy(Enemy& enemy, EnemySubCtx& ctx, int t, bool top) {
    if (t == 0) {
        enemy.m_Angle = top ? Util::HALF_PI : (enemy.m_Mirrored ? PI : 0.0f);
        enemy.m_Speed = 2.0f;
    }
    if (t == 40) {
        enemy.m_Acceleration = -0.06666667f;
        ctx.bullets.SpawnCircleAimed(ShootPos(enemy), ctx.playerPos, EBulletType::Rice,
                                     EBulletColor::Blue, top ? 60 : 60, top ? 1.6f : 2.4f);
    }
    if (t == 42) {
        enemy.m_Acceleration = 0.0f;
        enemy.m_Angle = RandFloat(0.7853982f, 2.3561945f);
        enemy.m_Speed = 1.8f;
    }
}

void RunSakuyaMain(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        BossPhaseUtil::StartPhase(enemy, ctx,
                                  {
                                      "Sakuya Izayoi",
                                      -1,
                                      0,
                                      1020,
                                      SUB_SAKUYA_EXIT,
                                      SUB_SAKUYA_DEATH,
                                      1000,
                                      SUB_SAKUYA_ETERNAL_MEEK,
                                      false,
                                      false,
                                      0,
                                      false,
                                      true,
                                  });
        enemy.m_CanTakeDamage = true;
        ctx.SetTimeStopped(false);
    }
    if (t < 30) return;
    const int loopT = (t - 30) % 151;
    const auto pos  = ShootPos(enemy);
    if (loopT == 0) {
        for (int i = 0; i < 10; i++) {
            SpawnFanAbs(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 8, 1, 3.2f,
                        3.2f, i * 0.2617994f, 0.28559932f, true);
        }
        ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
    }
    if (loopT == 80) {
        for (int i = 0; i < 10; i++) {
            SpawnFanAbs(ctx, pos, EBulletType::Kunai, EBulletColor::Red, 4, 2, 2.0f, 1.0f,
                        PI - i * 0.2617994f, 0.044879895f, true);
        }
    }
}

void RunEternalMeek(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        BossPhaseUtil::StartPhase(enemy, ctx,
                                  {
                                      "Conjuring \"Eternal Meek\"",
                                      -1,
                                      0,
                                      900,
                                      SUB_SAKUYA_EXIT,
                                      SUB_SAKUYA_DEATH,
                                      -1,
                                      -1,
                                      true,
                                      true,
                                      0,
                                      false,
                                      true,
                                  });
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 65;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0 || loopT == 18 || loopT == 36) {
        SpawnRandomArc(ctx, pos, EBulletType::Rice, EBulletColor::Red, 28, 2.0f, 4.2f, -PI, PI);
        ctx.bullets.SpawnCircle(pos, EBulletType::Ball, EBulletColor::Blue, 12, 4.0f,
                                RandAngle());
    }
}

void RunRemiliaNonSpell1(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 13000, 4, 2700, SUB_REMILIA_STAR,
                          1200, false);
    }
    if (t < 100) return;
    const int loopT = (t - 100) % 210;
    if (loopT == 0 || loopT == 100) {
        const auto pos = ShootPos(enemy);
        for (int i = 0; i < 32; i++) {
            SpawnFanAbs(ctx, pos, EBulletType::Ball, EBulletColor::DarkBlue, 3, 1, 1.8f, 1.8f,
                        RandAngle(), 0.09817477f);
            if (i % 4 == 0) {
                SpawnFanAbs(ctx, pos, EBulletType::Bubble, EBulletColor::Gray, 4, 1, 3.5f,
                            3.5f, -Util::HALF_PI - i * 0.3926991f, 1.0471976f);
            }
        }
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
}

void RunRemiliaNonSpell2(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 15500, 3, 2700, SUB_REMILIA_SCARLET,
                          1600, false);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
    }
    if (t < 60) return;
    const int loopT = (t - 60) % 340;
    const auto pos  = ShootPos(enemy);
    if (loopT == 90 || loopT == 180 || loopT == 270 || loopT == 280) {
        const float aim = AimAngle(pos, ctx.playerPos);
        for (float off : {0.0f, 0.4f, -0.4f}) {
            SpawnVampireBurst(ctx, pos, aim + off, true);
        }
    }
    if (loopT == 0 || loopT == 90 || loopT == 180) {
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
}

void RunRemiliaNonSpell3(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 11000, 2, 2700, SUB_REMILIA_VLAD,
                          1600, false);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
    }
    if (t < 60) return;
    const int loopT = (t - 60) % 298;
    const auto pos  = ShootPos(enemy);
    if (loopT == 0) {
        for (int i = 0; i < 30; i++) {
            ctx.bullets.SpawnCircle(pos, EBulletType::Dagger, EBulletColor::DarkRed, 4, 3.0f,
                                    RandAngle() + i * 0.09817477f, false, 0.0f, 0, true);
        }
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
    }
    if (loopT == 128) {
        for (int i = 0; i < 30; i++) {
            ctx.bullets.SpawnCircle(pos, EBulletType::Dagger, EBulletColor::DarkPurple, 4, 3.0f,
                                    RandAngle() - i * 0.09817477f, false, 0.0f, 0, true);
        }
    }
}

void RunRemiliaNonSpell4(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Remilia Scarlet", 13000, 1, 3600, SUB_REMILIA_SHOOT,
                          1300, false);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
        SetRemiliaPoses(enemy);
    }
    if (t < 60) return;
    const int loopT = (t - 60) % 360;
    const auto pos  = ShootPos(enemy);
    if (loopT == 0 || loopT == 120 || loopT == 240) {
        SpawnAimedStack(ctx, pos, EBulletType::Rice, EBulletColor::Red, 18, 1, 2.2f, 2.2f,
                        0.18f);
        SpawnAimedStack(ctx, pos, EBulletType::Dagger, EBulletColor::DarkPurple, 13, 2, 3.0f,
                        1.5f, 0.12f, 0.2f, true);
        ScriptUtil::StartRandomMove(enemy, ctx, (loopT == 120 ? 7.0f : 4.0f), 30);
    }
    if (loopT == 270) {
        ctx.bullets.SpawnCircle(pos, EBulletType::Rice, EBulletColor::Red, 48, 2.4f,
                                RandAngle());
    }
}

void RunStarOfDavid(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Heaven's Punishment \"Star of David\"", -1, 4, 2400,
                          SUB_REMILIA_NONSPELL_2, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 244;
    if (loopT == 0) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        for (int i = 0; i < 5; i++) {
            ctx.bullets.SpawnCircle(pos, EBulletType::BigBall, EBulletColor::DarkPurple, 5,
                                    1.4f, RandAngle() + i * TWO_PI / 5.0f);
        }
    }
    if (loopT == 60) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 90);
}

void RunScarletNetherworld(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Nether Sign \"Scarlet Netherworld\"", -1, 3, 2400,
                          SUB_REMILIA_NONSPELL_3, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 231;
    if (loopT < 48 && loopT % 8 == 0) {
        const auto pos = ShootPos(enemy, {0.0f, 0.0f});
        ctx.bullets.SpawnCircle(pos, EBulletType::Rice, EBulletColor::Red, 24, 1.8f,
                                RandAngle());
        ctx.bullets.SpawnCircle(pos, EBulletType::Shard, EBulletColor::Red, 16, 2.2f,
                                RandAngle());
    }
    if (loopT == 90) ScriptUtil::StartRandomMove(enemy, ctx, 1.5f, 90);
}

void RunVlad(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Curse \"Curse of Vlad Tepes\"", -1, 2, 2400,
                          SUB_REMILIA_NONSPELL_4, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 144.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 190;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0) {
        ctx.bullets.SpawnCircle(pos, EBulletType::Dagger, EBulletColor::DarkPurple, 18, 3.0f,
                                RandAngle(), false, 0.0f, 0, true);
        SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, 24, 1.0f, 3.0f, -PI,
                       PI);
    }
    if (loopT == 60) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
}

void RunScarletShoot(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "Scarlet Sign \"Scarlet Shoot\"", -1, 1, 1800,
                          SUB_REMILIA_RED_MAGIC, -1, true);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 112.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 424;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0 || loopT == 60 || loopT == 270 || loopT == 390) {
        const float aim = AimAngle(pos, ctx.playerPos);
        for (float off : {0.0f, 0.7853982f, -0.7853982f, 1.5707964f, -1.5707964f}) {
            SpawnVampireBurst(ctx, pos, aim + off, false);
        }
    }
    if (loopT == 60 || loopT == 180) ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
}

void RunRedMagic(Enemy& enemy, EnemySubCtx& ctx, int t) {
    if (t == 0) {
        StartRemiliaPhase(enemy, ctx, "\"Red Magic\"", 4700, 0, 7200, SUB_REMILIA_DEATH, -1,
                          true);
        ScriptUtil::DropPowerItems(enemy, ctx, 12);
        enemy.m_CanTakeDamage = false;
        ctx.StartLerpTo(enemy, 192.0f, 128.0f, 120);
    }
    if (t == 120) enemy.m_CanTakeDamage = true;
    if (t < 120) return;
    const int loopT = (t - 120) % 386;
    const auto pos  = ShootPos(enemy, {0.0f, 0.0f});
    if (loopT == 0 || loopT == 70 || loopT == 150 || loopT == 240 || loopT == 290) {
        ctx.bullets.SpawnCircle(pos, EBulletType::Bubble, EBulletColor::DarkRed,
                                loopT == 290 ? 16 : 10, loopT == 290 ? 4.0f : 2.0f,
                                RandAngle());
        SpawnRandomArc(ctx, pos, EBulletType::RingBall, EBulletColor::Red, 32, 1.0f, 3.0f, -PI,
                       PI);
        ScriptUtil::StartRandomMove(enemy, ctx, 2.5f, 60);
    }
}

}  // namespace

void Stage6Script::Preload(Anm::Manager& anm) {
    anm.LoadAnm(Anm::STG6ENM.folder, Anm::STG6ENM.txt, Anm::STG6ENM.offset);
    anm.LoadAnm(Anm::STG6ENM2.folder, Anm::STG6ENM2.txt, Anm::STG6ENM2.offset);
}

void Stage6Script::InitSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int offset = Anm::STG6ENM.offset;
    switch (enemy.m_SubId) {
        case SUB_STAGE_EFFECTS:
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            enemy.m_ItemDropCount = 0;
            break;
        case SUB_FAIRY_ARC_BLUE_L:
        case SUB_FAIRY_ARC_RED_L:
            ctx.anm.SetScript(enemy.m_Vm, offset + 8, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            break;
        case SUB_FAIRY_ARC_BLUE_R:
        case SUB_FAIRY_ARC_RED_R:
            ctx.anm.SetScript(enemy.m_Vm, offset + 9, offset);
            enemy.m_HitboxSize = {28.0f, 28.0f};
            break;
        case SUB_FAIRY_SIDE_BURST:
        case SUB_FAIRY_TOP_BURST:
            ctx.anm.SetScript(enemy.m_Vm, offset + 11, offset);
            enemy.m_HitboxSize       = {28.0f, 28.0f};
            enemy.m_DeathCallbackSub = SUB_FAIRY_DROP_8;
            enemy.m_ItemDropCount    = 0;
            break;
        case SUB_FAIRY_DROP_8:
            ctx.anm.SetScript(enemy.m_Vm, offset + 11, offset);
            enemy.m_CanTakeDamage = false;
            enemy.m_HitboxSize    = {0.0f, 0.0f};
            enemy.m_ItemDropCount = 0;
            break;
        case SUB_SAKUYA_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, offset + 128, offset);
            enemy.m_Pos            = Util::GameFieldToScreen(-32.0f, 128.0f);
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = true;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Sakuya Izayoi";
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 120.0f);
            enemy.m_HitboxSize     = {40.0f, 56.0f};
            SetSakuyaPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;
        case SUB_REMILIA_ENTRY:
            ctx.anm.SetScript(enemy.m_Vm, Anm::STG6ENM2.offset + 160, Anm::STG6ENM2.offset);
            enemy.m_Pos            = Util::GameFieldToScreen(256.0f, -64.0f);
            enemy.m_IsBoss         = true;
            enemy.m_BlocksTimeline = true;
            enemy.m_CanTakeDamage  = false;
            enemy.m_ItemDropCount  = 0;
            enemy.m_BossTitle      = "Remilia Scarlet";
            enemy.m_BoundsMin      = Util::GameFieldToScreen(32.0f, 48.0f);
            enemy.m_BoundsMax      = Util::GameFieldToScreen(352.0f, 120.0f);
            enemy.m_HitboxSize     = {56.0f, 56.0f};
            SetRemiliaPoses(enemy);
            ScriptUtil::SetDeathEffects(enemy, 671, 676);
            break;
        default:
            break;
    }
}

void Stage6Script::RunSub(Enemy& enemy, EnemySubCtx& ctx) {
    const int t = enemy.m_FrameTimer;
    switch (enemy.m_SubId) {
        case SUB_STAGE_EFFECTS:
            if (t >= 2160) enemy.m_Alive = false;
            break;
        case SUB_FAIRY_ARC_BLUE_L:
        case SUB_FAIRY_ARC_BLUE_R:
            RunArcFairy(enemy, ctx, t, false, true);
            break;
        case SUB_FAIRY_ARC_RED_L:
        case SUB_FAIRY_ARC_RED_R:
            RunArcFairy(enemy, ctx, t, true, false);
            break;
        case SUB_FAIRY_SIDE_BURST:
            RunBurstFairy(enemy, ctx, t, false);
            break;
        case SUB_FAIRY_TOP_BURST:
            RunBurstFairy(enemy, ctx, t, true);
            break;
        case SUB_FAIRY_DROP_8:
            if (t == 0) {
                ScriptUtil::DropPowerItems(enemy, ctx, 8);
                enemy.m_Alive = false;
            }
            break;
        case SUB_SAKUYA_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 128.0f, 60);
            if (t == 60) ctx.TransitionToSub(enemy, SUB_SAKUYA_MAIN);
            break;
        case SUB_SAKUYA_MAIN:
            RunSakuyaMain(enemy, ctx, t);
            break;
        case SUB_SAKUYA_ETERNAL_MEEK:
            RunEternalMeek(enemy, ctx, t);
            break;
        case SUB_SAKUYA_DEATH:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ScriptUtil::DropPowerItems(enemy, ctx, 20);
                ctx.items.SpawnItem(enemy.m_Pos, ItemType::Bomb);
                ctx.BulletCancelIntoPointItems();
                enemy.m_Alive = false;
            }
            break;
        case SUB_SAKUYA_EXIT:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                ctx.BulletCancelIntoPointItems();
                ctx.StartLerpTo(enemy, 192.0f, -64.0f, 60);
            }
            if (t >= 60) enemy.m_Alive = false;
            break;
        case SUB_REMILIA_ENTRY:
            if (t == 0) ctx.StartLerpTo(enemy, 192.0f, 96.0f, 120);
            if (t == 120) ctx.TransitionToSub(enemy, SUB_REMILIA_WAIT);
            break;
        case SUB_REMILIA_WAIT:
            if (t == 0) ctx.TransitionToSub(enemy, SUB_REMILIA_NONSPELL_1);
            break;
        case SUB_REMILIA_NONSPELL_1:
            RunRemiliaNonSpell1(enemy, ctx, t);
            break;
        case SUB_REMILIA_STAR:
            RunStarOfDavid(enemy, ctx, t);
            break;
        case SUB_REMILIA_NONSPELL_2:
            RunRemiliaNonSpell2(enemy, ctx, t);
            break;
        case SUB_REMILIA_SCARLET:
            RunScarletNetherworld(enemy, ctx, t);
            break;
        case SUB_REMILIA_NONSPELL_3:
            RunRemiliaNonSpell3(enemy, ctx, t);
            break;
        case SUB_REMILIA_VLAD:
            RunVlad(enemy, ctx, t);
            break;
        case SUB_REMILIA_NONSPELL_4:
            RunRemiliaNonSpell4(enemy, ctx, t);
            break;
        case SUB_REMILIA_SHOOT:
            RunScarletShoot(enemy, ctx, t);
            break;
        case SUB_REMILIA_RED_MAGIC:
            RunRedMagic(enemy, ctx, t);
            break;
        case SUB_REMILIA_DEATH:
            if (t == 0) {
                ctx.SetTimeStopped(false);
                enemy.m_CanTakeDamage = false;
                enemy.m_InSpellcard   = false;
                enemy.m_ShowSpellName = false;
                ctx.BulletCancelIntoPointItems();
                enemy.m_Alive = false;
            }
            break;
        default:
            break;
    }
}
