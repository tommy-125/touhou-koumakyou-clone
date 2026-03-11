#include "App.hpp"
#include "Scene/LoadingScene.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");
    m_Scene = std::make_unique<LoadingScene>();
}

void App::Update() {
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_Done = true;
        return;
    }

    if (!m_Scene) return;

    m_Scene->Update();

    if (m_Scene->IsDone()) {
        auto nextScene = m_Scene->NextScene();
        if (nextScene) {
            m_Scene = std::move(nextScene);
        } else {
            m_Done = true;
        }
    }
}

void App::End() {
    LOG_TRACE("End");
}
