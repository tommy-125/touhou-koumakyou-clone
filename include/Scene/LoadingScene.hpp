#ifndef SCENE_LOADING_SCENE_HPP
#define SCENE_LOADING_SCENE_HPP

#include "Util/GameObject.hpp"

#include "Scene/Scene.hpp"

class LoadingScene : public Scene {
public:
    LoadingScene();

    void Update() override;
    std::unique_ptr<Scene> NextScene() override;
private:
    std::shared_ptr<Util::GameObject> m_LoadingObj;
};

#endif
