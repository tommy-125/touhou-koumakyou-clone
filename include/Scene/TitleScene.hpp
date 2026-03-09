#ifndef SCENE_TITLE_SCENE_HPP
#define SCENE_TITLE_SCENE_HPP

#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

class TitleScene {
public:
    TitleScene();

    void Update();
    bool IsDone() const { return m_Done; }

private:
    std::shared_ptr<Util::GameObject> m_TitleObj;
    Util::Renderer m_Renderer;

    bool m_Done = false;
};

#endif
