#include "Item/ItemManager.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <string>

#include "Anm/AnmDefs.hpp"
#include "Audio/AudioManager.hpp"
#include "GameManager.hpp"
#include "Util/Math.hpp"

static constexpr float ITEM_GRAB_HALF_X = 12.0f;
static constexpr float ITEM_GRAB_HALF_Y = 12.0f;
static constexpr float ITEM_HALF_SIZE   = 8.0f;
static constexpr float ITEM_VEL_INIT_Y  = -2.2f;
static constexpr float ITEM_GRAVITY     = 0.03f;
static constexpr float ITEM_VEL_MAX_Y   = 3.0f;
static constexpr float ITEM_AUTOGET_Y   = Util::FIELD_OFFSET_Y + 128.0f;
static constexpr int   MAX_LIVES        = 8;
static constexpr int   MAX_BOMBS        = 8;
static constexpr int   PICKUP_LABEL_FRAMES = 60;
static constexpr float PICKUP_LABEL_SCALE  = 0.5f;
static constexpr float PICKUP_LABEL_Z      = 30.0f;
static constexpr float PICKUP_LABEL_RISE   = 0.5f;
static constexpr int   POWER_ITEM_SCORE_COUNT = 31;
static constexpr int   POWER_ITEM_SCORE[POWER_ITEM_SCORE_COUNT] = {
    10,   20,   30,   40,   50,    60,    70,    80,
    90,   100,  200,  300,  400,   500,   600,   700,
    800,  900,  1000, 2000, 3000,  4000,  5000,  6000,
    7000, 8000, 9000, 10000, 11000, 12000, 51200,
};

namespace {
void ResetItemForSpawn(Item& item) {
    auto obj = item.m_Vm.obj;
    item = Item{};
    item.m_Vm.obj = obj;
}

void AddScoreWithExtend(GameManager& gm, int points) {
    const int extendCount = gm.AddScore(points);
    for (int i = 0; i < extendCount; ++i) {
        AudioManager::Instance().Play(SoundEffect::Extend);
    }
}
}  // namespace

ItemManager::ItemManager() {
    m_Anm.LoadAnm(Anm::ETAMA3.folder, Anm::ETAMA3.txt, Anm::ETAMA3.offset);
    m_AsciiAnm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);

    for (auto& item : m_Items) {
        item.m_Vm.obj =
            std::make_shared<Util::GameObject>(nullptr, 1.0f, glm::vec2{0, 0}, false);
    }

    for (auto& label : m_PickupLabels) {
        label.line.Configure(m_LabelRenderer, m_AsciiAnm, PICKUP_LABEL_Z);
    }
}

void ItemManager::SpawnPickupLabel(glm::vec2 pos, const std::string& text) {
    PickupLabel& label = m_PickupLabels[m_NextLabelIdx];
    m_NextLabelIdx     = (m_NextLabelIdx + 1) % MAX_PICKUP_LABELS;

    label.active = true;
    label.timer  = 0;
    label.pos    = pos;
    label.line.SetText(text, Anm::Manager::ToPtsd(label.pos), PICKUP_LABEL_SCALE,
                       Util::AsciiTextAlign::Center);
    label.line.SetAlpha(1.0f);
    label.line.SetVisible(true);
}

bool ItemManager::ConsumeFullPowerActivated() {
    const bool activated = m_FullPowerActivated;
    m_FullPowerActivated = false;
    return activated;
}

void ItemManager::UpdatePickupLabels() {
    for (auto& label : m_PickupLabels) {
        if (!label.active) continue;

        if (!m_TimeStopped) {
            label.timer++;
            label.pos.y -= PICKUP_LABEL_RISE;
        }

        const float alpha =
            1.0f - std::clamp(static_cast<float>(label.timer) / PICKUP_LABEL_FRAMES, 0.0f, 1.0f);
        label.line.SetLayout(Anm::Manager::ToPtsd(label.pos), PICKUP_LABEL_SCALE,
                             Util::AsciiTextAlign::Center);
        label.line.SetAlpha(alpha);

        if (label.timer >= PICKUP_LABEL_FRAMES) {
            label.active = false;
            label.line.SetVisible(false);
        }
    }
}

void ItemManager::SpawnItem(glm::vec2 pos, ItemType type, int state) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        int idx = (m_NextIdx + i) % MAX_ITEMS;

        if (!m_Items[idx].m_Alive) {
            Item& item   = m_Items[idx];
            ResetItemForSpawn(item);
            item.m_Alive = true;
            item.m_Pos   = pos;
            item.m_Vel   = {0.0f, ITEM_VEL_INIT_Y};
            item.m_StartPos = pos;
            item.m_TargetPos = {
                Util::FIELD_OFFSET_X + 48.0f + static_cast<float>(std::rand() % 289),
                Util::FIELD_OFFSET_Y - 64.0f + static_cast<float>(std::rand() % 193),
            };
            item.m_Type  = type;
            item.m_State = state;
            item.m_Timer = 0;
            m_NextIdx    = (idx + 1) % MAX_ITEMS;

            int scriptIdx = Anm::ETAMA3.offset + 21 + static_cast<int>(type);
            m_Anm.SetScript(item.m_Vm, scriptIdx, Anm::ETAMA3.offset);
            if (item.m_Vm.obj) m_Renderer.AddChild(item.m_Vm.obj);
            return;
        }
    }
}

void ItemManager::Update(glm::vec2 playerPos, GameManager& gm) {
    m_TimeStopped = gm.timeStopped;
    if (gm.power < 128) m_PowerItemScoreIndex = 0;

    for (auto& item : m_Items) {
        if (!item.m_Alive) continue;

        if (!m_TimeStopped) {
            if (item.m_State == 2 && item.m_Timer < 60) {
                const float t = static_cast<float>(item.m_Timer) / 60.0f;
                item.m_Pos = item.m_StartPos * (1.0f - t) + item.m_TargetPos * t;
            } else {
                if (item.m_State == 1 || (gm.power >= 128 && playerPos.y <= ITEM_AUTOGET_Y)) {
                    glm::vec2 toPlayer = playerPos - item.m_Pos;
                    const float dist = std::max(1.0f, std::sqrt(toPlayer.x * toPlayer.x +
                                                                toPlayer.y * toPlayer.y));
                    item.m_Vel = toPlayer / dist * 8.0f;
                    item.m_State = 1;
                } else {
                    item.m_Vel.x = 0.0f;
                    if (item.m_Vel.y < ITEM_VEL_INIT_Y) item.m_Vel.y = ITEM_VEL_INIT_Y;
                    if (item.m_Vel.y < ITEM_VEL_MAX_Y)
                        item.m_Vel.y += ITEM_GRAVITY;
                    else
                        item.m_Vel.y = ITEM_VEL_MAX_Y;
                }
                item.m_Pos += item.m_Vel;
            }
            item.m_Timer++;
        }

        item.m_Vm.pos = item.m_Pos;
        m_Anm.UpdateObjects(item.m_Vm);

        if (m_TimeStopped) continue;

        float dx = std::abs(item.m_Pos.x - playerPos.x);
        float dy = std::abs(item.m_Pos.y - playerPos.y);
        if (dx < ITEM_GRAB_HALF_X + ITEM_HALF_SIZE && dy < ITEM_GRAB_HALF_Y + ITEM_HALF_SIZE) {
            switch (item.m_Type) {
                case ItemType::PowerSmall: {
                    const int oldPower = gm.power;
                    int itemScore = 10;
                    if (gm.power >= 128) {
                        m_PowerItemScoreIndex =
                            std::min(POWER_ITEM_SCORE_COUNT - 1, m_PowerItemScoreIndex + 1);
                        itemScore = POWER_ITEM_SCORE[m_PowerItemScoreIndex];
                    } else {
                        m_PowerItemScoreIndex = 0;
                        gm.power              = std::min(128, gm.power + 1);
                    }
                    AddScoreWithExtend(gm, itemScore);
                    if (oldPower < 128 && gm.power >= 128) {
                        m_FullPowerActivated = true;
                        SpawnPickupLabel(item.m_Pos, "MAX POWER");
                        AudioManager::Instance().Play(SoundEffect::PowerUp);
                    } else {
                        SpawnPickupLabel(item.m_Pos, std::to_string(itemScore));
                    }
                    break;
                }
                case ItemType::PowerBig: {
                    const int oldPower = gm.power;
                    int itemScore = 10;
                    if (gm.power >= 128) {
                        m_PowerItemScoreIndex =
                            std::min(POWER_ITEM_SCORE_COUNT - 1, m_PowerItemScoreIndex + 8);
                        itemScore = POWER_ITEM_SCORE[m_PowerItemScoreIndex];
                    } else {
                        m_PowerItemScoreIndex = 0;
                        gm.power              = std::min(128, gm.power + 8);
                    }
                    AddScoreWithExtend(gm, itemScore);
                    if (oldPower < 128 && gm.power >= 128) {
                        m_FullPowerActivated = true;
                        SpawnPickupLabel(item.m_Pos, "MAX POWER");
                        AudioManager::Instance().Play(SoundEffect::PowerUp);
                    } else {
                        SpawnPickupLabel(item.m_Pos, std::to_string(itemScore));
                    }
                    break;
                }
                case ItemType::Point: {
                    float fieldY = item.m_Pos.y - Util::FIELD_OFFSET_Y;
                    int   pts =
                        (fieldY < 128.0f)
                              ? 100000
                              : std::max(0, static_cast<int>(60000.0f - (fieldY - 128.0f) * 100.0f));
                    AddScoreWithExtend(gm, pts);
                    gm.AddPointItem();
                    SpawnPickupLabel(item.m_Pos, std::to_string(pts));
                    break;
                }
                case ItemType::PointBullet: {
                    const int pts = gm.PointBulletScore();
                    AddScoreWithExtend(gm, pts);
                    break;
                }
                case ItemType::Bomb:
                    gm.bombsRemaining = std::min(MAX_BOMBS, gm.bombsRemaining + 1);
                    break;
                case ItemType::FullPower:
                    if (gm.power < 128) {
                        gm.power              = 128;
                        m_FullPowerActivated = true;
                        SpawnPickupLabel(item.m_Pos, "MAX POWER");
                        AudioManager::Instance().Play(SoundEffect::PowerUp);
                    }
                    AddScoreWithExtend(gm, 1000);
                    SpawnPickupLabel(item.m_Pos, "1000");
                    break;
                case ItemType::Life:
                    gm.livesRemaining = std::min(MAX_LIVES, gm.livesRemaining + 1);
                    AudioManager::Instance().Play(SoundEffect::Extend);
                    break;
            }
            AudioManager::Instance().Play(SoundEffect::ItemPickup);
            item.m_Alive = false;
            if (item.m_Vm.obj) {
                m_Renderer.RemoveChild(item.m_Vm.obj);
            }
            continue;
        }

        if (!Util::IsInGameBounds(item.m_Pos.x, item.m_Pos.y, ITEM_HALF_SIZE, ITEM_HALF_SIZE, -200,
                                  -200, 600, 700)) {
            item.m_Alive = false;
            if (item.m_Vm.obj) {
                m_Renderer.RemoveChild(item.m_Vm.obj);
            }
        }
    }

    UpdatePickupLabels();
    m_Renderer.Update();
}

void ItemManager::Render() {
    m_Renderer.Update();
    RenderPickupLabels();
}

void ItemManager::RenderPickupLabels() {
    m_LabelRenderer.Update();
}
