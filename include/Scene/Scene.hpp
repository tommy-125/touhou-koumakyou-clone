#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include "Util/Renderer.hpp"
#include <memory>

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Update() = 0;
    virtual bool IsDone() const { return m_Done; }
    virtual std::unique_ptr<Scene> NextScene() { return nullptr;}
protected:
    Util::Renderer m_Renderer;
    bool m_Done = false;

};

#endif