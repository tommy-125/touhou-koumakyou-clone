#include "Util/AsciiTextLine.hpp"

#include "Anm/AnmDefs.hpp"

namespace {
constexpr float ASCII_CHAR_SIZE    = 16.0f;
constexpr float ASCII_CHAR_ADVANCE = 14.0f;
constexpr int   ASCII_FIRST_CHAR   = 0x15;
}  // namespace

namespace Util {

void AsciiTextLine::Configure(Renderer& renderer, const Anm::Manager& anm, float zIndex) {
    m_Renderer = &renderer;
    m_Anm      = &anm;
    m_ZIndex   = zIndex;
}

void AsciiTextLine::EnsureCapacity(size_t count) {
    if (!m_Renderer) return;

    while (m_Chars.size() < count) {
        auto obj = std::make_shared<GameObject>(nullptr, m_ZIndex);
        obj->SetVisible(false);
        m_Renderer->AddChild(obj);
        m_Chars.push_back(obj);
        m_DrawableChars.push_back(false);
    }
}

void AsciiTextLine::SetText(const std::string& text, glm::vec2 pos, float scale,
                            AsciiTextAlign align, const Color&) {
    EnsureCapacity(text.size());
    m_Text = text;

    const float width = text.empty()
                            ? 0.0f
                            : (static_cast<float>(text.size() - 1) * ASCII_CHAR_ADVANCE +
                               ASCII_CHAR_SIZE) *
                                  scale;
    float firstCenterX = pos.x + ASCII_CHAR_SIZE * scale * 0.5f;
    if (align == AsciiTextAlign::Center) {
        firstCenterX = pos.x - width * 0.5f + ASCII_CHAR_SIZE * scale * 0.5f;
    } else if (align == AsciiTextAlign::Right) {
        firstCenterX = pos.x - width + ASCII_CHAR_SIZE * scale * 0.5f;
    }

    const float advance = ASCII_CHAR_ADVANCE * scale;
    for (size_t i = 0; i < m_Chars.size(); i++) {
        auto& obj = m_Chars[i];
        if (!obj) continue;

        if (i >= text.size()) {
            m_DrawableChars[i] = false;
            obj->SetVisible(false);
            continue;
        }

        obj->m_Transform.translation = {firstCenterX + static_cast<float>(i) * advance, pos.y};
        obj->m_Transform.scale       = {scale, scale};
        obj->SetZIndex(m_ZIndex);

        m_DrawableChars[i] = false;
        const auto ch = static_cast<unsigned char>(text[i]);
        if (m_Anm && ch != ' ' && ch >= ASCII_FIRST_CHAR) {
            const int spriteIdx = Anm::ASCII.offset + static_cast<int>(ch) - ASCII_FIRST_CHAR;
            if (spriteIdx >= 0 && spriteIdx < Anm::Manager::MAX_ENTRIES &&
                m_Anm->sprites[spriteIdx].image) {
                obj->SetDrawable(m_Anm->sprites[spriteIdx].image);
                m_DrawableChars[i] = true;
            }
        }
        obj->SetVisible(m_Visible && m_DrawableChars[i]);
    }
}

void AsciiTextLine::SetLayout(glm::vec2 pos, float scale, AsciiTextAlign align) {
    const float width = m_Text.empty()
                            ? 0.0f
                            : (static_cast<float>(m_Text.size() - 1) * ASCII_CHAR_ADVANCE +
                               ASCII_CHAR_SIZE) *
                                  scale;
    float firstCenterX = pos.x + ASCII_CHAR_SIZE * scale * 0.5f;
    if (align == AsciiTextAlign::Center) {
        firstCenterX = pos.x - width * 0.5f + ASCII_CHAR_SIZE * scale * 0.5f;
    } else if (align == AsciiTextAlign::Right) {
        firstCenterX = pos.x - width + ASCII_CHAR_SIZE * scale * 0.5f;
    }

    const float advance = ASCII_CHAR_ADVANCE * scale;
    for (size_t i = 0; i < m_Chars.size(); i++) {
        auto& obj = m_Chars[i];
        if (!obj) continue;
        obj->m_Transform.translation = {firstCenterX + static_cast<float>(i) * advance, pos.y};
        obj->m_Transform.scale       = {scale, scale};
    }
}

void AsciiTextLine::SetVisible(bool visible) {
    m_Visible = visible;
    for (size_t i = 0; i < m_Chars.size(); i++) {
        if (m_Chars[i]) m_Chars[i]->SetVisible(visible && m_DrawableChars[i]);
    }
}

void AsciiTextLine::SetAlpha(float alpha) {
    for (auto& obj : m_Chars) {
        if (obj) obj->SetAlpha(alpha);
    }
}

void AsciiTextLine::SetZIndex(float zIndex) {
    m_ZIndex = zIndex;
    for (auto& obj : m_Chars) {
        if (obj) obj->SetZIndex(zIndex);
    }
}

}  // namespace Util
