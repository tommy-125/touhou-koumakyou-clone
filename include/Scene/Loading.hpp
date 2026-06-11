#ifndef SCENE_LOADING_HPP
#define SCENE_LOADING_HPP

#include <memory>

#include "Scene/Scene.hpp"
#include "Util/GameObject.hpp"

class Loading : public Scene {
   public:
    Loading();

    void                   Update() override;
    std::unique_ptr<Scene> NextScene() override;

   private:
    enum class LoadStep {
        ValidateConfigs,
        WarmAudio,
        CreateNextScene,
        Done,
    };

    void RunNextLoadStep();

    std::shared_ptr<Util::GameObject> m_LoadingObj;
    LoadStep                          m_LoadStep = LoadStep::ValidateConfigs;
    std::unique_ptr<Scene>            m_NextScene;
};

#endif
