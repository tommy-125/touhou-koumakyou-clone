#include "Scene/TitleScene.hpp"
#include "Util/Image.hpp"

TitleScene::TitleScene() {
    auto image = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/title00.jpg"
    );

    m_TitleObj = std::make_shared<Util::GameObject>(image, 0.0f);
    m_Renderer.AddChild(m_TitleObj);
}

void TitleScene::Update() {
    m_Renderer.Update();
}
