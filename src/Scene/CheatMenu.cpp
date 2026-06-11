#include "Scene/CheatMenu.hpp"

#include <string>

#include "Anm/AnmDefs.hpp"
#include "Audio/AudioManager.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int   CHEAT_MENU_INPUT_DELAY_FRAMES = 4;
static constexpr float CHEAT_MENU_TITLE_Y            = 112.0f;
static constexpr float CHEAT_MENU_FIRST_OPTION_Y     = 76.0f;
static constexpr float CHEAT_MENU_OPTION_STEP_Y      = 22.0f;
static constexpr float CHEAT_MENU_CENTER_X           = -96.0f;
static constexpr float CHEAT_MENU_OPTION_X           = -188.0f;
static constexpr float CHEAT_MENU_TITLE_SCALE        = 0.9f;
static constexpr float CHEAT_MENU_OPTION_SCALE       = 0.75f;

constexpr const char* CHEAT_MENU_OPTIONS[CheatMenu::OPTION_COUNT] = {
    "FULL POWER", "ADD LIFE", "ADD BOMB", "CLEAR BULLETS", "",
};

CheatMenu::Action ActionForIndex(int index) {
    switch (index) {
        case 0:
            return CheatMenu::Action::FullPower;
        case 1:
            return CheatMenu::Action::AddLife;
        case 2:
            return CheatMenu::Action::AddBomb;
        case 3:
            return CheatMenu::Action::ClearBullets;
        case 4:
            return CheatMenu::Action::ToggleInvincible;
        default:
            return CheatMenu::Action::None;
    }
}
}  // namespace

CheatMenu::CheatMenu() {
    m_Anm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);
    m_Renderer.AddChild(m_Mask.GetObj());

    for (auto& line : m_Lines) {
        line.Configure(m_Renderer, m_Anm, 46.0f);
        line.SetVisible(false);
    }
}

void CheatMenu::Open() {
    if (m_Open) return;

    m_Open          = true;
    m_SelectedIndex = 0;
    m_FramesOpen    = 0;
    m_Mask.Fade(6, 0.35f);
    RefreshText();
}

void CheatMenu::Close() {
    if (!m_Open) return;

    m_Open       = false;
    m_FramesOpen = 0;
    m_Mask.Fade(6, 0.0f);
    for (auto& line : m_Lines) {
        line.SetVisible(false);
    }
}

void CheatMenu::RefreshText() {
    m_Lines[0].SetText("CHEAT MENU", {CHEAT_MENU_CENTER_X, CHEAT_MENU_TITLE_Y},
                       CHEAT_MENU_TITLE_SCALE, Util::AsciiTextAlign::Center);
    m_Lines[0].SetVisible(m_Open);

    for (int i = 0; i < OPTION_COUNT; ++i) {
        const std::string prefix = i == m_SelectedIndex ? "> " : "  ";
        const std::string optionText =
            i == 4 ? (m_InvincibleEnabled ? "INVINCIBLE ON" : "INVINCIBLE OFF")
                   : CHEAT_MENU_OPTIONS[i];
        m_Lines[i + 1].SetText(
            prefix + optionText,
            {CHEAT_MENU_OPTION_X, CHEAT_MENU_FIRST_OPTION_Y - CHEAT_MENU_OPTION_STEP_Y * i},
            CHEAT_MENU_OPTION_SCALE, Util::AsciiTextAlign::Left);
        m_Lines[i + 1].SetAlpha(i == m_SelectedIndex ? 1.0f : 0.55f);
        m_Lines[i + 1].SetVisible(m_Open);
    }
}

CheatMenu::Action CheatMenu::Update() {
    m_Mask.Update();
    if (!m_Open) return Action::None;

    Action action = Action::None;
    if (m_FramesOpen >= CHEAT_MENU_INPUT_DELAY_FRAMES) {
        if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
            AudioManager::Instance().Play(SoundEffect::MenuMove);
            m_SelectedIndex = (m_SelectedIndex - 1 + OPTION_COUNT) % OPTION_COUNT;
            RefreshText();
        } else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
            AudioManager::Instance().Play(SoundEffect::MenuMove);
            m_SelectedIndex = (m_SelectedIndex + 1) % OPTION_COUNT;
            RefreshText();
        } else if (Util::Input::IsKeyDown(Util::Keycode::Z) ||
                   Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            AudioManager::Instance().Play(SoundEffect::MenuConfirm);
            action = ActionForIndex(m_SelectedIndex);
        } else if (Util::Input::IsKeyDown(Util::Keycode::X) ||
                   Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
            AudioManager::Instance().Play(SoundEffect::MenuBack);
            Close();
        }
    }

    ++m_FramesOpen;
    return action;
}

void CheatMenu::Render() {
    m_Renderer.Update();
}

void CheatMenu::SetInvincibleEnabled(bool enabled) {
    if (m_InvincibleEnabled == enabled) return;

    m_InvincibleEnabled = enabled;
    if (m_Open) RefreshText();
}
