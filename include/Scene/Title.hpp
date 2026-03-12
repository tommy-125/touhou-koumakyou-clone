#ifndef SCENE_TITLE_HPP
#define SCENE_TITLE_HPP

#include <memory>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Util/GameObject.hpp"

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
private:
    TitleState m_CurrentState = TitleState::Title;
    TitleMenuItem m_SelectedMenuItem = TitleMenuItem::Start;

    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    std::shared_ptr<Util::GameObject> m_BgObj;

    int m_MenuVmStartIdx;
    int m_SelectedMenuIdx = 0;
};

#endif
