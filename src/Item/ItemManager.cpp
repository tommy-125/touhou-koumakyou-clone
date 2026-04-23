#include "Item/ItemManager.hpp"

#include <algorithm>

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
static constexpr float ITEM_HOMING_GAIN = 0.18f;

ItemManager::ItemManager() {
    m_Anm.LoadAnm(Anm::ETAMA3.folder, Anm::ETAMA3.txt, Anm::ETAMA3.offset);
}

void ItemManager::SpawnItem(glm::vec2 pos, ItemType type) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        int idx = (m_NextIdx + i) % MAX_ITEMS;

        if (!m_Items[idx].m_Alive) {
            Item& item   = m_Items[idx];
            item         = Item{};
            item.m_Alive = true;
            item.m_Pos   = pos;
            item.m_Vel   = {0.0f, ITEM_VEL_INIT_Y};
            item.m_Type  = type;
            m_NextIdx    = (idx + 1) % MAX_ITEMS;

            int scriptIdx = Anm::ETAMA3.offset + 21 + static_cast<int>(type);
            m_Anm.SetScript(item.m_Vm, scriptIdx, Anm::ETAMA3.offset);
            if (item.m_Vm.obj) m_Renderer.AddChild(item.m_Vm.obj);
            return;
        }
    }
}

void ItemManager::Update(glm::vec2 playerPos, GameManager& gm) {
    for (auto& item : m_Items) {
        if (!item.m_Alive) continue;

        item.m_Pos += item.m_Vel;
        if (item.m_Vel.y < ITEM_VEL_MAX_Y)
            item.m_Vel.y += ITEM_GRAVITY;
        else
            item.m_Vel.y = ITEM_VEL_MAX_Y;

        item.m_Vm.pos = item.m_Pos;
        m_Anm.UpdateObjects(item.m_Vm);

        if (playerPos.y <= ITEM_AUTOGET_Y) {
            glm::vec2 toPlayer = playerPos - item.m_Pos;
            item.m_Vel += toPlayer * ITEM_HOMING_GAIN;
            item.m_Vel.x = std::clamp(item.m_Vel.x, -10.0f, 10.0f);
            item.m_Vel.y = std::clamp(item.m_Vel.y, -10.0f, 10.0f);
        }

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
                    break;
                }
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
