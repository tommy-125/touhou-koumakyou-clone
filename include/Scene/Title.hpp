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
    Start,
    ExtraStart,
    PracticeStart,
    Replay,
    Score,
    MusicRoom,
    Option,
    Quit,
};

class Title : public Scene {
public:
    Title();

    void Update() override;

private:
    TitleState m_CurrentState = TitleState::Title;
    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    std::shared_ptr<Util::GameObject> m_BgObj;
};

#endif
