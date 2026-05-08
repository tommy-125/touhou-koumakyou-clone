#ifndef SCENE_STAGE2_STAGE2_HPP
#define SCENE_STAGE2_STAGE2_HPP

#include <memory>
#include <string>
#include <vector>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"
#include "Util/AsciiTextLine.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"

class Stage2 : public PlayableStage {
   public:
    Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int STAGE_TOTAL_FRAMES = 7600;

    void OnAfterGameplayFrame(const BossHudState& bossHud) override;
    void OnFrameEnd() override;
    void OnMenuFrame() override;
    bool HandleStageOverlay() override;
    int  BossSkipFrame() const override;

    void StartStageClear();
    void UpdateStageClear();
    void UpdateStageClearLoading();
    void ShowStageClearText();
    void SetStageClearLine(size_t idx, const std::string& text, float y, float scale,
                           const Util::Color& color);

    Util::Renderer                    m_ClearRenderer;
    Anm::Manager                      m_ClearAnm;
    std::shared_ptr<Util::Image>      m_ClearLoadingImage;
    std::shared_ptr<Util::GameObject> m_ClearLoadingObj;
    std::vector<Util::AsciiTextLine>  m_ClearLines;
    bool m_FinalBossWasSeen    = false;
    bool m_StageClearStarted   = false;
    bool m_StageClearTextShown = false;
    int  m_FinalBossClearDelay = -1;
    int  m_StageClearTimer     = 0;
    int  m_StageClearScore     = 0;
};

#endif  // SCENE_STAGE2_STAGE2_HPP
