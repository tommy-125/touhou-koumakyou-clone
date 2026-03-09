#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");
    m_CurrentState = State::LOADING;
}

void App::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
        return;
    }

    switch (m_CurrentState) {
    case State::LOADING:
        m_LoadingScene.Update();
        if (m_LoadingScene.IsDone())
            m_CurrentState = State::TITLE;
        break;

    case State::TITLE:
        m_TitleScene.Update();
        break;

    default:
        break;
    }
}

void App::End() {
    LOG_TRACE("End");
}
