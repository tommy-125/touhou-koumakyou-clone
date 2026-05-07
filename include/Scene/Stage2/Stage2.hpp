#ifndef SCENE_STAGE2_STAGE2_HPP
#define SCENE_STAGE2_STAGE2_HPP

#include <array>
#include <memory>

#include "Enemy/EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Item/ItemManager.hpp"
#include "Player.hpp"
#include "Scene/Scene.hpp"
#include "Scene/StageMenu.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"

class Stage2 : public Scene {
   public:
    Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    void                   Update() override;
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int STAGE_TOTAL_FRAMES = 7600;

    void UpdateStageIntro();
    void UpdateBackground();

    CharacterItem m_Character;
    SpellCardItem m_SpellCard;

    int                               m_StageFrame = 0;
    std::shared_ptr<Util::Image>      m_BgImage;
    std::array<std::shared_ptr<Util::GameObject>, 3> m_BgObjs;
    StageMenu                         m_StageMenu;
    Util::Renderer                    m_IntroRenderer;
    std::shared_ptr<Util::Text>       m_IntroStageNoText;
    std::shared_ptr<Util::Text>       m_IntroStageNameText;
    std::shared_ptr<Util::Text>       m_IntroSongText;
    std::shared_ptr<Util::GameObject> m_IntroStageNoObj;
    std::shared_ptr<Util::GameObject> m_IntroStageNameObj;
    std::shared_ptr<Util::GameObject> m_IntroSongObj;
    bool                              m_FinalBossWasSeen = false;

    GameManager  m_GameManager;
    Gui          m_Gui;
    ItemManager  m_ItemManager;
    EnemyManager m_EnemyManager;
    Player       m_Player;
};

#endif  // SCENE_STAGE2_STAGE2_HPP
