#ifndef SCENE_PATTERN_STAGE_SCRIPT_HPP
#define SCENE_PATTERN_STAGE_SCRIPT_HPP

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Scene/IStageScript.hpp"
#include "Scene/StageScriptUtil.hpp"

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

class BossPhaseEnemySubPattern final : public IEnemySubPattern {
   public:
    using TimedRunFn = std::function<void(Enemy&, EnemySubCtx&, int)>;
    using StartFn    = std::function<void(Enemy&, EnemySubCtx&)>;

    BossPhaseEnemySubPattern(std::initializer_list<int> subIds,
                             StageScriptUtil::ConfigId::BossPhaseId phaseId,
                             TimedRunFn run, StartFn onStart = nullptr, int runStartFrame = 0)
        : m_SubIds(subIds),
          m_PhaseId(phaseId),
          m_Run(std::move(run)),
          m_OnStart(std::move(onStart)),
          m_RunStartFrame(runStartFrame) {}

    bool Handles(int subId) const override {
        return std::find(m_SubIds.begin(), m_SubIds.end(), subId) != m_SubIds.end();
    }

    void Init(Enemy&, EnemySubCtx&) override {}

    void Run(Enemy& enemy, EnemySubCtx& ctx) override {
        const int t = enemy.m_FrameTimer;
        if (t == 0) {
            StageScriptUtil::StartBossPhase(enemy, ctx, m_PhaseId);
            if (m_OnStart) m_OnStart(enemy, ctx);
        }
        if (m_Run && t >= m_RunStartFrame) m_Run(enemy, ctx, t);
    }

   private:
    std::vector<int>                         m_SubIds;
    StageScriptUtil::ConfigId::BossPhaseId   m_PhaseId;
    TimedRunFn                               m_Run;
    StartFn                                  m_OnStart;
    int                                      m_RunStartFrame;
};

class PatternStageScript : public IStageScript {
   public:
    void InitSub(Enemy& enemy, EnemySubCtx& ctx) override {
        if (IEnemySubPattern* pattern = FindPattern(enemy.m_SubId)) {
            pattern->Init(enemy, ctx);
            return;
        }
        ThrowMissingPattern(enemy.m_SubId);
    }

    void RunSub(Enemy& enemy, EnemySubCtx& ctx) override {
        if (IEnemySubPattern* pattern = FindPattern(enemy.m_SubId)) {
            pattern->Run(enemy, ctx);
            return;
        }
        ThrowMissingPattern(enemy.m_SubId);
    }

    bool HasSub(int subId) const override { return FindPattern(subId) != nullptr; }

    void Validate() const override {
        for (const auto& phaseId : m_BossPhaseIds) {
            const auto config = StageScriptUtil::LoadBossPhaseConfig(phaseId);
            ValidateBossPhaseSub(phaseId, "timerSub", config.timerSub);
            ValidateBossPhaseSub(phaseId, "deathSub", config.deathSub);
            ValidateBossPhaseSub(phaseId, "lifeSub", config.lifeSub);
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

    void AddBossPhasePattern(std::initializer_list<int> subIds,
                             StageScriptUtil::ConfigId::BossPhaseId phaseId,
                             TimedRunFn run, BossPhaseEnemySubPattern::StartFn onStart = nullptr,
                             int runStartFrame = 0) {
        RegisterBossPhase(phaseId);
        m_Patterns.push_back(std::make_unique<BossPhaseEnemySubPattern>(
            subIds, phaseId, std::move(run), std::move(onStart), runStartFrame));
    }

    void AddBossPhasePattern(int subId, StageScriptUtil::ConfigId::BossPhaseId phaseId,
                             TimedRunFn run,
                             BossPhaseEnemySubPattern::StartFn onStart       = nullptr,
                             int                               runStartFrame = 0) {
        AddBossPhasePattern({subId}, phaseId, std::move(run), std::move(onStart), runStartFrame);
    }

    void RegisterBossPhase(StageScriptUtil::ConfigId::BossPhaseId phaseId) {
        const auto hasPhase = std::find_if(
            m_BossPhaseIds.begin(), m_BossPhaseIds.end(), [phaseId](const auto& registered) {
                return registered.value == phaseId.value;
            });
        if (hasPhase != m_BossPhaseIds.end()) {
            return;
        }
        m_BossPhaseIds.push_back(phaseId);
    }

    void RegisterBossPhases(
        std::initializer_list<StageScriptUtil::ConfigId::BossPhaseId> phaseIds) {
        for (const auto& phaseId : phaseIds) {
            RegisterBossPhase(phaseId);
        }
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

    [[noreturn]] static void ThrowMissingPattern(int subId) {
        throw std::runtime_error("unhandled enemy subId: " + std::to_string(subId));
    }

    void ValidateBossPhaseSub(StageScriptUtil::ConfigId::BossPhaseId phaseId, const char* field,
                              int subId) const {
        if (subId < 0 || HasSub(subId)) return;

        throw std::runtime_error("boss phase '" + std::string(phaseId.value) + "' " + field +
                                 " uses unhandled enemy subId: " + std::to_string(subId));
    }

    std::vector<std::unique_ptr<IEnemySubPattern>> m_Patterns;
    std::vector<StageScriptUtil::ConfigId::BossPhaseId> m_BossPhaseIds;
};

#endif  // SCENE_PATTERN_STAGE_SCRIPT_HPP
