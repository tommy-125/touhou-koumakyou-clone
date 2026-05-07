#ifndef SCENE_PLAYABLE_STAGE_HPP
#define SCENE_PLAYABLE_STAGE_HPP

#include <memory>
#include <string>

#include "Enemy/EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Item/ItemManager.hpp"
#include "Player.hpp"
#include "Scene/IStageScript.hpp"
#include "Scene/Scene.hpp"
#include "Scene/StageMenu.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"

class PlayableStage : public Scene {
   public:
    PlayableStage(CharacterItem character, SpellCardItem spellCard, GameManager gameManager,
                  const char* timelinePath, std::unique_ptr<IStageScript> script,
                  const std::string& stageNo, const std::string& stageName,
                  const std::string& songName);
    void Update() override;

   protected:
    virtual void UpdateBackground() = 0;
    virtual void OnAfterGameplayFrame(const BossHudState&) {}
    virtual void OnFrameEnd() {}
    virtual void OnMenuFrame() {}
    virtual bool HandleStageOverlay() { return false; }
    virtual int  BossSkipFrame() const { return -1; }

    void UpdateStageIntro();
    bool ShouldReturnToTitle() const { return m_ReturnToTitle; }

    CharacterItem m_Character;
    SpellCardItem m_SpellCard;

    int          m_StageFrame = 0;
    GameManager  m_GameManager;
    Gui          m_Gui;
    ItemManager  m_ItemManager;
    EnemyManager m_EnemyManager;
    Player       m_Player;

   private:
    static std::shared_ptr<Util::GameObject> MakeIntroText(std::shared_ptr<Util::Text>& text,
                                                           const std::string& str,
                                                           const Util::Color& color, int fontSize,
                                                           float x, float y);

    StageMenu                   m_StageMenu;
    Util::Renderer              m_IntroRenderer;
    std::shared_ptr<Util::Text> m_IntroStageNoText;
    std::shared_ptr<Util::Text> m_IntroStageNameText;
    std::shared_ptr<Util::Text> m_IntroSongText;
    std::shared_ptr<Util::GameObject> m_IntroStageNoObj;
    std::shared_ptr<Util::GameObject> m_IntroStageNameObj;
    std::shared_ptr<Util::GameObject> m_IntroSongObj;
    bool                              m_ReturnToTitle = false;
};

#endif  // SCENE_PLAYABLE_STAGE_HPP
