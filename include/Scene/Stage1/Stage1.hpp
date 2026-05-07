#ifndef SCENE_STAGE1_STAGE1_HPP
#define SCENE_STAGE1_STAGE1_HPP

#include <string>
#include <vector>

#include "Scene/PlayableStage.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"

class Stage1 : public PlayableStage {
   public:
    Stage1(CharacterItem character, SpellCardItem spellCard);
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 11600;
    static constexpr float BG_CANVAS_H        = 2250.0f;
    static constexpr float FIELD_H            = 448.0f;

    void UpdateBackground() override;
    void OnAfterGameplayFrame(const BossHudState& bossHud) override;
    void OnFrameEnd() override;
    void OnMenuFrame() override;
    bool HandleStageOverlay() override;
    int  BossSkipFrame() const override;

    void StartStageClear();
    void UpdateStageClear();
    void UpdateStageClearLoading();
    void ShowStageClearText();
    void SetStageClearLine(size_t idx, const std::string& text, const Util::Color& color,
                           float y);

    std::shared_ptr<Util::Image>      m_BgImage;
    std::shared_ptr<Util::GameObject> m_BgObj;
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
};

#endif  // SCENE_STAGE1_STAGE1_HPP
