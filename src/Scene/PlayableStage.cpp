#include "Scene/PlayableStage.hpp"

#include <algorithm>
#include <memory>
#include <utility>

#include "Anm/AnmDefs.hpp"
#include "Audio/AudioManager.hpp"
#include "Scene/TimelineLoader.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr float   INTRO_CENTER_X         = -96.0f;
static constexpr float   INTRO_STAGE_NO_SCALE   = 1.0f;
static constexpr float   INTRO_STAGE_NAME_SCALE = 0.9f;
static constexpr float   INTRO_SONG_SCALE       = 0.62f;
static constexpr float   INTRO_SONG_ADVANCE_SCALE       = 0.86f;
static constexpr int     MAX_DEBUG_LIVES        = 8;
static constexpr int     MIN_BOMBS_AFTER_DEATH  = 3;
static constexpr int     POWER_LOSS_ON_DEATH    = 16;
static constexpr int     DEATH_POWER_SMALL_DROPS = 5;
static constexpr int     GAME_OVER_FULL_POWER_DROPS = 5;
static const glm::vec2   INTRO_SONG_POS         = {64.0f, -204.0f};
static const Util::Color INTRO_STAGE_YELLOW     = Util::Color::FromRGB(255, 255, 64);
static const Util::Color INTRO_LIGHT_CYAN       = Util::Color::FromRGB(224, 255, 255);

static float IntroAlpha(int frame) {
    if (frame < 30) return static_cast<float>(frame) / 30.0f;
    if (frame < 210) return 1.0f;
    if (frame < 270) return 1.0f - static_cast<float>(frame - 210) / 60.0f;
    return 0.0f;
}

void AddScoreWithExtend(GameManager& gm, int points) {
    const int extendCount = gm.AddScore(points);
    for (int i = 0; i < extendCount; ++i) {
        AudioManager::Instance().Play(SoundEffect::Extend);
    }
}

}  // namespace

PlayableStage::PlayableStage(CharacterItem character, SpellCardItem spellCard,
                             GameManager gameManager, PlayableStageConfig config,
                             std::unique_ptr<IStageScript> script)
    : m_Character(character),
      m_SpellCard(spellCard),
      m_Config(std::move(config)),
      m_GameManager(gameManager),
      m_Player(character, spellCard) {
    AudioManager::Instance().PlayMusic(m_Config.stageBgmPath, 500);
    m_IntroAnm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);

    m_EnemyManager.SetItemManager(&m_ItemManager);
    m_EnemyManager.SetTimeline(LoadTimelineFromJson(m_Config.timelinePath));
    m_EnemyManager.SetScript(std::move(script));

    SetupIntroAsciiLine(m_IntroStageNoLine, m_Config.stageNo, {INTRO_CENTER_X, 42.0f},
                        INTRO_STAGE_NO_SCALE, Util::AsciiTextAlign::Center, INTRO_STAGE_YELLOW);
    SetupIntroAsciiLine(m_IntroStageNameLine, m_Config.stageName, {INTRO_CENTER_X, 16.0f},
                        INTRO_STAGE_NAME_SCALE, Util::AsciiTextAlign::Center, INTRO_LIGHT_CYAN,
                        m_Config.stageNameAdvanceScale);
    SetupIntroAsciiLine(m_IntroSongLine, m_Config.songName, INTRO_SONG_POS, INTRO_SONG_SCALE,
                        Util::AsciiTextAlign::Right, INTRO_LIGHT_CYAN, INTRO_SONG_ADVANCE_SCALE);
    if (m_Config.hasStageClear) m_ClearOverlay.Init();
    UpdateStageIntro();
}

void PlayableStage::SetupIntroAsciiLine(Util::AsciiTextLine& line, const std::string& text,
                                        glm::vec2 pos, float scale, Util::AsciiTextAlign align,
                                        const Util::Color& color, float advanceScale) {
    line.Configure(m_IntroRenderer, m_IntroAnm, 30.0f);
    line.SetText(text, pos, scale, align, color, advanceScale);
}

void PlayableStage::UpdateStageIntro() {
    const float alpha   = IntroAlpha(m_StageFrame);
    const bool  visible = alpha > 0.0f;

    m_IntroStageNoLine.SetVisible(visible);
    m_IntroStageNameLine.SetVisible(visible);
    m_IntroSongLine.SetVisible(visible);
    m_IntroStageNoLine.SetAlpha(alpha);
    m_IntroStageNameLine.SetAlpha(alpha);
    m_IntroSongLine.SetAlpha(alpha);
}

void PlayableStage::SetBackground(std::unique_ptr<StageBackground> background) {
    m_Background = std::move(background);
}

void PlayableStage::UpdateBackground() {
    if (m_Background) m_Background->Update(m_StageFrame);
}

void PlayableStage::UpdateFinalBossClearFlow(const BossHudState& bossHud) {
    if (m_Config.bossSkipFrame >= 0 &&
        m_EnemyManager.GetTimelineFrame() >= m_Config.bossSkipFrame && bossHud.visible) {
        m_FinalBossWasSeen = true;
    } else if (m_FinalBossWasSeen && !bossHud.visible) {
        if (m_Config.hasStageClear) {
            if (m_FinalBossClearDelay < 0) {
                m_FinalBossClearDelay = m_Config.bossDeathResultDelay;
            }
            if (m_FinalBossClearDelay == 0) {
                m_ClearOverlay.Start(m_GameManager, m_Config.stageBonus);
            } else {
                m_FinalBossClearDelay--;
            }
        } else {
            m_Done = true;
        }
    }

    if (!m_FinalBossWasSeen && m_Config.totalFrames >= 0 && m_StageFrame > m_Config.totalFrames) {
        m_Done = true;
    }
}

bool PlayableStage::HandleStageOverlay() {
    if (!m_ClearOverlay.HasStarted()) return false;
    m_Renderer.Update();
    m_Gui.Update(m_GameManager, {}, true);
    if (m_ClearOverlay.Update()) m_Done = true;
    return true;
}

void PlayableStage::OnAfterGameplayFrame(const BossHudState& bossHud) {
    if (!m_BossMusicStarted && bossHud.visible && m_Config.bossSkipFrame >= 0 &&
        m_EnemyManager.GetTimelineFrame() >= m_Config.bossSkipFrame) {
        AudioManager::Instance().PlayMusic(m_Config.bossBgmPath, 500);
        m_BossMusicStarted = true;
    }
    UpdateFinalBossClearFlow(bossHud);
}

void PlayableStage::HandleDebugShortcuts() {
    if (m_StageMenu.IsOpen()) return;

    const bool shiftHeld = Util::Input::IsKeyPressed(Util::Keycode::LSHIFT) ||
                           Util::Input::IsKeyPressed(Util::Keycode::RSHIFT);
    const bool plusPressed = Util::Input::IsKeyDown(Util::Keycode::EQUALS) ||
                             Util::Input::IsKeyDown(Util::Keycode::KP_PLUS);
    if (shiftHeld && Util::Input::IsKeyPressed(Util::Keycode::L) && plusPressed &&
        m_GameManager.livesRemaining < MAX_DEBUG_LIVES) {
        ++m_GameManager.livesRemaining;
    }

    const int bossSkipFrame = BossSkipFrame();
    if (bossSkipFrame >= 0 && Util::Input::IsKeyDown(Util::Keycode::P)) {
        m_StageFrame = bossSkipFrame;
        m_EnemyManager.SkipToFrame(bossSkipFrame);
    }

    if (m_Config.midbossSkipFrame >= 0 && Util::Input::IsKeyDown(Util::Keycode::O)) {
        m_StageFrame = m_Config.midbossSkipFrame;
        m_EnemyManager.SkipToFrame(m_Config.midbossSkipFrame);
    }
}

void PlayableStage::DropPlayerPowerOnDeath(glm::vec2 pos) {
    if (m_GameManager.livesRemaining <= 0) {
        m_GameManager.power = 0;
        for (int i = 0; i < GAME_OVER_FULL_POWER_DROPS; ++i) {
            m_ItemManager.SpawnItem(pos, ItemType::FullPower, 2);
        }
        return;
    }

    m_GameManager.power = std::max(0, m_GameManager.power - POWER_LOSS_ON_DEATH);
    m_ItemManager.SpawnItem(pos, ItemType::PowerBig, 2);
    for (int i = 0; i < DEATH_POWER_SMALL_DROPS; ++i) {
        m_ItemManager.SpawnItem(pos, ItemType::PowerSmall, 2);
    }
}

void PlayableStage::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
        m_StageMenu.Toggle();
    }

    const auto stageMenuAction = m_StageMenu.Update();
    if (stageMenuAction == StageMenu::Action::ReturnToTitle) {
        m_ReturnToTitle = true;
        m_Done          = true;
        return;
    }

    HandleDebugShortcuts();

    if (m_StageMenu.IsOpen()) {
        UpdateBackground();
        m_Renderer.Update();
        m_ItemManager.Render();
        m_EnemyManager.Render();
        m_Player.Render();
        m_Gui.Update(m_GameManager, m_EnemyManager.GetBossHudState(), false);
        m_StageMenu.Render();
        UpdateStageIntro();
        m_IntroRenderer.Update();
        OnMenuFrame();
        return;
    }

    if (HandleStageOverlay()) return;

    ++m_StageFrame;
    UpdateBackground();
    m_Renderer.Update();

    m_GameManager.bombActive = m_Player.IsBombActive();
    m_ItemManager.Update(m_Player.GetPos(), m_GameManager);
    if (m_ItemManager.ConsumeFullPowerActivated()) {
        m_EnemyManager.TurnAllBulletsIntoPointItems();
    }
    m_EnemyManager.Update(m_Player.GetPos(), m_GameManager);
    m_Player.Update(m_GameManager);
    if (!m_GameManager.timeStopped) {
        const bool usedBomb      = m_Player.TryUseBomb(m_GameManager);
        m_GameManager.bombActive = usedBomb || m_Player.IsBombActive();
        if (m_GameManager.bombActive) {
            m_EnemyManager.TurnAllBulletsIntoPointItems();
        }
    }

    const bool canGraze =
        m_Player.GetState() != PlayerState::DEAD && m_Player.GetState() != PlayerState::SPAWNING;
    if (canGraze) {
        const int grazeCount =
            m_EnemyManager.ApplyGraze(m_Player.GetPos(), {PLAYER_HITBOX_X, PLAYER_HITBOX_Y});
        if (grazeCount > 0) {
            if (!m_GameManager.bombActive) {
                m_GameManager.graze = std::min(999999, m_GameManager.graze + grazeCount);
            }
            AddScoreWithExtend(m_GameManager, grazeCount * 500);
        }
    }

    int scoreGained =
        m_GameManager.timeStopped ? 0 : m_EnemyManager.ApplyPlayerBulletDamage(m_Player);
    if (scoreGained > 0) {
        AddScoreWithExtend(m_GameManager, scoreGained);
    }

    if (m_Player.IsVulnerable() &&
        m_EnemyManager.CheckPlayerHit(m_Player.GetPos(), {PLAYER_HITBOX_X, PLAYER_HITBOX_Y})) {
        DropPlayerPowerOnDeath(m_Player.GetPos());
        m_Player.Die();
        if (m_GameManager.bombsRemaining < MIN_BOMBS_AFTER_DEATH) {
            m_GameManager.bombsRemaining = MIN_BOMBS_AFTER_DEATH;
        }
        if (--m_GameManager.livesRemaining < 0) {
            m_GameManager.livesRemaining = 0;
            m_GameOver                   = true;
            m_Done                       = true;
        }
    }

    if (m_Player.JustEnteredSpawning()) {
        m_EnemyManager.ClearAllBullets();
    }

    const BossHudState bossHud = m_EnemyManager.GetBossHudState();
    OnAfterGameplayFrame(bossHud);

    m_ItemManager.RenderPickupLabels();
    m_Gui.Update(m_GameManager, bossHud, true);
    UpdateStageIntro();
    m_IntroRenderer.Update();
    OnFrameEnd();
}

void PlayableStage::OnFrameEnd() {
    if (m_ClearOverlay.Update()) m_Done = true;
}

void PlayableStage::OnMenuFrame() {
    if (m_ClearOverlay.Update()) m_Done = true;
}
