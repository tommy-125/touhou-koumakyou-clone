#ifndef SCENE_LOADING_HPP
#define SCENE_LOADING_HPP

#include "Scene/Scene.hpp"
#include "Util/GameObject.hpp"

class Loading : public Scene {
   public:
    Loading();

    void                   Update() override;
    std::unique_ptr<Scene> NextScene() override;

   private:
    std::shared_ptr<Util::GameObject> m_LoadingObj;
};

#endif
