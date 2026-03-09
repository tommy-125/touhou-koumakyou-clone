#include "Scene/LoadingScene.hpp"
#include "Util/Image.hpp"

LoadingScene::LoadingScene() {
    auto image = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/loading.png"
    );

    m_LoadingObj = std::make_shared<Util::GameObject>(image, 0.0f);
    m_Renderer.AddChild(m_LoadingObj);
}

void LoadingScene::Update() {
    m_Renderer.Update();

    // TODO: Wait for actual resource loading to complete before setting m_Done = true
    m_Done = true;
}
