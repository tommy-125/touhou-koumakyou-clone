#ifndef SCENE_CHEAT_MENU_HPP
#define SCENE_CHEAT_MENU_HPP

#include <array>

#include "Anm/AnmManager.hpp"
#include "Util/AsciiTextLine.hpp"
#include "Util/BlackMask.hpp"
#include "Util/Renderer.hpp"

class CheatMenu {
   public:
    enum class Action {
        None,
        FullPower,
        AddLife,
        AddBomb,
        ClearBullets,
        ToggleInvincible,
    };

    static constexpr int OPTION_COUNT = 5;

    CheatMenu();

    void   Open();
    void   Close();
    Action Update();
    void   Render();
    void   SetInvincibleEnabled(bool enabled);
    bool   IsOpen() const { return m_Open; }

   private:
    static constexpr int LINE_COUNT = OPTION_COUNT + 1;

    void RefreshText();

    Util::Renderer                              m_Renderer;
    Anm::Manager                                m_Anm;
    Util::BlackMask                             m_Mask = Util::BlackMask(45.0f, 0.0f);
    std::array<Util::AsciiTextLine, LINE_COUNT> m_Lines{};
    bool                                        m_Open          = false;
    bool                                        m_InvincibleEnabled = false;
    int                                         m_SelectedIndex = 0;
    int                                         m_FramesOpen    = 0;
};

#endif  // SCENE_CHEAT_MENU_HPP
