#ifndef SCENE_STAGE_MENU_HPP
#define SCENE_STAGE_MENU_HPP

#include "Anm/AnmManager.hpp"
#include "Util/BlackMask.hpp"
#include "Util/Renderer.hpp"

class StageMenu {
   public:
    enum class Action {
        None,
        Resume,
        ReturnToTitle,
    };

    explicit StageMenu(Util::Renderer& renderer);

    void   Toggle();
    Action Update();
    bool   IsOpen() const { return m_State != State::Closed; }

   private:
    enum class State {
        Closed,
        PauseMenu,
        ConfirmQuit,
        ClosingResume,
        ClosingQuit,
    };

    static constexpr int VM_COUNT = 6;

    void OpenPauseMenu();
    void Close();
    void ShowScripts(int startIdx, int count);
    void HideScripts(int startIdx, int count);
    void UpdateInput();
    void ApplySelectionVisuals();

    Util::Renderer& m_Renderer;
    Anm::Manager    m_Anm;
    Util::BlackMask m_Mask = Util::BlackMask(40.0f, 0.0f);
    Anm::Vm         m_Vms[VM_COUNT];
    State           m_State = State::Closed;
    int             m_MenuIndex = 0;
    int             m_ConfirmIndex = 1;
    int             m_StateFrames = 0;
    Action          m_PendingAction = Action::None;
};

#endif
