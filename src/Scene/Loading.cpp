#include "Scene/Loading.hpp"

#include <memory>

#include "Audio/AudioManager.hpp"
#include "Scene/StageScriptUtil.hpp"
#include "Scene/Title.hpp"
#include "Util/Image.hpp"

Loading::Loading() {
    auto image =
        std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/loading.png");

    m_LoadingObj = std::make_shared<Util::GameObject>(image, 0.0f);
    m_Renderer.AddChild(m_LoadingObj);
}

void Loading::Update() {
    m_Renderer.Update();
    RunNextLoadStep();
}

void Loading::RunNextLoadStep() {
    switch (m_LoadStep) {
        case LoadStep::ValidateConfigs:
            StageScriptUtil::ValidateAllConfigs();
            m_LoadStep = LoadStep::WarmAudio;
            break;
        case LoadStep::WarmAudio:
            AudioManager::Instance();
            m_LoadStep = LoadStep::CreateNextScene;
            break;
        case LoadStep::CreateNextScene:
            m_NextScene = std::make_unique<Title>();
            m_LoadStep  = LoadStep::Done;
            m_Done      = true;
            break;
        case LoadStep::Done:
            break;
    }
}

std::unique_ptr<Scene> Loading::NextScene() {
    return std::move(m_NextScene);
}
