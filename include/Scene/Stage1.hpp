#ifndef SCENE_STAGE1_HPP
#define SCENE_STAGE1_HPP

#include "Enemy/EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "Scene/Scene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Stage1 : public Scene {
   public:
    Stage1(CharacterItem character, SpellCardItem spellCard);
    void                   Update() override;
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 11600;
    static constexpr float BG_CANVAS_H        = 2250.0f;
    static constexpr float FIELD_H            = 448.0f;

    int                               m_StageFrame = 0;
    std::shared_ptr<Util::Image>      m_BgImage;
    std::shared_ptr<Util::GameObject> m_BgObj;

    GameManager  m_GameManager;
    Gui          m_Gui;
    EnemyManager m_EnemyManager;
    Player       m_Player;
};

#endif  // SCENE_STAGE1_HPP
