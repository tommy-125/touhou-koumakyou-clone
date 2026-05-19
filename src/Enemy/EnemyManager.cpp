#include "Enemy/EnemyManager.hpp"

#include <cstdlib>
#include <utility>

#include "GameManager.hpp"
#include "Item/ItemManager.hpp"
#include "Player.hpp"
#include "Util/Math.hpp"

static constexpr ItemType RANDOM_ITEM_TABLE[32] = {
    ItemType::PowerSmall, ItemType::PowerSmall, ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::PowerSmall, ItemType::Point,
    ItemType::Point,      ItemType::Point,      ItemType::PowerSmall, ItemType::PowerSmall,
    ItemType::PowerSmall, ItemType::Point,      ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::Point,      ItemType::PowerSmall,
    ItemType::Point,      ItemType::PowerSmall, ItemType::PowerSmall, ItemType::Point,
    ItemType::Point,      ItemType::Point,      ItemType::PowerSmall, ItemType::PowerBig,
};

namespace {
constexpr int   EFF_DEATH_ANM_669          = Anm::EFF00.offset + 0;
constexpr int   EFF_DEATH_ANM_670          = Anm::EFF00.offset + 1;
constexpr int   EFF_DEATH_ANM_671          = Anm::EFF00.offset + 2;
constexpr int   EFF_DEATH_ANM_680          = Anm::EFF00.offset + 9;
constexpr int   EFF_DEATH_ANM_682          = Anm::EFF00.offset + 11;
}  // namespace

EnemyManager::EnemyManager() {
    m_EffectAnm.LoadAnm(Anm::EFF00.folder, Anm::EFF00.txt, Anm::EFF00.offset);
}

void EnemyManager::TurnAllBulletsIntoPointItems() {
    SetTimeStopped(false);
    if (!m_Items) {
        ClearAllBullets();
        return;
    }
    m_BulletManager.TurnAllBulletsIntoPointItems(*m_Items);
    m_LaserManager.TurnAllLasersIntoPointItems(*m_Items);
}

void EnemyManager::SetScript(std::unique_ptr<IStageScript> script) {
    m_Script = std::move(script);
    if (m_Script) m_Script->Preload(m_Anm);
}

EnemySubCtx EnemyManager::MakeCtx() {
    return EnemySubCtx{
        m_Anm, m_BulletManager, m_LaserManager, *m_Items, m_Renderer, m_PlayerPos,
        [this](int subId, float x, float y, int life, int score, bool mirrored, int itemDrop) {
            return SpawnEnemy(subId, x, y, life, score, mirrored, itemDrop);
        },
        [this]() { KillAllNonBossEnemies(); },
        [this](bool stopped) { SetTimeStopped(stopped); },
        [this]() { m_BulletManager.RedirectTimeStopBullets(m_PlayerPos, 14); }};
}

void EnemyManager::SetTimeStopped(bool stopped) {
    m_TimeStopped = stopped;
    m_BulletManager.SetTimeStopped(stopped);
    m_LaserManager.SetTimeStopped(stopped);
    if (m_Items) m_Items->SetTimeStopped(stopped);
    if (m_GameManager) m_GameManager->timeStopped = stopped;
}

void EnemyManager::KillAllNonBossEnemies() {
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive || enemy.m_IsBoss) continue;

        SpawnDeathEffect(enemy);
        if (m_Items) {
            for (int k = 0; k < enemy.m_ItemDropCount; k++) {
                if (enemy.m_ItemDrop >= 0) {
                    m_Items->SpawnItem(enemy.m_Pos, static_cast<ItemType>(enemy.m_ItemDrop));
                } else if (enemy.m_ItemDrop == -1 && m_RandomItemSpawnIndex++ % 3 == 0) {
                    m_Items->SpawnItem(enemy.m_Pos, RANDOM_ITEM_TABLE[m_RandomItemTableIndex]);
                    m_RandomItemTableIndex = (m_RandomItemTableIndex + 1) % 32;
                }
            }
        }

        enemy.m_Alive = false;
        if (enemy.m_Vm.obj) {
            m_Renderer.RemoveChild(enemy.m_Vm.obj);
            enemy.m_Vm.obj = nullptr;
        }
    }
}

void EnemyManager::DespawnAllNonBossEnemies() {
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive || enemy.m_IsBoss) continue;

        enemy.m_Alive = false;
        if (enemy.m_Vm.obj) {
            m_Renderer.RemoveChild(enemy.m_Vm.obj);
            enemy.m_Vm.obj = nullptr;
        }
    }
}

void EnemyManager::UpdateBossCallbacks(Enemy& enemy, GameManager& /*gm*/) {
    if (!enemy.m_IsBoss) return;

    enemy.m_BossTimer++;

    if (enemy.m_LifeCallbackThreshold >= 0 && enemy.m_Life < enemy.m_LifeCallbackThreshold) {
        enemy.m_Life                   = enemy.m_LifeCallbackThreshold;
        int sub                        = enemy.m_LifeCallbackSub;
        enemy.m_LifeCallbackThreshold  = -1;
        enemy.m_LifeCallbackSub        = -1;
        enemy.m_TimerCallbackThreshold = -1;
        enemy.m_TimerCallbackSub       = enemy.m_DeathCallbackSub;
        enemy.m_CanTakeDamage          = false;
        TurnAllBulletsIntoPointItems();
        DespawnAllNonBossEnemies();
        enemy.m_SubId      = sub;
        enemy.m_FrameTimer = -1;
        return;
    }

    if (enemy.m_TimerCallbackThreshold >= 0 &&
        enemy.m_BossTimer >= enemy.m_TimerCallbackThreshold) {
        if (enemy.m_LifeCallbackThreshold > 0) {
            enemy.m_Life                  = enemy.m_LifeCallbackThreshold;
            enemy.m_LifeCallbackThreshold = -1;
        }
        int sub                        = enemy.m_TimerCallbackSub;
        enemy.m_TimerCallbackThreshold = -1;
        enemy.m_TimerCallbackSub       = enemy.m_DeathCallbackSub;
        enemy.m_CanTakeDamage          = false;
        ClearAllBullets();
        DespawnAllNonBossEnemies();
        enemy.m_SubId      = sub;
        enemy.m_FrameTimer = -1;
        return;
    }
}

Enemy* EnemyManager::SpawnEnemy(int subId, float x, float y, int life, int score, bool mirrored,
                                int itemDrop) {
    for (auto& enemy : m_Enemies) {
        if (enemy.m_Alive) continue;

        enemy              = Enemy{};
        enemy.m_Alive      = true;
        enemy.m_SubId      = subId;
        enemy.m_Pos        = Util::GameFieldToScreen(x, y);
        enemy.m_Life       = life;
        enemy.m_Score      = score;
        enemy.m_Mirrored   = mirrored;
        enemy.m_FrameTimer = 0;

        if (m_Script) {
            auto ctx = MakeCtx();
            m_Script->InitSub(enemy, ctx);
        }

        if (itemDrop != -99) {
            enemy.m_ItemDrop = itemDrop;
        }

        if (enemy.m_Vm.obj) {
            m_Renderer.AddChild(enemy.m_Vm.obj);
        }
        return &enemy;
    }
    return nullptr;
}

void EnemyManager::UpdatePhysics(Enemy& enemy) {
    if (enemy.m_IsLerping) {
        enemy.m_LerpElapsed++;
        float t     = 1.0f - (float)enemy.m_LerpElapsed / (float)enemy.m_LerpFrames;
        float frac  = 1.0f - t * t;
        enemy.m_Pos = enemy.m_LerpOrigin + frac * (enemy.m_LerpTarget - enemy.m_LerpOrigin);
        if (enemy.m_LerpElapsed >= enemy.m_LerpFrames) {
            enemy.m_Pos       = enemy.m_LerpTarget;
            enemy.m_IsLerping = false;
        }
        return;
    }
    enemy.m_Pos.x += std::cos(enemy.m_Angle) * enemy.m_Speed;
    enemy.m_Pos.y += std::sin(enemy.m_Angle) * enemy.m_Speed;
    enemy.m_Angle += enemy.m_AngularVelocity;
    enemy.m_Speed += enemy.m_Acceleration;
}

void EnemyManager::UpdateBossPose(Enemy& enemy, float horizontalDelta) {
    if (!enemy.m_IsBoss || enemy.m_AnmLeft < 0) return;

    int nextState = 0;
    if (horizontalDelta < 0.0f) {
        nextState = 1;
    } else if (horizontalDelta > 0.0f) {
        nextState = 2;
    }

    if (enemy.m_AnmMoveState == nextState) return;

    const int offset = enemy.m_Vm.spriteOffset;
    switch (nextState) {
        case 0:
            if (enemy.m_AnmMoveState == 0xff) {
                m_Anm.SetScript(enemy.m_Vm, offset + enemy.m_AnmDefault, offset);
            } else if (enemy.m_AnmMoveState == 1) {
                m_Anm.SetScript(enemy.m_Vm, offset + enemy.m_AnmFarLeft, offset);
            } else {
                m_Anm.SetScript(enemy.m_Vm, offset + enemy.m_AnmFarRight, offset);
            }
            break;
        case 1:
            m_Anm.SetScript(enemy.m_Vm, offset + enemy.m_AnmLeft, offset);
            break;
        case 2:
            m_Anm.SetScript(enemy.m_Vm, offset + enemy.m_AnmRight, offset);
            break;
    }
    enemy.m_AnmMoveState = nextState;
}

void EnemyManager::SetTimeline(std::vector<TimelineEntry> entries) {
    m_Timeline    = std::move(entries);
    m_TimelineIdx = 0;
    m_TimelineFrame = 0;
}

void EnemyManager::RunTimeline() {
    if (m_Timeline.empty()) return;
    for (auto& e : m_Enemies) {
        if (e.m_Alive && e.m_BlocksTimeline) return;
    }
    bool bossPresent = false;
    for (auto& e : m_Enemies) {
        if (e.m_Alive && e.m_IsBoss) {
            bossPresent = true;
            break;
        }
    }
    while (m_TimelineIdx < m_Timeline.size() &&
           m_Timeline[m_TimelineIdx].frame <= m_TimelineFrame) {
        const auto& e = m_Timeline[m_TimelineIdx];
        if (e.frame == m_TimelineFrame && !bossPresent) {
            float spawnX = e.randomX ? static_cast<float>(rand() % 353 + 16) : e.x;
            Enemy* spawned = SpawnEnemy(e.subId, spawnX, e.y, e.life, e.score, e.mirrored, e.itemDrop);
            if (spawned && spawned->m_IsBoss) bossPresent = true;
        }
        m_TimelineIdx++;
    }
    bool blockedAfterSpawn = false;
    for (auto& e : m_Enemies) {
        if (e.m_Alive && e.m_BlocksTimeline) {
            blockedAfterSpawn = true;
            break;
        }
    }
    if (!blockedAfterSpawn) {
        m_TimelineFrame++;
    }
}

void EnemyManager::Update(const glm::vec2& playerPos, GameManager& gm) {
    m_GameManager = &gm;
    gm.timeStopped = m_TimeStopped;
    m_PlayerPos = playerPos;
    RunTimeline();

    auto ctx = MakeCtx();
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;

        UpdateBossCallbacks(enemy, gm);
        if (!enemy.m_Alive) continue;

        if (m_Script) m_Script->RunSub(enemy, ctx);

        if (!enemy.m_Alive) {
            if (enemy.m_SpawnDeathEffectOnRemoval) {
                SpawnDeathEffect(enemy);
                enemy.m_SpawnDeathEffectOnRemoval = false;
            }
            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
            continue;
        }

        const float oldX = enemy.m_Pos.x;
        UpdatePhysics(enemy);
        UpdateBossPose(enemy, enemy.m_Pos.x - oldX);

        enemy.m_Vm.pos = enemy.m_Pos;
        if (enemy.m_RotateWithAngle) enemy.m_Vm.rotation = Util::HALF_PI - enemy.m_Angle;
        m_Anm.UpdateObjects(enemy.m_Vm);

        enemy.m_FrameTimer++;

        if (!enemy.m_IsBoss &&
            !Util::IsInGameBounds(enemy.m_Pos.x, enemy.m_Pos.y, 0, 0, -200, -200, 600, 700)) {
            enemy.m_Alive = false;
            if (enemy.m_Vm.obj) {
                m_Renderer.RemoveChild(enemy.m_Vm.obj);
                enemy.m_Vm.obj = nullptr;
            }
        }
    }

    m_BulletManager.Update(m_PlayerPos);
    m_LaserManager.Update();
    UpdateEffects();
    m_Renderer.Update();
    m_Frame++;
}

void EnemyManager::Render() {
    m_BulletManager.Render();
    m_LaserManager.Render();
    m_Renderer.Update();
}

int EnemyManager::ApplyPlayerBulletDamage(Player& player) {
    int totalScore = 0;
    for (auto& enemy : m_Enemies) {
        if (!enemy.m_Alive) continue;
        if (!enemy.m_CanTakeDamage) continue;

        int dmg = player.CalcDamageToEnemy(enemy.m_Pos, enemy.m_HitboxSize);
        if (dmg <= 0) continue;

        // TH6: damage capped at 70/frame, hit score = (dmg/5)*10 on capped value,
        // spellcard divides damage by 7 (min 1).
        if (dmg > 70) dmg = 70;
        totalScore += (dmg / 5) * 10;
        if (enemy.m_InSpellcard) {
            dmg = (dmg > 7) ? dmg / 7 : 1;
        }
        enemy.m_Life -= dmg;

        // Boss overshoot guard: if a single hit crosses below a pending life callback
        // threshold, clamp to threshold and trigger the callback instead of death. Prevents
        // players from skipping spellcards with a burst hit.
        if (enemy.m_IsBoss && enemy.m_LifeCallbackThreshold >= 0 &&
            enemy.m_Life < enemy.m_LifeCallbackThreshold) {
            enemy.m_Life                   = enemy.m_LifeCallbackThreshold;
            int sub                        = enemy.m_LifeCallbackSub;
            enemy.m_LifeCallbackThreshold  = -1;
            enemy.m_LifeCallbackSub        = -1;
            enemy.m_TimerCallbackThreshold = -1;
            enemy.m_TimerCallbackSub       = enemy.m_DeathCallbackSub;
            enemy.m_CanTakeDamage          = false;
            TurnAllBulletsIntoPointItems();
            DespawnAllNonBossEnemies();
            enemy.m_SubId      = sub;
            enemy.m_FrameTimer = -1;
            continue;
        }

        if (enemy.m_Life <= 0) {
            if (enemy.m_IsBoss) {
                enemy.m_Life             = 0;
                enemy.m_CanTakeDamage    = false;
                int sub                  = enemy.m_DeathCallbackSub;
                enemy.m_DeathCallbackSub = -1;
                if (sub >= 0) {
                    TurnAllBulletsIntoPointItems();
                    SpawnDeathEffect(enemy);
                    enemy.m_SpawnDeathEffectOnRemoval = true;
                    enemy.m_SubId      = sub;
                    enemy.m_FrameTimer = -1;
                } else {
                    TurnAllBulletsIntoPointItems();
                    totalScore += enemy.m_Score;
                    SpawnDeathEffect(enemy);
                    enemy.m_Alive = false;
                    if (enemy.m_Vm.obj) {
                        m_Renderer.RemoveChild(enemy.m_Vm.obj);
                        enemy.m_Vm.obj = nullptr;
                    }
                }
            } else {
                totalScore += enemy.m_Score;
                SpawnDeathEffect(enemy);

                for (int k = 0; k < enemy.m_ItemDropCount; k++) {
                    if (enemy.m_ItemDrop >= 0) {
                        m_Items->SpawnItem(enemy.m_Pos, static_cast<ItemType>(enemy.m_ItemDrop));
                    } else {
                        if (m_RandomItemSpawnIndex++ % 3 == 0) {
                            m_Items->SpawnItem(enemy.m_Pos,
                                               RANDOM_ITEM_TABLE[m_RandomItemTableIndex]);
                            m_RandomItemTableIndex = (m_RandomItemTableIndex + 1) % 32;
                        }
                    }
                }

                int sub                  = enemy.m_DeathCallbackSub;
                enemy.m_DeathCallbackSub = -1;
                if (sub >= 0) {
                    enemy.m_SubId           = sub;
                    enemy.m_FrameTimer      = 0;
                    enemy.m_Life            = 1;
                    enemy.m_Speed           = 0.0f;
                    enemy.m_Acceleration    = 0.0f;
                    enemy.m_AngularVelocity = 0.0f;
                    enemy.m_IsLerping       = false;
                    enemy.m_HitboxSize      = {0.0f, 0.0f};
                    enemy.m_ItemDropCount   = 0;

                    if (m_Script) {
                        auto ctx = MakeCtx();
                        m_Script->RunSub(enemy, ctx);
                    }
                    if (enemy.m_Alive) enemy.m_FrameTimer = 1;
                    if (!enemy.m_Alive && enemy.m_Vm.obj) {
                        m_Renderer.RemoveChild(enemy.m_Vm.obj);
                        enemy.m_Vm.obj = nullptr;
                    }
                    continue;
                }

                enemy.m_Alive = false;
                if (enemy.m_Vm.obj) {
                    m_Renderer.RemoveChild(enemy.m_Vm.obj);
                    enemy.m_Vm.obj = nullptr;
                }
            }
        }
    }
    return totalScore;
}

void EnemyManager::SpawnEffect(int scriptIdx, const glm::vec2& pos, float zIndex,
                               const glm::vec2& scale) {
    for (auto& effect : m_Effects) {
        if (effect.active) continue;

        effect.active = true;
        effect.vm     = Anm::Vm{};
        m_EffectAnm.SetScript(effect.vm, scriptIdx, Anm::EFF00.offset);
        effect.vm.pos    = pos;
        effect.vm.zIndex = zIndex;
        effect.vm.scale  = scale;
        if (effect.vm.obj) {
            m_Renderer.AddChild(effect.vm.obj);
        }
        return;
    }
}

int EnemyManager::GetDeathPrimaryScript(int deathAnm1) const {
    switch (deathAnm1) {
        case 669:
            return EFF_DEATH_ANM_669;
        case 671:
            return EFF_DEATH_ANM_671;
        case 670:
        default:
            return EFF_DEATH_ANM_670;
    }
}

int EnemyManager::GetDeathSecondaryScript(int deathAnm2) const {
    switch (deathAnm2) {
        case 676:
            return EFF_DEATH_ANM_680;
        case 678:
        default:
            return EFF_DEATH_ANM_682;
    }
}

void EnemyManager::SpawnDeathEffect(const Enemy& enemy) {
    const glm::vec2 pos = enemy.m_Pos;
    const int       primaryScript = GetDeathPrimaryScript(enemy.m_DeathEffectPrimary);
    const int       secondaryScript = GetDeathSecondaryScript(enemy.m_DeathEffectSecondary);

    SpawnEffect(primaryScript, pos, 0.78f);

    for (int i = 0; i < 4; i++) {
        glm::vec2 offset = {
            ((static_cast<float>(std::rand() % 2001) / 1000.0f) - 1.0f) * 10.0f,
            ((static_cast<float>(std::rand() % 2001) / 1000.0f) - 1.0f) * 8.0f,
        };
        SpawnEffect(secondaryScript, pos + offset, 0.79f);
    }
}

void EnemyManager::UpdateEffects() {
    for (auto& effect : m_Effects) {
        if (!effect.active) continue;

        m_EffectAnm.UpdateObjects(effect.vm);
        if (effect.vm.scriptIdx >= 0) continue;

        if (effect.vm.obj) {
            m_Renderer.RemoveChild(effect.vm.obj);
            effect.vm.obj = nullptr;
        }
        effect.active = false;
    }
}

bool EnemyManager::CheckPlayerHit(glm::vec2 playerPos, glm::vec2 playerHitboxSize) {
    return m_BulletManager.CheckPlayerHit(playerPos, playerHitboxSize) ||
           m_LaserManager.CheckPlayerHit(playerPos, playerHitboxSize);
}

void EnemyManager::ClearAllBullets() {
    SetTimeStopped(false);
    m_BulletManager.ClearAll();
    m_LaserManager.ClearAll();
}

BossHudState EnemyManager::GetBossHudState() const {
    for (const auto& enemy : m_Enemies) {
        if (!enemy.m_Alive || !enemy.m_IsBoss) continue;

        BossHudState state;
        state.visible          = true;
        state.showSpellName    = enemy.m_ShowSpellName;
        state.life             = enemy.m_Life;
        state.minLife          = 0;
        state.maxLife          = enemy.m_BossMaxLife > 0 ? enemy.m_BossMaxLife : 1;
        state.bossLifeCount    = enemy.m_BossLifeCount;
        state.title            = enemy.m_BossTitle;
        if (enemy.m_TimerCallbackThreshold >= 0) {
            const int framesLeft = std::max(0, enemy.m_TimerCallbackThreshold - enemy.m_BossTimer);
            state.secondsRemaining = (framesLeft + 59) / 60;
        }
        return state;
    }

    return {};
}

void EnemyManager::SkipToFrame(int frame) {
    for (auto& e : m_Enemies) {
        if (!e.m_Alive) continue;
        e.m_Alive = false;
        if (e.m_Vm.obj) {
            m_Renderer.RemoveChild(e.m_Vm.obj);
            e.m_Vm.obj = nullptr;
        }
    }
    m_BulletManager.ClearAll();
    m_LaserManager.ClearAll();
    m_Frame = frame;
    m_TimelineFrame = frame;
    m_TimelineIdx = 0;
    while (m_TimelineIdx < m_Timeline.size() && m_Timeline[m_TimelineIdx].frame < frame) {
        m_TimelineIdx++;
    }
}
