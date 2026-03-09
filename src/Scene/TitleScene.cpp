#include "Scene/TitleScene.hpp"

#include "Util/Image.hpp"

TitleScene::TitleScene() {
    // Background
    auto bgImage = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/title00.jpg"
    );
    m_BgObj = std::make_shared<Util::GameObject>(bgImage, 0.0f);
    m_Renderer.AddChild(m_BgObj);

    // Load ANM
    m_Anm.LoadAnm(
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02",
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/title02/title02.txt",
        TITLE02_OFFSET
    );

    // Init each Vm + GameObject, then queue interrupt 2 to start the slide-in
    for (int i = 0; i < TITLE02_SCRIPTS; i++) {
        m_Vms[i].scriptIdx    = i + TITLE02_OFFSET;
        m_Vms[i].spriteOffset = TITLE02_OFFSET;

        m_Objs[i] = std::make_shared<Util::GameObject>(nullptr, 1.0f, glm::vec2{0, 0}, false);
        m_Renderer.AddChild(m_Objs[i]);

        m_Anm.SendInterrupt(m_Vms[i], 1);
    }
}

void TitleScene::Update() {
    for (int i = 0; i < TITLE02_SCRIPTS; i++) {
        m_Anm.ExecuteScript(m_Vms[i]);

        const auto &vm  = m_Vms[i];
        auto       &obj = *m_Objs[i];

        obj.SetVisible(vm.isVisible);

        if (vm.spriteIdx >= 0 && m_Anm.sprites[vm.spriteIdx].image) {
            obj.SetDrawable(m_Anm.sprites[vm.spriteIdx].image);
        }

        obj.m_Transform.translation = Anm::Manager::ToPtsd(vm.pos);
        obj.m_Transform.scale       = vm.scale;
        obj.m_Transform.rotation    = vm.rotation;
    }

    m_Renderer.Update();
}
