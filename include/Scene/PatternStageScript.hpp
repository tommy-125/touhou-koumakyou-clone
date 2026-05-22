#pragma once

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

#include "Scene/IStageScript.hpp"

class IEnemySubPattern {
   public:
    virtual ~IEnemySubPattern()                       = default;
    virtual bool Handles(int subId) const             = 0;
    virtual void Init(Enemy& enemy, EnemySubCtx& ctx) = 0;
    virtual void Run(Enemy& enemy, EnemySubCtx& ctx)  = 0;
};

class LambdaEnemySubPattern final : public IEnemySubPattern {
   public:
    using InitFn = std::function<void(Enemy&, EnemySubCtx&)>;
    using RunFn  = std::function<void(Enemy&, EnemySubCtx&)>;

    LambdaEnemySubPattern(std::initializer_list<int> subIds, InitFn init, RunFn run)
        : m_SubIds(subIds), m_Init(std::move(init)), m_Run(std::move(run)) {}

    bool Handles(int subId) const override {
        return std::find(m_SubIds.begin(), m_SubIds.end(), subId) != m_SubIds.end();
    }

    void Init(Enemy& enemy, EnemySubCtx& ctx) override {
        if (m_Init) m_Init(enemy, ctx);
    }

    void Run(Enemy& enemy, EnemySubCtx& ctx) override {
        if (m_Run) m_Run(enemy, ctx);
    }

   private:
    std::vector<int> m_SubIds;
    InitFn           m_Init;
    RunFn            m_Run;
};

class PatternStageScript : public IStageScript {
   public:
    void InitSub(Enemy& enemy, EnemySubCtx& ctx) override {
        if (IEnemySubPattern* pattern = FindPattern(enemy.m_SubId)) {
            pattern->Init(enemy, ctx);
        }
    }

    void RunSub(Enemy& enemy, EnemySubCtx& ctx) override {
        if (IEnemySubPattern* pattern = FindPattern(enemy.m_SubId)) {
            pattern->Run(enemy, ctx);
        }
    }

   protected:
    using InitFn     = LambdaEnemySubPattern::InitFn;
    using RunFn      = LambdaEnemySubPattern::RunFn;
    using TimedRunFn = std::function<void(Enemy&, EnemySubCtx&, int)>;

    void AddPattern(std::initializer_list<int> subIds, InitFn init, RunFn run) {
        m_Patterns.push_back(
            std::make_unique<LambdaEnemySubPattern>(subIds, std::move(init), std::move(run)));
    }

    void AddPattern(int subId, InitFn init, RunFn run) {
        AddPattern({subId}, std::move(init), std::move(run));
    }

    void AddRunOnlyPattern(std::initializer_list<int> subIds, RunFn run) {
        AddPattern(subIds, nullptr, std::move(run));
    }

    void AddRunOnlyPattern(int subId, RunFn run) { AddPattern(subId, nullptr, std::move(run)); }

    void AddTimedPattern(std::initializer_list<int> subIds, InitFn init, TimedRunFn run) {
        AddPattern(subIds, std::move(init), WithFrameTimer(std::move(run)));
    }

    void AddTimedPattern(int subId, InitFn init, TimedRunFn run) {
        AddTimedPattern({subId}, std::move(init), std::move(run));
    }

    void AddTimedRunOnlyPattern(std::initializer_list<int> subIds, TimedRunFn run) {
        AddTimedPattern(subIds, nullptr, std::move(run));
    }

    void AddTimedRunOnlyPattern(int subId, TimedRunFn run) {
        AddTimedPattern(subId, nullptr, std::move(run));
    }

   private:
    static RunFn WithFrameTimer(TimedRunFn run) {
        return [run = std::move(run)](Enemy& enemy, EnemySubCtx& ctx) {
            run(enemy, ctx, enemy.m_FrameTimer);
        };
    }

    IEnemySubPattern* FindPattern(int subId) const {
        for (const auto& pattern : m_Patterns) {
            if (pattern->Handles(subId)) return pattern.get();
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<IEnemySubPattern>> m_Patterns;
};
