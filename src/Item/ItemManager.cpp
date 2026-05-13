#include "Item/ItemManager.hpp"

#include <algorithm>
#include <cstdlib>
#include <cmath>

#include "Anm/AnmDefs.hpp"
#include "GameManager.hpp"
#include "Util/Math.hpp"

static constexpr float ITEM_GRAB_HALF_X = 12.0f;
static constexpr float ITEM_GRAB_HALF_Y = 12.0f;
static constexpr float ITEM_HALF_SIZE   = 8.0f;
static constexpr float ITEM_VEL_INIT_Y  = -2.2f;
static constexpr float ITEM_GRAVITY     = 0.03f;
static constexpr float ITEM_VEL_MAX_Y   = 3.0f;
static constexpr float ITEM_AUTOGET_Y   = Util::FIELD_OFFSET_Y + 128.0f;

ItemManager::ItemManager() {
    m_Anm.LoadAnm(Anm::ETAMA3.folder, Anm::ETAMA3.txt, Anm::ETAMA3.offset);
}

void ItemManager::SpawnItem(glm::vec2 pos, ItemType type, int state) {
    if ((type == ItemType::PowerSmall || type == ItemType::PowerBig) && m_GameManager &&
        m_GameManager->power >= 128) {
        type = ItemType::Point;
    }

    for (int i = 0; i < MAX_ITEMS; i++) {
        int idx = (m_NextIdx + i) % MAX_ITEMS;

        if (!m_Items[idx].m_Alive) {
            Item& item   = m_Items[idx];
            item         = Item{};
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
                case ItemType::PowerSmall:
                    gm.power = std::min(128, gm.power + 1);
                    gm.score += 10;
                    break;
                case ItemType::PowerBig:
                    gm.power = std::min(128, gm.power + 8);
                    gm.score += 10;
                    break;
                case ItemType::Point: {
                    float fieldY = item.m_Pos.y - Util::FIELD_OFFSET_Y;
                    int   pts =
                        (fieldY < 128.0f)
                              ? 100000
                              : std::max(0, static_cast<int>(60000.0f - (fieldY - 128.0f) * 100.0f));
                    gm.score += pts;
                    gm.pointItems += 1;
                    break;
                }
                case ItemType::PointBullet: {
                    int pts = (gm.graze / 3) * 10 + 500;
                    if (pts < 100) pts = 100;
                    gm.score += pts;
                    break;
                }
                case ItemType::Bomb:
                case ItemType::FullPower:
                case ItemType::Life:
                    break;
            }
            if (gm.score > gm.highScore) gm.highScore = gm.score;
            item.m_Alive = false;
            if (item.m_Vm.obj) {
                m_Renderer.RemoveChild(item.m_Vm.obj);
                item.m_Vm.obj = nullptr;
            }
            continue;
        }

        if (!Util::IsInGameBounds(item.m_Pos.x, item.m_Pos.y, ITEM_HALF_SIZE, ITEM_HALF_SIZE, -200,
                                  -200, 600, 700)) {
            item.m_Alive = false;
            if (item.m_Vm.obj) {
                m_Renderer.RemoveChild(item.m_Vm.obj);
                item.m_Vm.obj = nullptr;
            }
        }
    }

    m_Renderer.Update();
}
