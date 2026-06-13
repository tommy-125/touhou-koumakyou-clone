#include "Scene/StageClearOverlay.hpp"

#include <cstdio>

#include "Anm/AnmDefs.hpp"
#include "Audio/AudioManager.hpp"
#include "Util/Color.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int   CLEAR_LOADING_FADE_IN  = 60;
static constexpr int   CLEAR_LOADING_HOLD     = 240;
static constexpr int   CLEAR_LOADING_FADE_OUT = 60;
static constexpr int   CLEAR_LOADING_TOTAL =
    CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD + CLEAR_LOADING_FADE_OUT;
static constexpr float CLEAR_TEXT_CENTER_X = -96.0f;
static constexpr float CLEAR_TITLE_SCALE   = 1.0f;
static constexpr float CLEAR_LINE_SCALE    = 1.0f;

static const Util::Color CLEAR_SUNSHINE_YELLOW = Util::Color::FromRGB(255, 255, 64);
static const Util::Color CLEAR_WHITE           = Util::Color::FromRGB(255, 255, 255);
static const Util::Color CLEAR_LAVENDER        = Util::Color::FromRGB(224, 224, 255);
static const Util::Color CLEAR_LIGHT_BLUE      = Util::Color::FromRGB(208, 208, 255);
static const Util::Color CLEAR_LIGHT_RED       = Util::Color::FromRGB(255, 128, 128);

static std::string FormatClearLine(const char* fmt, int value) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), fmt, value);
    return buf;
}

static float ClearTextAlpha(int timer) {
    if (timer < CLEAR_LOADING_FADE_IN) {
        return static_cast<float>(timer) / static_cast<float>(CLEAR_LOADING_FADE_IN);
    }
    if (timer < CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD) return 1.0f;
    if (timer < CLEAR_LOADING_TOTAL) {
        return 1.0f - static_cast<float>(timer - CLEAR_LOADING_FADE_IN - CLEAR_LOADING_HOLD) /
                          static_cast<float>(CLEAR_LOADING_FADE_OUT);
    }
    return 0.0f;
}

static float ClearImageAlpha(int timer) {
    if (timer < CLEAR_LOADING_FADE_IN) {
        return static_cast<float>(timer) / static_cast<float>(CLEAR_LOADING_FADE_IN);
    }
    if (timer < CLEAR_LOADING_TOTAL) return 1.0f;
    return 0.0f;
}

void PlayExtends(int count) {
    for (int i = 0; i < count; ++i) {
        AudioManager::Instance().Play(SoundEffect::Extend);
    }
}

}  // namespace

void StageClearOverlay::Init() {
    if (m_Initialized) return;

    m_Lines.reserve(8);
    m_Anm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);
    m_LoadingImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_CM/loading.png");
    m_LoadingObj   = std::make_shared<Util::GameObject>(m_LoadingImage, 29.0f);
    m_LoadingObj->m_Transform.scale = {1.5f, 1.75f};
    m_LoadingObj->SetVisible(false);
    m_Renderer.AddChild(m_LoadingObj);

    for (int i = 0; i < 8; i++) {
        m_Lines.emplace_back();
        m_Lines.back().Configure(m_Renderer, m_Anm, 31.0f);
    }

    m_Initialized = true;
}

void StageClearOverlay::Start(GameManager& gm, int stageBonus) {
    Init();
    if (m_Started) return;

    const int powerBonus = gm.power * 100;
    const int grazeBonus = gm.stageGraze * 10;

    m_GameManager = &gm;
    m_StageBonus  = stageBonus;
    m_ClearScore  = (stageBonus + powerBonus + grazeBonus) * gm.stagePointItems;
    PlayExtends(gm.AddScore(m_ClearScore));

    m_Started   = true;
    m_TextShown = false;
    m_Finished  = false;
    m_Timer     = 0;
}

void StageClearOverlay::SetLine(size_t idx, const std::string& text, float y, float scale,
                                const Util::Color& color) {
    if (idx >= m_Lines.size()) return;
    m_Lines[idx].SetText(text, {CLEAR_TEXT_CENTER_X, y}, scale, Util::AsciiTextAlign::Center,
                         color);
    m_Lines[idx].SetVisible(true);
}

void StageClearOverlay::ShowText() {
    if (m_TextShown || !m_GameManager) return;

    const int powerBonus = m_GameManager->power * 100;
    const int grazeBonus = m_GameManager->stageGraze * 10;

    SetLine(0, "Stage Clear", 112.0f, CLEAR_TITLE_SCALE, CLEAR_SUNSHINE_YELLOW);
    SetLine(1, FormatClearLine("Stage * 1000 = %5d", m_StageBonus), 80.0f, CLEAR_LINE_SCALE,
            CLEAR_WHITE);
    SetLine(2, FormatClearLine("Power *  100 = %5d", powerBonus), 64.0f, CLEAR_LINE_SCALE,
            CLEAR_LAVENDER);
    SetLine(3, FormatClearLine("Graze *   10 = %5d", grazeBonus), 48.0f, CLEAR_LINE_SCALE,
            CLEAR_LIGHT_BLUE);
    SetLine(4, FormatClearLine("    * Point Item %3d", m_GameManager->stagePointItems), 32.0f,
            CLEAR_LINE_SCALE, CLEAR_LIGHT_RED);
    SetLine(5, "Normal Rank    * 1.0", 0.0f, CLEAR_LINE_SCALE, CLEAR_LIGHT_RED);
    SetLine(6, FormatClearLine("Total     = %8d", m_ClearScore), -16.0f, CLEAR_LINE_SCALE,
            CLEAR_WHITE);
    m_TextShown = true;
}

void StageClearOverlay::UpdateLoading() {
    if (!m_Started) return;

    const float imageAlpha = ClearImageAlpha(m_Timer);
    const float textAlpha  = ClearTextAlpha(m_Timer);
    m_LoadingObj->SetVisible(imageAlpha > 0.0f);
    m_LoadingObj->SetAlpha(imageAlpha);
    m_LoadingObj->m_Transform.translation = {-96.0f, 0.0f};

    if (!m_TextShown && m_Timer >= CLEAR_LOADING_FADE_IN) {
        ShowText();
    }

    const bool textVisible = m_TextShown && m_Timer < CLEAR_LOADING_TOTAL;
    for (auto& line : m_Lines) {
        line.SetVisible(textVisible);
        line.SetAlpha(textAlpha);
    }
}

bool StageClearOverlay::Update() {
    if (!m_Started || m_Finished) return m_Finished;

    UpdateLoading();
    m_Renderer.Update();

    if (m_TextShown && m_Timer > CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD &&
        (Util::Input::IsKeyDown(Util::Keycode::Z) ||
         Util::Input::IsKeyDown(Util::Keycode::RETURN))) {
        m_Finished = true;
        return true;
    }

    m_Timer++;
    if (m_Timer >= CLEAR_LOADING_TOTAL) {
        m_Finished = true;
    }
    return m_Finished;
}
