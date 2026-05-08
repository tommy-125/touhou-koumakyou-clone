#ifndef SCENE_STAGE_CLEAR_OVERLAY_HPP
#define SCENE_STAGE_CLEAR_OVERLAY_HPP

#include <memory>
#include <string>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "GameManager.hpp"
#include "Util/AsciiTextLine.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"

class StageClearOverlay {
   public:
    void Init();
    void Start(GameManager& gm, int stageBonus);
    bool Update();

    bool HasStarted() const { return m_Started; }
    bool IsFinished() const { return m_Finished; }

   private:
    void ShowText();
    void SetLine(size_t idx, const std::string& text, float y, float scale,
                 const Util::Color& color);
    void UpdateLoading();

    Util::Renderer                    m_Renderer;
    Anm::Manager                      m_Anm;
    std::shared_ptr<Util::Image>      m_LoadingImage;
    std::shared_ptr<Util::GameObject> m_LoadingObj;
    std::vector<Util::AsciiTextLine>  m_Lines;

    bool m_Initialized = false;
    bool m_Started     = false;
    bool m_TextShown   = false;
    bool m_Finished    = false;
    int  m_Timer       = 0;
    int  m_StageBonus  = 0;
    int  m_ClearScore  = 0;
    const GameManager* m_GameManager = nullptr;
};

#endif  // SCENE_STAGE_CLEAR_OVERLAY_HPP
