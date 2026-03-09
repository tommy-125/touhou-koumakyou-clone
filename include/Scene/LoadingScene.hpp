#ifndef SCENE_LOADING_SCENE_HPP
#define SCENE_LOADING_SCENE_HPP

#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

class LoadingScene {
public:
    LoadingScene();

    void Update();
    bool IsDone() const { return m_Done; }

private:
    std::shared_ptr<Util::GameObject> m_LoadingObj;
    Util::Renderer m_Renderer;

    bool m_Done = false;
};

#endif
