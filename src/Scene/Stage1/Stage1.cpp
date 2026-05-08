#include "Scene/Stage1/Stage1.hpp"

#include <cstdio>
#include <memory>

#include "Scene/Stage1/Stage1Script.hpp"
#include "Scene/Stage2/Stage2.hpp"
#include "Scene/Title.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int        STAGE1_BOSS_SKIP_FRAME = 5279;
static constexpr int        CLEAR_LOADING_WAIT     = 0;
static constexpr int        CLEAR_LOADING_FADE_IN  = 60;
static constexpr int        CLEAR_LOADING_HOLD     = 240;
static constexpr int        CLEAR_LOADING_FADE_OUT = 60;
static constexpr int        CLEAR_LOADING_TOTAL =
    CLEAR_LOADING_WAIT + CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD + CLEAR_LOADING_FADE_OUT;
static constexpr int        BOSS_DEATH_RESULT_DELAY = 60;
static constexpr float      CLEAR_TEXT_CENTER_X = -96.0f;
static constexpr float      CLEAR_TITLE_SCALE   = 1.0f;
static constexpr float      CLEAR_LINE_SCALE    = 1.0f;
static const Util::Color    CLEAR_SUNSHINE_YELLOW = Util::Color::FromRGB(255, 255, 64);
static const Util::Color    CLEAR_WHITE           = Util::Color::FromRGB(255, 255, 255);
static const Util::Color    CLEAR_LAVENDER        = Util::Color::FromRGB(224, 224, 255);
static const Util::Color    CLEAR_LIGHT_BLUE      = Util::Color::FromRGB(208, 208, 255);
static const Util::Color    CLEAR_LIGHT_RED       = Util::Color::FromRGB(255, 128, 128);

static std::string FormatClearLine(const char* fmt, int value) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), fmt, value);
    return buf;
}

static float ClearLoadingAlpha(int timer) {
    const int local = timer - CLEAR_LOADING_WAIT;
    if (local < 0) return 0.0f;
    if (local < CLEAR_LOADING_FADE_IN) {
        return static_cast<float>(local) / static_cast<float>(CLEAR_LOADING_FADE_IN);
    }
    if (local < CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD) return 1.0f;
    if (local < CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD + CLEAR_LOADING_FADE_OUT) {
        return 1.0f - static_cast<float>(local - CLEAR_LOADING_FADE_IN - CLEAR_LOADING_HOLD) /
                          static_cast<float>(CLEAR_LOADING_FADE_OUT);
    }
    return 0.0f;
}

static float ClearLoadingImageAlpha(int timer) {
    const int local = timer - CLEAR_LOADING_WAIT;
    if (local < 0) return 0.0f;
    if (local < CLEAR_LOADING_FADE_IN) {
        return static_cast<float>(local) / static_cast<float>(CLEAR_LOADING_FADE_IN);
    }
    if (local < CLEAR_LOADING_TOTAL) return 1.0f;
    return 0.0f;
}

}  // namespace

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard)
    : PlayableStage(character, spellCard, {}, GA_RESOURCE_DIR "/stages/stage1_timeline.json",
                    std::make_unique<Stage1Script>(), "STAGE 1",
                    "A Dream More Scarlet than Red",
                    "BGM: A Soul as Red as a Ground Cherry") {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage1_bg.png", -10.0f, -96.0f, BG_CANVAS_H, FIELD_H,
        STAGE_TOTAL_FRAMES));

    m_ClearLines.reserve(8);
    m_ClearAnm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);
    m_ClearLoadingImage = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_CM/loading.png");
    m_ClearLoadingObj = std::make_shared<Util::GameObject>(m_ClearLoadingImage, 29.0f);
    m_ClearLoadingObj->m_Transform.scale = {1.5f, 1.75f};
    m_ClearLoadingObj->SetVisible(false);
    m_ClearRenderer.AddChild(m_ClearLoadingObj);
    for (int i = 0; i < 8; i++) {
        m_ClearLines.emplace_back();
        m_ClearLines.back().Configure(m_ClearRenderer, m_ClearAnm, 31.0f);
    }
}

int Stage1::BossSkipFrame() const { return STAGE1_BOSS_SKIP_FRAME; }

void Stage1::SetStageClearLine(size_t idx, const std::string& text, float y, float scale,
                               const Util::Color& color) {
    if (idx >= m_ClearLines.size()) return;

    m_ClearLines[idx].SetText(text, {CLEAR_TEXT_CENTER_X, y}, scale,
                              Util::AsciiTextAlign::Center, color);
    m_ClearLines[idx].SetVisible(true);
}

void Stage1::ShowStageClearText() {
    if (m_StageClearTextShown) return;
    const int stageBonus = 1000;
    const int powerBonus = m_GameManager.power * 100;
    const int grazeBonus = m_GameManager.graze * 10;

    SetStageClearLine(0, "Stage Clear", 112.0f, CLEAR_TITLE_SCALE, CLEAR_SUNSHINE_YELLOW);
    SetStageClearLine(1, FormatClearLine("Stage * 1000 = %5d", stageBonus), 80.0f,
                      CLEAR_LINE_SCALE, CLEAR_WHITE);
    SetStageClearLine(2, FormatClearLine("Power *  100 = %5d", powerBonus), 64.0f,
                      CLEAR_LINE_SCALE, CLEAR_LAVENDER);
    SetStageClearLine(3, FormatClearLine("Graze *   10 = %5d", grazeBonus), 48.0f,
                      CLEAR_LINE_SCALE, CLEAR_LIGHT_BLUE);
    SetStageClearLine(4, FormatClearLine("    * Point Item %3d", m_GameManager.pointItems),
                      32.0f, CLEAR_LINE_SCALE, CLEAR_LIGHT_RED);
    SetStageClearLine(5, "Normal Rank    * 1.0", 0.0f, CLEAR_LINE_SCALE, CLEAR_LIGHT_RED);
    SetStageClearLine(6, FormatClearLine("Total     = %8d", m_StageClearScore), -16.0f,
                      CLEAR_LINE_SCALE, CLEAR_WHITE);
    m_StageClearTextShown = true;
}

void Stage1::StartStageClear() {
    if (m_StageClearStarted) return;

    const int stageBonus = 1000;
    const int powerBonus = m_GameManager.power * 100;
    const int grazeBonus = m_GameManager.graze * 10;
    m_StageClearScore    = (stageBonus + powerBonus + grazeBonus) * m_GameManager.pointItems;
    m_GameManager.score += m_StageClearScore;
    if (m_GameManager.score > m_GameManager.highScore) {
        m_GameManager.highScore = m_GameManager.score;
    }

    m_StageClearStarted = true;
    m_StageClearTimer   = 0;
}

void Stage1::UpdateStageClearLoading() {
    if (!m_StageClearStarted) return;

    const float imageAlpha = ClearLoadingImageAlpha(m_StageClearTimer);
    const float textAlpha  = ClearLoadingAlpha(m_StageClearTimer);
    m_ClearLoadingObj->SetVisible(imageAlpha > 0.0f);
    m_ClearLoadingObj->SetAlpha(imageAlpha);
    m_ClearLoadingObj->m_Transform.translation = {-96.0f, 0.0f};

    if (!m_StageClearTextShown && m_StageClearTimer >= CLEAR_LOADING_FADE_IN) {
        ShowStageClearText();
    }

    const bool textVisible = m_StageClearTextShown && m_StageClearTimer < CLEAR_LOADING_TOTAL;
    for (auto& line : m_ClearLines) {
        line.SetVisible(textVisible);
        line.SetAlpha(textAlpha);
    }

    if (m_StageClearTimer >= CLEAR_LOADING_TOTAL) {
        m_ClearLoadingObj->SetVisible(false);
        for (auto& line : m_ClearLines) line.SetVisible(false);
    }
}

void Stage1::UpdateStageClear() {
    if (!m_StageClearStarted) return;

    UpdateStageClearLoading();
    m_ClearRenderer.Update();

    if (m_StageClearTextShown &&
        m_StageClearTimer > CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD &&
        (Util::Input::IsKeyDown(Util::Keycode::Z) ||
         Util::Input::IsKeyDown(Util::Keycode::RETURN))) {
        m_Done = true;
        return;
    }

    m_StageClearTimer++;
    if (m_StageClearTimer >= CLEAR_LOADING_TOTAL) {
        m_Done = true;
    }
}

void Stage1::OnMenuFrame() {
    UpdateStageClear();
}

bool Stage1::HandleStageOverlay() {
    if (!m_StageClearStarted) return false;
    m_Renderer.Update();
    m_Gui.Update(m_GameManager, {}, true);
    UpdateStageClear();
    return true;
}

void Stage1::OnAfterGameplayFrame(const BossHudState& bossHud) {
    if (m_StageFrame >= STAGE1_BOSS_SKIP_FRAME && bossHud.visible) {
        m_FinalBossWasSeen = true;
    } else if (m_FinalBossWasSeen && !bossHud.visible) {
        if (m_FinalBossClearDelay < 0) m_FinalBossClearDelay = BOSS_DEATH_RESULT_DELAY;
        if (m_FinalBossClearDelay == 0) {
            StartStageClear();
        } else {
            m_FinalBossClearDelay--;
        }
    }
}

void Stage1::OnFrameEnd() {
    UpdateStageClear();
}

std::unique_ptr<Scene> Stage1::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver()) return std::make_unique<Title>();
    return std::make_unique<Stage2>(m_Character, m_SpellCard, m_GameManager);
}
