#include "Scene/Loading.hpp"

#include <memory>

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

    // TODO: Wait for actual resource loading to complete before setting m_Done = true
    m_Done = true;
}

std::unique_ptr<Scene> Loading::NextScene() {
    return std::make_unique<Title>();
}
