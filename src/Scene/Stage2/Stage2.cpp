#include "Scene/Stage2/Stage2.hpp"

#include <cmath>
#include <memory>

#include "Scene/Stage2/Stage2Script.hpp"
#include "Scene/TimelineLoader.hpp"
#include "Scene/Title.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int        INTRO_FONT_SIZE        = 18;
static constexpr int        INTRO_SMALL_SIZE       = 14;
static constexpr const char INTRO_FONT_PATH[]      = PTSD_FONT_PATH;
static constexpr int        STAGE2_FINAL_BOSS_FRAME = 5984;
static constexpr float      INTRO_CENTER_X         = -96.0f;
static constexpr float      BG_CENTER_X            = -96.0f;
static constexpr float      BG_SCALE               = 1.85f;
static constexpr float      BG_TILE_H              = 256.0f * BG_SCALE;
static constexpr float      BG_SCROLL_SPEED        = 0.75f;
static const Util::Color    INTRO_CYAN             = Util::Color::FromRGB(190, 245, 255);
static const Util::Color    INTRO_YELLOW           = Util::Color::FromRGB(255, 230, 120);

static std::shared_ptr<Util::GameObject> MakeIntroText(std::shared_ptr<Util::Text>& text,
                                                       const std::string& str,
                                                       const Util::Color& color, int fontSize,
                                                       float x, float y) {
    text = std::make_shared<Util::Text>(INTRO_FONT_PATH, fontSize, str, color);
    auto obj = std::make_shared<Util::GameObject>(text, 30.0f);
    obj->m_Transform.translation = {x, y};
    obj->SetVisible(false);
    return obj;
}

static float IntroAlpha(int frame) {
    if (frame < 30) return static_cast<float>(frame) / 30.0f;
    if (frame < 210) return 1.0f;
    if (frame < 270) return 1.0f - static_cast<float>(frame - 210) / 60.0f;
    return 0.0f;
}

}  // namespace

Stage2::Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : m_Character(character),
      m_SpellCard(spellCard),
      m_StageMenu(m_Renderer),
      m_GameManager(gameManager),
      m_Player(character, spellCard) {
    m_EnemyManager.SetItemManager(&m_ItemManager);
    m_EnemyManager.SetTimeline(
        LoadTimelineFromJson(GA_RESOURCE_DIR "/stages/stage2_timeline.json"));
    m_EnemyManager.SetScript(std::make_unique<Stage2Script>());

    m_BgImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_ST/stg2bg.png");
    for (auto& bgObj : m_BgObjs) {
        bgObj                    = std::make_shared<Util::GameObject>(m_BgImage, -10.0f);
        bgObj->m_Transform.scale = {BG_SCALE, BG_SCALE};
        m_Renderer.AddChild(bgObj);
    }
    UpdateBackground();

    m_IntroStageNoObj = MakeIntroText(m_IntroStageNoText, "STAGE 2", INTRO_YELLOW,
                                      INTRO_FONT_SIZE, INTRO_CENTER_X, 42.0f);
    m_IntroStageNameObj = MakeIntroText(m_IntroStageNameText, "The Lake in the Moonlight",
                                        INTRO_CYAN, INTRO_FONT_SIZE, INTRO_CENTER_X, 16.0f);
    m_IntroSongObj = MakeIntroText(m_IntroSongText, "BGM: Lunate Elf", INTRO_CYAN,
                                   INTRO_SMALL_SIZE, INTRO_CENTER_X, -12.0f);
    m_IntroRenderer.AddChild(m_IntroStageNoObj);
    m_IntroRenderer.AddChild(m_IntroStageNameObj);
    m_IntroRenderer.AddChild(m_IntroSongObj);
    UpdateStageIntro();
}

void Stage2::UpdateStageIntro() {
    const float alpha   = IntroAlpha(m_StageFrame);
    const bool  visible = alpha > 0.0f;
    const auto  alphaByte = static_cast<Uint8>(alpha * 255.0f);

    m_IntroStageNoObj->SetVisible(visible);
    m_IntroStageNameObj->SetVisible(visible);
    m_IntroSongObj->SetVisible(visible);
    if (!visible) return;

    m_IntroStageNoText->SetColor(Util::Color::FromRGB(255, 230, 120, alphaByte));
    m_IntroStageNameText->SetColor(Util::Color::FromRGB(190, 245, 255, alphaByte));
    m_IntroSongText->SetColor(Util::Color::FromRGB(190, 245, 255, alphaByte));
    m_IntroStageNoObj->SetAlpha(1.0f);
    m_IntroStageNameObj->SetAlpha(1.0f);
    m_IntroSongObj->SetAlpha(1.0f);
}

void Stage2::UpdateBackground() {
    const float scroll = std::fmod(static_cast<float>(m_StageFrame) * BG_SCROLL_SPEED, BG_TILE_H);
    const float sway   = std::sin(static_cast<float>(m_StageFrame) * 0.01f) * 2.0f;

    for (size_t i = 0; i < m_BgObjs.size(); i++) {
        auto& bgObj = m_BgObjs[i];
        if (!bgObj) continue;
        const float tileOffset = (static_cast<float>(i) - 1.0f) * BG_TILE_H;
        bgObj->m_Transform.translation = {BG_CENTER_X + sway, tileOffset - scroll};
    }
}

void Stage2::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
        m_StageMenu.Toggle();
    }

    const auto stageMenuAction = m_StageMenu.Update();
    if (stageMenuAction == StageMenu::Action::ReturnToTitle) {
        m_Done = true;
        return;
    }

    if (!m_StageMenu.IsOpen() && Util::Input::IsKeyDown(Util::Keycode::P)) {
        m_StageFrame = STAGE2_FINAL_BOSS_FRAME;
        m_EnemyManager.SkipToFrame(STAGE2_FINAL_BOSS_FRAME);
    }

    if (m_StageMenu.IsOpen()) {
        UpdateBackground();
        m_Renderer.Update();
        m_Gui.Update(m_GameManager, m_EnemyManager.GetBossHudState(), false);
        UpdateStageIntro();
        m_IntroRenderer.Update();
        return;
    }

    ++m_StageFrame;
    UpdateBackground();
    m_Renderer.Update();

    m_ItemManager.Update(m_Player.GetPos(), m_GameManager);
    m_EnemyManager.Update(m_Player.GetPos(), m_GameManager);
    m_Player.Update(m_GameManager);
    if (m_Player.TryUseBomb(m_GameManager) || m_Player.IsBombActive()) {
        m_EnemyManager.ClearAllBullets();
    }

    int scoreGained = m_EnemyManager.ApplyPlayerBulletDamage(m_Player);
    if (scoreGained > 0) {
        m_GameManager.score += scoreGained;
        if (m_GameManager.score > m_GameManager.highScore)
            m_GameManager.highScore = m_GameManager.score;
    }

    if (m_Player.IsVulnerable() &&
        m_EnemyManager.CheckPlayerHit(m_Player.GetPos(), {PLAYER_HITBOX_X, PLAYER_HITBOX_Y})) {
        m_Player.Die();
        if (--m_GameManager.livesRemaining < 0) {
            m_GameManager.livesRemaining = 0;
            m_Done                       = true;
        }
    }

    if (m_Player.JustEnteredSpawning()) {
        m_EnemyManager.ClearAllBullets();
    }

    const BossHudState bossHud = m_EnemyManager.GetBossHudState();
    if (m_StageFrame >= STAGE2_FINAL_BOSS_FRAME && bossHud.visible) {
        m_FinalBossWasSeen = true;
    } else if (m_FinalBossWasSeen && !bossHud.visible) {
        m_Done = true;
    }

    if (!m_FinalBossWasSeen && m_StageFrame > STAGE_TOTAL_FRAMES) {
        m_Done = true;
    }

    m_Gui.Update(m_GameManager, bossHud, true);
    UpdateStageIntro();
    m_IntroRenderer.Update();
}

std::unique_ptr<Scene> Stage2::NextScene() {
    return std::make_unique<Title>();
}
