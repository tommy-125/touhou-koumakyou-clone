#ifndef ITEM_MANAGER_HPP
#define ITEM_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>

#include "Anm/AnmManager.hpp"
#include "Util/Renderer.hpp"

struct GameManager;

enum class ItemType : int {
    PowerSmall = 0,
    Point      = 1,
    PowerBig   = 2,
};

struct Item {
    Anm::Vm   m_Vm;
    glm::vec2 m_Pos;
    glm::vec2 m_Vel;
    ItemType  m_Type;
    bool      m_Alive = false;
};

class ItemManager {
   public:
    ItemManager();
    void SpawnItem(glm::vec2 pos, ItemType type);
    void Update(glm::vec2 playerPos, GameManager& gm);

   private:
    static constexpr int MAX_ITEMS = 512;

    std::array<Item, MAX_ITEMS> m_Items{};
    int                         m_NextIdx = 0;

    Anm::Manager   m_Anm;
    Util::Renderer m_Renderer;
};

#endif  // ITEM_MANAGER_HPP
