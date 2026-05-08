#ifndef SCENE_PLAYABLE_STAGE_HPP
#define SCENE_PLAYABLE_STAGE_HPP

#include <memory>
#include <string>

#include "Anm/AnmManager.hpp"
#include "Enemy/EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Item/ItemManager.hpp"
#include "Player.hpp"
#include "Scene/IStageScript.hpp"
#include "Scene/Scene.hpp"
#include "Scene/StageBackground.hpp"
#include "Scene/StageMenu.hpp"
#include "Util/Color.hpp"
#include "Util/AsciiTextLine.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

class PlayableStage : public Scene {
   public:
    PlayableStage(CharacterItem character, SpellCardItem spellCard, GameManager gameManager,
                  const char* timelinePath, std::unique_ptr<IStageScript> script,
                  const std::string& stageNo, const std::string& stageName,
                  const std::string& songName);
    void Update() override;

   protected:
    virtual void UpdateBackground();
    virtual void OnAfterGameplayFrame(const BossHudState&) {}
    virtual void OnFrameEnd() {}
    virtual void OnMenuFrame() {}
    virtual bool HandleStageOverlay() { return false; }
    virtual int  BossSkipFrame() const { return -1; }

    void UpdateStageIntro();
    void SetBackground(std::unique_ptr<StageBackground> background);
    bool ShouldReturnToTitle() const { return m_ReturnToTitle; }
    bool WasGameOver() const { return m_GameOver; }

    CharacterItem m_Character;
    SpellCardItem m_SpellCard;

    int          m_StageFrame = 0;
    GameManager  m_GameManager;
    Gui          m_Gui;
    ItemManager  m_ItemManager;
    EnemyManager m_EnemyManager;
    Player       m_Player;

   private:
    void SetupIntroAsciiLine(Util::AsciiTextLine& line, const std::string& text, glm::vec2 pos,
                             float scale, Util::AsciiTextAlign align,
                             const Util::Color& color);

    StageMenu                   m_StageMenu;
    Util::Renderer              m_IntroRenderer;
    Anm::Manager                m_IntroAnm;
    Util::AsciiTextLine         m_IntroStageNoLine;
    Util::AsciiTextLine         m_IntroStageNameLine;
    Util::AsciiTextLine         m_IntroSongLine;
    std::unique_ptr<StageBackground>  m_Background;
    bool                              m_ReturnToTitle = false;
    bool                              m_GameOver      = false;
};

#endif  // SCENE_PLAYABLE_STAGE_HPP
