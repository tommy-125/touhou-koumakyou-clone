#include "Scene/Stage1.hpp"

#include "Enemy/Timeline.hpp"
#include "Scene/Title.hpp"

// Transcribed from ecldata1.txt timeline0 (Normal difficulty)
// Format: {frame, subId, x, y, life, score, mirrored}
static const TimelineEntry STAGE1_TIMELINE[] = {
    // Wave 1: sub0 from left
    {128, 0, 60.0f, -32.0f, 8, 300, false},
    {144, 0, 68.0f, -32.0f, 32, 300, false},
    {160, 0, 76.0f, -32.0f, 32, 300, false},
    {176, 0, 84.0f, -32.0f, 32, 300, false},
    {192, 0, 92.0f, -32.0f, 32, 300, false},

    // Wave 1: sub0 from right (mirror)
    {256, 0, 324.0f, -32.0f, 32, 300, true},
    {272, 0, 316.0f, -32.0f, 32, 300, true},
    {288, 0, 308.0f, -32.0f, 32, 300, true},
    {304, 0, 300.0f, -32.0f, 32, 300, true},
    {320, 0, 292.0f, -32.0f, 32, 300, true},
    {336, 0, 284.0f, -32.0f, 32, 300, true},
    {352, 0, 276.0f, -32.0f, 32, 300, true},
    {368, 0, 268.0f, -32.0f, 32, 300, true},

    // Wave 2: sub1 left + right pairs
    {432, 1, 32.0f, -32.0f, 32, 300, false},
    {432, 1, 352.0f, -32.0f, 32, 300, true},
    {448, 1, 48.0f, -32.0f, 32, 300, false},
    {448, 1, 336.0f, -32.0f, 32, 300, true},
    {464, 1, 64.0f, -32.0f, 32, 300, false},
    {464, 1, 320.0f, -32.0f, 32, 300, true},
    {480, 1, 80.0f, -32.0f, 32, 300, false},
    {480, 1, 304.0f, -32.0f, 32, 300, true},
    {496, 1, 96.0f, -32.0f, 32, 300, false},
    {496, 1, 288.0f, -32.0f, 32, 300, true},
    {512, 1, 112.0f, -32.0f, 32, 300, false},
    {512, 1, 272.0f, -32.0f, 32, 300, true},
    {528, 1, 128.0f, -32.0f, 32, 300, false},
    {528, 1, 256.0f, -32.0f, 32, 300, true},
    {544, 1, 144.0f, -32.0f, 32, 300, false},
    {544, 1, 240.0f, -32.0f, 32, 300, true},
    {560, 1, 160.0f, -32.0f, 32, 300, false},
    {560, 1, 224.0f, -32.0f, 32, 300, true},
    {576, 1, 176.0f, -32.0f, 32, 300, false},
    {576, 1, 208.0f, -32.0f, 8, 300, true},

    // Wave 3: sub2/sub3 scattered
    {640, 2, 32.0f, -32.0f, 80, 700, false},
    {700, 3, 256.0f, -32.0f, 80, 700, true},
    {750, 3, 128.0f, -32.0f, 80, 700, false},
    {800, 2, 352.0f, -32.0f, 80, 700, true},
    {850, 3, 24.0f, -32.0f, 80, 700, false},
    {890, 3, 304.0f, -32.0f, 80, 700, true},
    {930, 2, 144.0f, -32.0f, 80, 700, false},
    {960, 3, 344.0f, -32.0f, 80, 700, true},
    {990, 3, 32.0f, -32.0f, 80, 700, false},
    {1020, 2, 240.0f, -32.0f, 80, 700, true},
    {1040, 3, 24.0f, -32.0f, 80, 700, false},
    {1060, 3, 304.0f, -32.0f, 80, 700, true},
    {1080, 2, 144.0f, -32.0f, 80, 700, false},
    {1090, 3, 344.0f, -32.0f, 80, 700, true},
    {1100, 2, 32.0f, -32.0f, 80, 700, false},
    {1110, 2, 240.0f, -32.0f, 80, 700, true},
};

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard) : m_Player(character, spellCard) {
    m_EnemyManager.SetTimeline(STAGE1_TIMELINE,
                               sizeof(STAGE1_TIMELINE) / sizeof(STAGE1_TIMELINE[0]));

    m_BgImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/stage1_bg.png");
    m_BgObj   = std::make_shared<Util::GameObject>(m_BgImage, -10.0f);
    m_BgObj->m_Transform.translation = {-96.0f, 901.0f};
    m_Renderer.AddChild(m_BgObj);
}

void Stage1::Update() {
    ++m_StageFrame;
    float scrollY = m_StageFrame * (BG_CANVAS_H - FIELD_H) / STAGE_TOTAL_FRAMES;
    m_BgObj->m_Transform.translation.y = (BG_CANVAS_H / 2.0f - FIELD_H / 2.0f) - scrollY;
    m_Renderer.Update();

    m_EnemyManager.Update(m_Player.GetPos(), m_GameManager);
    m_Player.Update(m_GameManager);

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

    m_Gui.Update(m_GameManager);
}

std::unique_ptr<Scene> Stage1::NextScene() {
    return std::make_unique<Title>();
}
