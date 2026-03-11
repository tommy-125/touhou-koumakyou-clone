#ifndef SCENE_LOADING_HPP
#define SCENE_LOADING_HPP

#include "Util/GameObject.hpp"

#include "Scene/Scene.hpp"

class Loading : public Scene {
public:
    Loading();

    void Update() override;
    std::unique_ptr<Scene> NextScene() override;
private:
    std::shared_ptr<Util::GameObject> m_LoadingObj;
};

#endif
