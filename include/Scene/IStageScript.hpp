#ifndef ISTAGE_SCRIPT_HPP
#define ISTAGE_SCRIPT_HPP

#include "Anm/AnmManager.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/EnemySubCtx.hpp"

class IStageScript {
   public:
    virtual ~IStageScript()                          = default;
    virtual void Preload(Anm::Manager& anm)          = 0;
    virtual void InitSub(Enemy& e, EnemySubCtx& ctx) = 0;
    virtual void RunSub(Enemy& e, EnemySubCtx& ctx)  = 0;
};

#endif  // ISTAGE_SCRIPT_HPP
