#ifndef UTIL_ASCII_TEXT_LINE_HPP
#define UTIL_ASCII_TEXT_LINE_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Util/Color.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

namespace Util {

enum class AsciiTextAlign {
    Left,
    Center,
    Right,
};

class AsciiTextLine {
   public:
    void Configure(Renderer& renderer, const Anm::Manager& anm, float zIndex);
    void SetText(const std::string& text, glm::vec2 pos, float scale = 1.0f,
                 AsciiTextAlign align = AsciiTextAlign::Left,
                 const Color&   color = Color::FromRGB(255, 255, 255));
    void SetLayout(glm::vec2 pos, float scale = 1.0f, AsciiTextAlign align = AsciiTextAlign::Left);
    void SetVisible(bool visible);
    void SetAlpha(float alpha);
    void SetZIndex(float zIndex);

   private:
    void EnsureCapacity(size_t count);

    Renderer*                                m_Renderer = nullptr;
    const Anm::Manager*                      m_Anm      = nullptr;
    std::vector<std::shared_ptr<GameObject>> m_Chars;
    std::vector<bool>                        m_DrawableChars;
    std::string                              m_Text;
    float                                    m_ZIndex  = 0.0f;
    bool                                     m_Visible = false;
};

}  // namespace Util

#endif  // UTIL_ASCII_TEXT_LINE_HPP
