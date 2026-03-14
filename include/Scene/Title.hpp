#ifndef SCENE_TITLE_HPP
#define SCENE_TITLE_HPP

#include <memory>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Anm/AnmTypes.hpp"
#include "Util/GameObject.hpp"
#include "Util/BlackMask.hpp"

#include "Scene/Scene.hpp"

constexpr int TITLE_INTERRUPT_ENTER_TITLE = 1;
constexpr int TITLE_INTERRUPT_ENTER_MAINMENU = 2;
constexpr int TITLE_INTERRUPT_ENTER_OPTION = 3;
constexpr int TITLE_INTERRUPT_LEAVE_MAINMENU = 4;

enum class TitleState {
    Title,
    MainMenu,
};

enum class TitleMenuItem {
    Start,
    ExtraStart,
    PracticeStart,
    Replay,
    Score,
    MusicRoom,
    Option,
    Quit,
};

constexpr int TITLE_MENU_COUNT = static_cast<int>(TitleMenuItem::Quit) + 1;

class Title : public Scene {
public:
    Title();

    void Update() override;
    std::unique_ptr<Scene> NextScene() override;
    void LeaveMainMenu(bool quitGame);
private:
    TitleState m_CurrentState = TitleState::Title;
    TitleMenuItem m_SelectedMenuItem = TitleMenuItem::Start;

    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    std::vector<Anm::Vm*> m_UnselectedMenuItemVms;
    std::vector<std::shared_ptr<Util::GameObject>> m_UnselectedMenuItemObjs;

    std::shared_ptr<Util::GameObject> m_BgObj;
    Util::BlackMask m_MainMenuBlackMask;
    Util::BlackMask m_LeaveMainMenuBlackMask;

    int m_SelectedMenuItemIdx = 0;

    bool m_Quitting = false;
    int m_QuitTimer = 0; // quit timer for animating leave main menu transition
};

#endif
