#ifndef SCENE_TITLE_HPP
#define SCENE_TITLE_HPP

#include <memory>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Util/GameObject.hpp"

#include "Scene/Scene.hpp"

class Title : public Scene {
public:
    Title();

    void Update() override;

private:
    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    std::shared_ptr<Util::GameObject> m_BgObj;
};

#endif
