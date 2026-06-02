#ifndef ITEM_MANAGER_HPP
#define ITEM_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>
#include <string>

#include "Anm/AnmManager.hpp"
#include "Util/AsciiTextLine.hpp"
#include "Util/Renderer.hpp"

struct GameManager;

enum class ItemType : int {
    PowerSmall  = 0,
    Point       = 1,
    PowerBig    = 2,
    Bomb        = 3,
    FullPower   = 4,
    Life        = 5,
    PointBullet = 6,
};

struct Item {
    Anm::Vm   m_Vm;
    glm::vec2 m_Pos;
    glm::vec2 m_Vel;
    glm::vec2 m_StartPos;
    glm::vec2 m_TargetPos;
    ItemType  m_Type;
    int       m_State = 0;
    int       m_Timer = 0;
    bool      m_Alive = false;
};

class ItemManager {
   public:
    ItemManager();
    void SpawnItem(glm::vec2 pos, ItemType type, int state = 0);
    void Update(glm::vec2 playerPos, GameManager& gm);
    void Render();
    void RenderPickupLabels();
    bool ConsumeFullPowerActivated();
    void SetTimeStopped(bool stopped) { m_TimeStopped = stopped; }

   private:
    static constexpr int MAX_ITEMS          = 512;
    static constexpr int MAX_PICKUP_LABELS  = 32;

    struct PickupLabel {
        bool                active = false;
        int                 timer  = 0;
        glm::vec2           pos    = {};
        Util::AsciiTextLine line;
    };

    void SpawnPickupLabel(glm::vec2 pos, const std::string& text);
    void UpdatePickupLabels();

    std::array<Item, MAX_ITEMS> m_Items{};
    int                         m_NextIdx = 0;
    int                         m_PowerItemScoreIndex = 0;

    std::array<PickupLabel, MAX_PICKUP_LABELS> m_PickupLabels{};
    int                                        m_NextLabelIdx = 0;

    Anm::Manager   m_Anm;
    Anm::Manager   m_AsciiAnm;
    Util::Renderer m_Renderer;
    Util::Renderer m_LabelRenderer;
    bool           m_TimeStopped        = false;
    bool           m_FullPowerActivated = false;
};

#endif  // ITEM_MANAGER_HPP
