#ifndef SCENE_STAGE6_STAGE6SCRIPT_HPP
#define SCENE_STAGE6_STAGE6SCRIPT_HPP

#include "Scene/IStageScript.hpp"

class Stage6Script : public IStageScript {
   public:
    void Preload(Anm::Manager& anm) override;
    void InitSub(Enemy& enemy, EnemySubCtx& ctx) override;
    void RunSub(Enemy& enemy, EnemySubCtx& ctx) override;
};

#endif  // SCENE_STAGE6_STAGE6SCRIPT_HPP
