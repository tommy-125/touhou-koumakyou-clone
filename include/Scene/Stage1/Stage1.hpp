#ifndef SCENE_STAGE1_STAGE1_HPP
#define SCENE_STAGE1_STAGE1_HPP

#include <string>
#include <vector>

#include "Enemy/EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Item/ItemManager.hpp"
#include "Player.hpp"
#include "Scene/Scene.hpp"
#include "Scene/StageMenu.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"

class Stage1 : public Scene {
   public:
    Stage1(CharacterItem character, SpellCardItem spellCard);
    void                   Update() override;
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 11600;
    static constexpr float BG_CANVAS_H        = 2250.0f;
    static constexpr float FIELD_H            = 448.0f;

    void UpdateStageIntro();
    void StartStageClear();
    void UpdateStageClear();
    void UpdateStageClearLoading();
    void ShowStageClearText();
    void SetStageClearLine(size_t idx, const std::string& text, const Util::Color& color,
                           float y);

    int                               m_StageFrame = 0;
    std::shared_ptr<Util::Image>      m_BgImage;
    std::shared_ptr<Util::GameObject> m_BgObj;
    StageMenu                         m_StageMenu;
    Util::Renderer                    m_IntroRenderer;
    std::shared_ptr<Util::Text>       m_IntroStageNoText;
    std::shared_ptr<Util::Text>       m_IntroStageNameText;
    std::shared_ptr<Util::Text>       m_IntroSongText;
    std::shared_ptr<Util::GameObject> m_IntroStageNoObj;
    std::shared_ptr<Util::GameObject> m_IntroStageNameObj;
    std::shared_ptr<Util::GameObject> m_IntroSongObj;
    Util::Renderer                    m_ClearRenderer;
    std::shared_ptr<Util::Image>      m_ClearLoadingImage;
    std::shared_ptr<Util::GameObject> m_ClearLoadingObj;
    std::vector<std::shared_ptr<Util::Text>>       m_ClearTexts;
    std::vector<std::shared_ptr<Util::GameObject>> m_ClearObjs;
    bool m_FinalBossWasSeen   = false;
    bool m_StageClearStarted  = false;
    bool m_StageClearTextShown = false;
    int  m_StageClearTimer    = 0;
    int  m_StageClearScore    = 0;

    GameManager  m_GameManager;
    Gui          m_Gui;
    ItemManager  m_ItemManager;
    EnemyManager m_EnemyManager;
    Player       m_Player;
};

#endif  // SCENE_STAGE1_STAGE1_HPP
