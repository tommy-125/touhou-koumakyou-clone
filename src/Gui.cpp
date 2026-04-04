#include "Gui.hpp"

#include <cstdio>
#include <string>

#include "Anm/AnmDefs.hpp"
#include "Util/Color.hpp"

static constexpr int        FONT_SIZE   = 12;
static constexpr const char FONT_PATH[] = PTSD_FONT_PATH;
static const Util::Color    TEXT_COLOR  = Util::Color::FromRGB(255, 255, 128);

static void SetupText(std::shared_ptr<Util::Text>& text, std::shared_ptr<Util::GameObject>& obj,
                      const std::string& str, float x, float y) {
    text = std::make_shared<Util::Text>(FONT_PATH, FONT_SIZE, str, TEXT_COLOR);
    obj  = std::make_shared<Util::GameObject>(text, 10.0f);
    obj->m_Transform.translation = {x, y};
}

Gui::Gui() {
    m_BgImage =
        std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_CM_output/front/bg.png");
    m_BgObj                          = std::make_shared<Util::GameObject>(m_BgImage, 8.0f);
    m_BgObj->m_Transform.translation = {0.0f, 0.0f};
    m_Renderer.AddChild(m_BgObj);

    const int off = Anm::FRONT.offset;
    m_Anm.LoadAnm(Anm::FRONT.folder, Anm::FRONT.txt, off);

    // Animated decorations: scripts 0–5
    for (int i = 0; i < NUM_DECO; i++) {
        m_Anm.SetScript(m_DecoVms[i], off + i, off);
        m_DecoVms[i].zIndex = 10.0f;
        if (m_DecoVms[i].obj) {
            m_DecoVms[i].obj->SetZIndex(10.0f);
            m_Renderer.AddChild(m_DecoVms[i].obj);
        }
    }

    // HUD label sprites: scripts 9–15
    for (int i = 0; i < NUM_LABEL; i++) {
        m_Anm.SetScript(m_LabelVms[i], off + 9 + i, off);
        m_LabelVms[i].zIndex = 10.0f;
        if (m_LabelVms[i].obj) {
            m_LabelVms[i].obj->SetZIndex(10.0f);
            m_Renderer.AddChild(m_LabelVms[i].obj);
        }
    }

    // Life icons: script 16, one per slot, positions set manually
    for (int i = 0; i < MAX_LIVES; i++) {
        m_Anm.SetScript(m_LifeVms[i], off + 16, off);
        m_LifeVms[i].pos    = {496.0f + i * 16.0f, 122.0f};
        m_LifeVms[i].zIndex = 10.0f;
        if (m_LifeVms[i].obj) {
            m_Anm.UpdateObjects(m_LifeVms[i]);
            m_LifeVms[i].obj->SetZIndex(10.0f);
            m_LifeVms[i].obj->SetVisible(false);
            m_Renderer.AddChild(m_LifeVms[i].obj);
        }
    }

    // Bomb icons: script 17
    for (int i = 0; i < MAX_BOMBS; i++) {
        m_Anm.SetScript(m_BombVms[i], off + 17, off);
        m_BombVms[i].pos    = {496.0f + i * 16.0f, 146.0f};
        m_BombVms[i].zIndex = 10.0f;
        if (m_BombVms[i].obj) {
            m_Anm.UpdateObjects(m_BombVms[i]);
            m_BombVms[i].obj->SetZIndex(10.0f);
            m_BombVms[i].obj->SetVisible(false);
            m_Renderer.AddChild(m_BombVms[i].obj);
        }
    }

    // Text value objects (PTSD coords = th06 - (320, -240))
    // HiScore: th06(496,58)  → PTSD(176,182)
    // Score:   th06(496,82)  → PTSD(176,158)
    // Power:   th06(496,186) → PTSD(176,54)
    // Graze:   th06(496,206) → PTSD(176,34)
    SetupText(m_HiScoreText, m_HiScoreObj, "000000000", 230.0f, 182.0f);
    SetupText(m_ScoreText, m_ScoreObj, "000000000", 230.0f, 158.0f);
    SetupText(m_PowerText, m_PowerObj, "0", 218.0f, 54.0f);
    SetupText(m_GrazeText, m_GrazeObj, "0", 218.0f, 34.0f);

    m_Renderer.AddChild(m_HiScoreObj);
    m_Renderer.AddChild(m_ScoreObj);
    m_Renderer.AddChild(m_PowerObj);
    m_Renderer.AddChild(m_GrazeObj);
}

void Gui::Update(const GameManager& gm) {
    for (auto& vm : m_DecoVms) m_Anm.UpdateObjects(vm);
    for (auto& vm : m_LabelVms) m_Anm.UpdateObjects(vm);

    if (m_LastLives != gm.livesRemaining) {
        m_LastLives = gm.livesRemaining;
        for (int i = 0; i < MAX_LIVES; i++)
            if (m_LifeVms[i].obj) m_LifeVms[i].obj->SetVisible(i < gm.livesRemaining);
    }

    if (m_LastBombs != gm.bombsRemaining) {
        m_LastBombs = gm.bombsRemaining;
        for (int i = 0; i < MAX_BOMBS; i++)
            if (m_BombVms[i].obj) m_BombVms[i].obj->SetVisible(i < gm.bombsRemaining);
    }

    if (m_LastScore != gm.score) {
        m_LastScore = gm.score;
        char buf[16];
        snprintf(buf, sizeof(buf), "%09d", gm.score);
        m_ScoreText->SetText(buf);
    }

    if (m_LastHiScore != gm.highScore) {
        m_LastHiScore = gm.highScore;
        char buf[16];
        snprintf(buf, sizeof(buf), "%09d", gm.highScore);
        m_HiScoreText->SetText(buf);
    }

    if (m_LastPower != gm.power) {
        m_LastPower = gm.power;
        m_PowerText->SetText(std::to_string(gm.power));
    }

    if (m_LastGraze != gm.graze) {
        m_LastGraze = gm.graze;
        m_GrazeText->SetText(std::to_string(gm.graze));
    }

    m_Renderer.Update();
}
