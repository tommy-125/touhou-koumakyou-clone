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
#include "Scene/StageClearOverlay.hpp"
#include "Scene/StageMenu.hpp"
#include "Util/Color.hpp"
#include "Util/AsciiTextLine.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

struct PlayableStageConfig {
    std::string timelinePath;
    std::string stageNo;
    std::string stageName;
    std::string songName;
    float stageNameAdvanceScale = 0.8f;
    int bossSkipFrame = -1;
    int midbossSkipFrame = -1;
    int totalFrames = -1;
    int stageBonus = 0;
    bool hasStageClear = false;
    int bossDeathResultDelay = 60;
};

class PlayableStage : public Scene {
   public:
    PlayableStage(CharacterItem character, SpellCardItem spellCard, GameManager gameManager,
                  PlayableStageConfig config, std::unique_ptr<IStageScript> script);
    void Update() override;

   protected:
    virtual void UpdateBackground();
    virtual void OnAfterGameplayFrame(const BossHudState& bossHud);
    virtual void OnFrameEnd();
    virtual void OnMenuFrame();
    virtual bool HandleStageOverlay();
    virtual int  BossSkipFrame() const { return m_Config.bossSkipFrame; }

    void UpdateStageIntro();
    void SetBackground(std::unique_ptr<StageBackground> background);
    void UpdateFinalBossClearFlow(const BossHudState& bossHud);
    bool ShouldReturnToTitle() const { return m_ReturnToTitle; }
    bool WasGameOver() const { return m_GameOver; }
    bool StageClearStarted() const { return m_ClearOverlay.HasStarted(); }

    CharacterItem m_Character;
    SpellCardItem m_SpellCard;
    PlayableStageConfig m_Config;

    int          m_StageFrame = 0;
    GameManager  m_GameManager;
    Gui          m_Gui;
    ItemManager  m_ItemManager;
    EnemyManager m_EnemyManager;
    Player       m_Player;

   private:
    void SetupIntroAsciiLine(Util::AsciiTextLine& line, const std::string& text, glm::vec2 pos,
                             float scale, Util::AsciiTextAlign align,
                             const Util::Color& color, float advanceScale = 1.0f);
    void HandleDebugShortcuts();
    void StartBombClearWave(glm::vec2 origin);
    void UpdateBombClearWave();
    void DropPlayerPowerOnDeath(glm::vec2 pos);

    StageMenu                   m_StageMenu;
    Util::Renderer              m_IntroRenderer;
    Anm::Manager                m_IntroAnm;
    Util::AsciiTextLine         m_IntroStageNoLine;
    Util::AsciiTextLine         m_IntroStageNameLine;
    Util::AsciiTextLine         m_IntroSongLine;
    std::unique_ptr<StageBackground>  m_Background;
    StageClearOverlay                 m_ClearOverlay;
    bool                              m_ReturnToTitle = false;
    bool                              m_GameOver      = false;
    bool                              m_FinalBossWasSeen = false;
    bool                              m_BombClearWaveActive = false;
    glm::vec2                         m_BombClearWaveOrigin = {};
    int                               m_BombClearWaveTimer  = 0;
    int                               m_FinalBossClearDelay = -1;
};

#endif  // SCENE_PLAYABLE_STAGE_HPP
