#include "Scene/Title.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"

Title::Title() {
    // Background
    auto bgImage = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/title00.jpg"
    );
    m_BgObj = std::make_shared<Util::GameObject>(bgImage, 0.0f);
    m_Renderer.AddChild(m_BgObj);

    // Load ANM
    std::vector<Anm::LoadedEntry> loaded = {
        {&Anm::TITLE01, m_Anm.LoadAnm(Anm::TITLE01.folder, Anm::TITLE01.txt, Anm::TITLE01.offset)},
        {&Anm::TITLE01S, m_Anm.LoadAnm(Anm::TITLE01S.folder, Anm::TITLE01S.txt, Anm::TITLE01S.offset)},
        {&Anm::TITLE02, m_Anm.LoadAnm(Anm::TITLE02.folder, Anm::TITLE02.txt, Anm::TITLE02.offset)},
        {&Anm::TITLE03, m_Anm.LoadAnm(Anm::TITLE03.folder, Anm::TITLE03.txt, Anm::TITLE03.offset)},
    };

    int total = 0;
    for (auto &e : loaded) total += e.scriptCount;
    m_Vms.resize(total);
    m_Objs.resize(total);

    // Init each VM and GameObject, then queue interrupt 1 to start the animation
    int vmIdx = 0;
    for (auto &e : loaded) {
        for (int i = 0; i < e.scriptCount; i++, vmIdx++) {
            m_Vms[vmIdx].scriptIdx    = e.entry->offset + i;
            m_Vms[vmIdx].spriteOffset = e.entry->offset;

            m_Objs[vmIdx] = std::make_shared<Util::GameObject>(nullptr, 1.0f, glm::vec2{0, 0}, false);
            m_Renderer.AddChild(m_Objs[vmIdx]);

            m_Anm.SendInterrupt(m_Vms[vmIdx], 1);
        }
    }

    m_MenuVmStartIdx = 0; // start idx of TITLE01S's VM
}

void Title::Update() {
    switch (m_CurrentState) {
        case TitleState::Title:
            if(Util::Input::IsKeyDown(Util::Keycode::Z) || Util::Input::IsKeyDown(Util::Keycode::X)) {
                m_CurrentState = TitleState::MainMenu;
                for (auto &vm : m_Vms) {
                    m_Anm.SendInterrupt(vm, TITLE_INTERRUPT_ENTER_MAINMENU);
                }
            }
            break;
        case TitleState::MainMenu: {
            if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
                m_SelectedMenuIdx = (m_SelectedMenuIdx - 1 + TITLE_MENU_COUNT) % TITLE_MENU_COUNT; // add TITLE_MENU_COUNT before mod to avoid negative
                if(static_cast<TitleMenuItem>(m_SelectedMenuIdx) == TitleMenuItem::ExtraStart) { // skip ExtraStart
                    m_SelectedMenuIdx = (m_SelectedMenuIdx - 1 + TITLE_MENU_COUNT) % TITLE_MENU_COUNT;
                }

            } else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
                m_SelectedMenuIdx = (m_SelectedMenuIdx + 1) % TITLE_MENU_COUNT; // wrap around
                if(static_cast<TitleMenuItem>(m_SelectedMenuIdx) == TitleMenuItem::ExtraStart) { // skip ExtraStart
                    m_SelectedMenuIdx = (m_SelectedMenuIdx + 1) % TITLE_MENU_COUNT; 
                }
            }
            m_SelectedMenuItem = static_cast<TitleMenuItem>(m_SelectedMenuIdx);
            
            switch(m_SelectedMenuItem) {
                case TitleMenuItem::Start:
                case TitleMenuItem::ExtraStart:
                case TitleMenuItem::PracticeStart:
                case TitleMenuItem::Replay:
                case TitleMenuItem::Score:
                case TitleMenuItem::MusicRoom:
                case TitleMenuItem::Option:
                    break;
                case TitleMenuItem::Quit:
                    if(Util::Input::IsKeyDown(Util::Keycode::Z) || Util::Input::IsKeyDown(Util::Keycode::X)) {
                        m_Done = true;
                        
                    }
                    break;
            }
        }
    }

    // Update each VM and corresponding GameObject
    for (int i = 0; i < static_cast<int>(m_Vms.size()); i++) {
        m_Anm.ExecuteScript(m_Vms[i]);

        const auto &vm  = m_Vms[i];
        auto       &obj = *m_Objs[i];

        obj.SetVisible(vm.isVisible);
        obj.SetAlpha(0.5f);
        if (m_Anm.sprites[vm.spriteIdx].image) {
            obj.SetDrawable(m_Anm.sprites[vm.spriteIdx].image);
        }
        
        
        if (i >= m_MenuVmStartIdx && i < m_MenuVmStartIdx + TITLE_MENU_COUNT) {
            // TITLE01S range: show the currently selected menu item
            if (i == m_MenuVmStartIdx + static_cast<int>(m_SelectedMenuItem)) {
                int spriteIdx = vm.spriteIdx - vm.spriteOffset + Anm::TITLE01S.offset;
                if (m_Anm.sprites[spriteIdx].image) {
                    obj.SetDrawable(m_Anm.sprites[spriteIdx].image);
                }
            }
            else {
                obj.SetAlpha(0.5f); // dim unselected menu items
            }
        }

        glm::vec2 translation = Anm::Manager::ToPtsd(vm.pos);
        if (vm.anchorTopLeft) {
            const auto &spr = m_Anm.sprites[vm.spriteIdx];
            translation += glm::vec2{spr.width / 2.0f, -spr.height / 2.0f};
        }
        obj.m_Transform.translation = translation;
        obj.m_Transform.scale       = vm.scale;
        obj.m_Transform.rotation    = vm.rotation;
    }

    m_Renderer.Update();
}

std::unique_ptr<Scene> Title::NextScene() {
    if (m_Done) {
        switch(m_SelectedMenuItem) {
            case TitleMenuItem::Start:
            case TitleMenuItem::ExtraStart:
            case TitleMenuItem::PracticeStart:
            case TitleMenuItem::Replay:
            case TitleMenuItem::Score:
            case TitleMenuItem::MusicRoom:
            case TitleMenuItem::Option:
            case TitleMenuItem::Quit:
                return nullptr;
        }

    }
    return nullptr;
}