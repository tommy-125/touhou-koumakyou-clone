#include "Scene/StageMenu.hpp"

#include "Anm/AnmDefs.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int GAME_MENU_SPRITE_TITLE_PAUSE   = 0;
static constexpr int GAME_MENU_SPRITE_CURSOR_RESUME = 1;
static constexpr int GAME_MENU_SPRITE_CURSOR_QUIT   = 2;
static constexpr int GAME_MENU_SPRITE_TITLE_QUIT    = 3;
static constexpr int GAME_MENU_SPRITE_CURSOR_YES    = 4;
static constexpr int GAME_MENU_SPRITE_CURSOR_NO     = 5;
static constexpr int GAME_MENU_INPUT_DELAY_FRAMES   = 4;
static constexpr int GAME_MENU_CLOSE_DELAY_FRAMES   = 20;
}

StageMenu::StageMenu(Util::Renderer& renderer) : m_Renderer(renderer) {
    m_Anm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);
    m_Renderer.AddChild(m_Mask.GetObj());

    for (int i = 0; i < VM_COUNT; ++i) {
        m_Anm.SetScript(m_Vms[i], Anm::ASCII.offset + 2 + i, Anm::ASCII.offset);
        m_Vms[i].zIndex = 41.0f;
        if (m_Vms[i].obj) {
            m_Vms[i].obj->SetVisible(false);
            m_Renderer.AddChild(m_Vms[i].obj);
        }
    }
}

void StageMenu::ShowScripts(int startIdx, int count) {
    for (int i = startIdx; i < startIdx + count; ++i) {
        m_Anm.SetScript(m_Vms[i], Anm::ASCII.offset + 2 + i, Anm::ASCII.offset);
        m_Vms[i].zIndex = 41.0f;
        m_Anm.SendInterrupt(m_Vms[i], 1);
    }
}

void StageMenu::HideScripts(int startIdx, int count) {
    for (int i = startIdx; i < startIdx + count; ++i) {
        m_Anm.SendInterrupt(m_Vms[i], 2);
    }
}

void StageMenu::OpenPauseMenu() {
    m_State        = State::PauseMenu;
    m_MenuIndex    = 0;
    m_ConfirmIndex = 1;
    m_StateFrames  = 0;
    m_Mask.Fade(6, 0.45f);
    ShowScripts(0, 3);
    HideScripts(3, 3);
}

void StageMenu::Close() {
    m_State = State::Closed;
    m_StateFrames = 0;
    m_Mask.Fade(6, 0.0f);
    HideScripts(0, 3);
    HideScripts(3, 3);
}

void StageMenu::Toggle() {
    if (m_State == State::Closed) {
        OpenPauseMenu();
    } else {
        HideScripts(0, 3);
        HideScripts(3, 3);
        m_State = State::ClosingResume;
        m_StateFrames = 0;
    }
}

void StageMenu::UpdateInput() {
    if (m_State == State::PauseMenu) {
        if (m_StateFrames < GAME_MENU_INPUT_DELAY_FRAMES) return;

        if (Util::Input::IsKeyDown(Util::Keycode::UP) ||
            Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
            m_MenuIndex = 1 - m_MenuIndex;
        }

        if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
            if (m_MenuIndex == 0) {
                HideScripts(0, 3);
                m_State = State::ClosingResume;
                m_StateFrames = 0;
            } else {
                HideScripts(0, 3);
                ShowScripts(3, 3);
                m_State        = State::ConfirmQuit;
                m_ConfirmIndex = 1;
                m_StateFrames  = 0;
            }
        }
        return;
    }

    if (m_State == State::ConfirmQuit) {
        if (m_StateFrames < GAME_MENU_INPUT_DELAY_FRAMES) return;

        if (Util::Input::IsKeyDown(Util::Keycode::UP) ||
            Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
            m_ConfirmIndex = 1 - m_ConfirmIndex;
        }

        if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
            if (m_ConfirmIndex == 0) {
                HideScripts(3, 3);
                m_State = State::ClosingQuit;
                m_StateFrames = 0;
            } else {
                HideScripts(3, 3);
                ShowScripts(0, 3);
                m_State = State::PauseMenu;
                m_StateFrames = 0;
            }
        }
    }
}

void StageMenu::ApplySelectionVisuals() {
    if (m_Vms[GAME_MENU_SPRITE_TITLE_PAUSE].obj) {
        m_Vms[GAME_MENU_SPRITE_TITLE_PAUSE].obj->SetVisible(m_State == State::PauseMenu);
    }
    if (m_Vms[GAME_MENU_SPRITE_TITLE_QUIT].obj) {
        m_Vms[GAME_MENU_SPRITE_TITLE_QUIT].obj->SetVisible(m_State == State::ConfirmQuit);
    }

    for (int idx : {GAME_MENU_SPRITE_CURSOR_RESUME, GAME_MENU_SPRITE_CURSOR_QUIT}) {
        auto& vm = m_Vms[idx];
        if (!vm.obj) continue;
        const bool selected = (idx == GAME_MENU_SPRITE_CURSOR_RESUME && m_MenuIndex == 0) ||
                              (idx == GAME_MENU_SPRITE_CURSOR_QUIT && m_MenuIndex == 1);
        vm.obj->SetVisible(m_State == State::PauseMenu);
        vm.obj->SetAlpha(selected ? 1.0f : 0.5f);
        vm.obj->m_Transform.scale       = {selected ? 1.7f : 1.5f, selected ? 1.7f : 1.5f};
        vm.obj->m_Transform.translation = Anm::Manager::ToPtsd(vm.pos) +
                                          glm::vec2{selected ? -4.0f : 0.0f,
                                                    selected ? 4.0f : 0.0f};
    }

    for (int idx : {GAME_MENU_SPRITE_CURSOR_YES, GAME_MENU_SPRITE_CURSOR_NO}) {
        auto& vm = m_Vms[idx];
        if (!vm.obj) continue;
        const bool selected = (idx == GAME_MENU_SPRITE_CURSOR_YES && m_ConfirmIndex == 0) ||
                              (idx == GAME_MENU_SPRITE_CURSOR_NO && m_ConfirmIndex == 1);
        vm.obj->SetVisible(m_State == State::ConfirmQuit);
        vm.obj->SetAlpha(selected ? 1.0f : 0.5f);
        vm.obj->m_Transform.scale       = {selected ? 1.7f : 1.5f, selected ? 1.7f : 1.5f};
        vm.obj->m_Transform.translation = Anm::Manager::ToPtsd(vm.pos) +
                                          glm::vec2{selected ? -4.0f : 0.0f,
                                                    selected ? 4.0f : 0.0f};
    }
}

StageMenu::Action StageMenu::Update() {
    if (m_State != State::Closed) {
        UpdateInput();
        for (auto& vm : m_Vms) {
            m_Anm.UpdateObjects(vm);
        }
        ApplySelectionVisuals();

        if (m_State == State::ClosingResume && m_StateFrames >= GAME_MENU_CLOSE_DELAY_FRAMES) {
            Close();
            m_PendingAction = Action::Resume;
        } else if (m_State == State::ClosingQuit &&
                   m_StateFrames >= GAME_MENU_CLOSE_DELAY_FRAMES) {
            Close();
            m_PendingAction = Action::ReturnToTitle;
        }

        ++m_StateFrames;
    }

    m_Mask.Update();
    const Action action = m_PendingAction;
    m_PendingAction     = Action::None;
    return action;
}
