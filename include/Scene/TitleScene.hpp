#ifndef SCENE_TITLE_SCENE_HPP
#define SCENE_TITLE_SCENE_HPP

#include <array>
#include <memory>

#include "Anm/AnmManager.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

class TitleScene {
public:
    TitleScene();

    void Update();

private:
    static constexpr int TITLE02_SCRIPTS = 4;
    static constexpr int TITLE02_OFFSET  = 0;

    Anm::Manager m_Anm;

    std::array<Anm::Vm, TITLE02_SCRIPTS> m_Vms;
    std::array<std::shared_ptr<Util::GameObject>, TITLE02_SCRIPTS> m_Objs;

    std::shared_ptr<Util::GameObject> m_BgObj;
    Util::Renderer m_Renderer;
};

#endif
