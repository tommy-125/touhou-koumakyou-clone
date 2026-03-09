#ifndef APP_HPP
#define APP_HPP

#include "Scene/LoadingScene.hpp"
#include "Scene/TitleScene.hpp"

class App {
    public:
        enum class State {
            START,
            LOADING,
            TITLE,
            END,
        };

        State GetCurrentState() const { return m_CurrentState; }

        void Start();
        void Update();
        void End(); // NOLINT(readability-convert-member-functions-to-static)

    private:
        State m_CurrentState = State::START;

        LoadingScene m_LoadingScene;
        TitleScene m_TitleScene;
};

#endif
