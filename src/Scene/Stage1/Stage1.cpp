#include "Scene/Stage1/Stage1.hpp"

#include <cstdio>
#include <memory>

#include "Scene/Stage1/Stage1Script.hpp"
#include "Scene/TimelineLoader.hpp"
#include "Scene/Title.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int        INTRO_FONT_SIZE   = 18;
static constexpr int        INTRO_SMALL_SIZE  = 14;
static constexpr int        CLEAR_FONT_SIZE   = 16;
static constexpr const char INTRO_FONT_PATH[] = PTSD_FONT_PATH;
static constexpr int        STAGE1_BOSS_SKIP_FRAME = 5279;
static constexpr int        CLEAR_LOADING_WAIT     = 0;
static constexpr int        CLEAR_LOADING_FADE_IN  = 60;
static constexpr int        CLEAR_LOADING_HOLD     = 240;
static constexpr int        CLEAR_LOADING_FADE_OUT = 60;
static constexpr int        CLEAR_LOADING_TOTAL =
    CLEAR_LOADING_WAIT + CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD + CLEAR_LOADING_FADE_OUT;
static constexpr float      CLEAR_TEXT_CENTER_X = 0.0f;
// Center of the TH06 play field: field x 192 + screen offset 32 = screen x 224,
// then converted to PTSD x: 224 - 320 = -96.
static constexpr float      INTRO_CENTER_X    = -96.0f;
static const Util::Color    INTRO_CYAN        = Util::Color::FromRGB(190, 245, 255);
static const Util::Color    INTRO_YELLOW      = Util::Color::FromRGB(255, 230, 120);
static const Util::Color    CLEAR_WHITE       = Util::Color::FromRGB(255, 255, 255);
static const Util::Color    CLEAR_LAVENDER    = Util::Color::FromRGB(210, 190, 255);
static const Util::Color    CLEAR_BLUE        = Util::Color::FromRGB(170, 220, 255);
static const Util::Color    CLEAR_RED         = Util::Color::FromRGB(255, 160, 180);

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

static std::string FormatClearLine(const char* fmt, int value) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), fmt, value);
    return buf;
}

static float ClearLoadingAlpha(int timer) {
    const int local = timer - CLEAR_LOADING_WAIT;
    if (local < 0) return 0.0f;
    if (local < CLEAR_LOADING_FADE_IN) {
        return static_cast<float>(local) / static_cast<float>(CLEAR_LOADING_FADE_IN);
    }
    if (local < CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD) return 1.0f;
    if (local < CLEAR_LOADING_FADE_IN + CLEAR_LOADING_HOLD + CLEAR_LOADING_FADE_OUT) {
        return 1.0f - static_cast<float>(local - CLEAR_LOADING_FADE_IN - CLEAR_LOADING_HOLD) /
                          static_cast<float>(CLEAR_LOADING_FADE_OUT);
    }
    return 0.0f;
}

}  // namespace

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard)
    : m_StageMenu(m_Renderer), m_Player(character, spellCard) {
    m_EnemyManager.SetItemManager(&m_ItemManager);
    m_EnemyManager.SetTimeline(
        LoadTimelineFromJson(GA_RESOURCE_DIR "/stages/stage1_timeline.json"));
    m_EnemyManager.SetScript(std::make_unique<Stage1Script>());

    m_BgImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/stage1_bg.png");
    m_BgObj   = std::make_shared<Util::GameObject>(m_BgImage, -10.0f);
    m_BgObj->m_Transform.translation = {-96.0f, 901.0f};
    m_Renderer.AddChild(m_BgObj);

    m_IntroStageNoObj = MakeIntroText(m_IntroStageNoText, "STAGE 1", INTRO_YELLOW,
                                      INTRO_FONT_SIZE, 0.0f, 42.0f);
    m_IntroStageNameObj = MakeIntroText(m_IntroStageNameText,
                                        "A Dream More Scarlet than Red", INTRO_CYAN,
                                        INTRO_FONT_SIZE, 0.0f, 16.0f);
    m_IntroSongObj = MakeIntroText(m_IntroSongText, "BGM: A Soul as Red as a Ground Cherry",
                                   INTRO_CYAN, INTRO_SMALL_SIZE, 0.0f, -12.0f);
    m_IntroRenderer.AddChild(m_IntroStageNoObj);
    m_IntroRenderer.AddChild(m_IntroStageNameObj);
    m_IntroRenderer.AddChild(m_IntroSongObj);
    UpdateStageIntro();

    m_ClearTexts.reserve(8);
    m_ClearObjs.reserve(8);
    m_ClearLoadingImage = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_CM/loading.png");
    m_ClearLoadingObj = std::make_shared<Util::GameObject>(m_ClearLoadingImage, 29.0f);
    m_ClearLoadingObj->m_Transform.scale = {1.5f, 1.75f};
    m_ClearLoadingObj->SetVisible(false);
    m_ClearRenderer.AddChild(m_ClearLoadingObj);
    for (int i = 0; i < 8; i++) {
        auto text = std::make_shared<Util::Text>(INTRO_FONT_PATH, CLEAR_FONT_SIZE, " ", CLEAR_WHITE);
        auto obj  = std::make_shared<Util::GameObject>(text, 31.0f);
        obj->SetVisible(false);
        m_ClearTexts.push_back(text);
        m_ClearObjs.push_back(obj);
        m_ClearRenderer.AddChild(obj);
    }
}

void Stage1::UpdateStageIntro() {
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
    m_IntroStageNoObj->m_Transform.translation   = {INTRO_CENTER_X, 42.0f};
    m_IntroStageNameObj->m_Transform.translation = {INTRO_CENTER_X, 16.0f};
    m_IntroSongObj->m_Transform.translation      = {INTRO_CENTER_X, -12.0f};
}

void Stage1::SetStageClearLine(size_t idx, const std::string& text, const Util::Color& color,
                               float y) {
    if (idx >= m_ClearTexts.size()) return;

    m_ClearTexts[idx]->SetText(text);
    m_ClearTexts[idx]->SetColor(color);
    m_ClearObjs[idx]->m_Transform.translation = {CLEAR_TEXT_CENTER_X, y};
    m_ClearObjs[idx]->SetVisible(true);
}

void Stage1::ShowStageClearText() {
    if (m_StageClearTextShown) return;
    const int stageBonus = 1000;
    const int powerBonus = m_GameManager.power * 100;
    const int grazeBonus = m_GameManager.graze * 10;

    SetStageClearLine(0, "Stage Clear", INTRO_YELLOW, 112.0f);
    SetStageClearLine(1, FormatClearLine("Stage * 1000 = %5d", stageBonus), CLEAR_WHITE, 80.0f);
    SetStageClearLine(2, FormatClearLine("Power *  100 = %5d", powerBonus), CLEAR_LAVENDER, 64.0f);
    SetStageClearLine(3, FormatClearLine("Graze *   10 = %5d", grazeBonus), CLEAR_BLUE, 48.0f);
    SetStageClearLine(4, FormatClearLine("    * Point Item %3d", m_GameManager.pointItems),
                      CLEAR_RED, 32.0f);
    SetStageClearLine(5, "Normal Rank    * 1.0", CLEAR_RED, 0.0f);
    SetStageClearLine(6, FormatClearLine("Total     = %8d", m_StageClearScore), CLEAR_WHITE,
                      -16.0f);
    m_StageClearTextShown = true;
}

void Stage1::StartStageClear() {
    if (m_StageClearStarted) return;

    const int stageBonus = 1000;
    const int powerBonus = m_GameManager.power * 100;
    const int grazeBonus = m_GameManager.graze * 10;
    m_StageClearScore    = (stageBonus + powerBonus + grazeBonus) * m_GameManager.pointItems;
    m_GameManager.score += m_StageClearScore;
    if (m_GameManager.score > m_GameManager.highScore) {
        m_GameManager.highScore = m_GameManager.score;
    }

    m_StageClearStarted = true;
    m_StageClearTimer   = 0;
}

void Stage1::UpdateStageClearLoading() {
    if (!m_StageClearStarted) return;

    const float alpha = ClearLoadingAlpha(m_StageClearTimer);
    m_ClearLoadingObj->SetVisible(alpha > 0.0f);
    m_ClearLoadingObj->SetAlpha(alpha);
    m_ClearLoadingObj->m_Transform.translation = {-96.0f, 0.0f};

    if (!m_StageClearTextShown && m_StageClearTimer >= CLEAR_LOADING_FADE_IN) {
        ShowStageClearText();
    }

    const bool textVisible = m_StageClearTextShown && m_StageClearTimer < CLEAR_LOADING_TOTAL;
    for (auto& obj : m_ClearObjs) {
        obj->SetVisible(textVisible);
        obj->SetAlpha(alpha);
    }

    if (m_StageClearTimer >= CLEAR_LOADING_TOTAL) {
        m_ClearLoadingObj->SetVisible(false);
        for (auto& obj : m_ClearObjs) obj->SetVisible(false);
    }
}

void Stage1::UpdateStageClear() {
    if (!m_StageClearStarted) return;
    m_StageClearTimer++;
    UpdateStageClearLoading();
    m_ClearRenderer.Update();

    if (m_StageClearTextShown && m_StageClearTimer > CLEAR_LOADING_FADE_IN + 30 &&
        (Util::Input::IsKeyDown(Util::Keycode::Z) ||
         Util::Input::IsKeyDown(Util::Keycode::RETURN))) {
        m_Done = true;
        return;
    }

    if (m_StageClearTimer > CLEAR_LOADING_TOTAL + 30) {
        m_Done = true;
    }
}

void Stage1::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
        m_StageMenu.Toggle();
    }

    const auto stageMenuAction = m_StageMenu.Update();
    if (stageMenuAction == StageMenu::Action::ReturnToTitle) {
        m_Done = true;
        return;
    }

    if (!m_StageMenu.IsOpen() && Util::Input::IsKeyDown(Util::Keycode::P)) {
        m_StageFrame = STAGE1_BOSS_SKIP_FRAME;
        m_EnemyManager.SkipToFrame(STAGE1_BOSS_SKIP_FRAME);
    }

    if (m_StageMenu.IsOpen()) {
        m_Renderer.Update();
        m_Gui.Update(m_GameManager, m_EnemyManager.GetBossHudState(), false);
        UpdateStageIntro();
        m_IntroRenderer.Update();
        UpdateStageClear();
        return;
    }

    if (m_StageClearStarted) {
        m_Renderer.Update();
        m_Gui.Update(m_GameManager, {}, true);
        UpdateStageClear();
        return;
    }

    ++m_StageFrame;
    float scrollY = m_StageFrame * (BG_CANVAS_H - FIELD_H) / STAGE_TOTAL_FRAMES;
    m_BgObj->m_Transform.translation.y = (BG_CANVAS_H / 2.0f - FIELD_H / 2.0f) - scrollY;
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
    if (m_StageFrame >= STAGE1_BOSS_SKIP_FRAME && bossHud.visible) {
        m_FinalBossWasSeen = true;
    } else if (m_FinalBossWasSeen && !bossHud.visible) {
        StartStageClear();
    }

    m_Gui.Update(m_GameManager, bossHud, true);
    UpdateStageIntro();
    m_IntroRenderer.Update();
    UpdateStageClear();
}

std::unique_ptr<Scene> Stage1::NextScene() {
    return std::make_unique<Title>();
}
