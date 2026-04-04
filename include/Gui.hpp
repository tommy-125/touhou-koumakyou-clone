#ifndef GUI_HPP
#define GUI_HPP

#include <memory>

#include "Anm/AnmManager.hpp"
#include "GameManager.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"

class Gui {
   public:
    Gui();
    void Update(const GameManager& gm);

   private:
    static constexpr int NUM_DECO  = 6;  // front scripts 0–5
    static constexpr int NUM_LABEL = 7;  // front scripts 9–15
    static constexpr int MAX_LIVES = 8;
    static constexpr int MAX_BOMBS = 8;

    Anm::Manager   m_Anm;
    Util::Renderer m_Renderer;

    std::shared_ptr<Util::Image>      m_BgImage;
    std::shared_ptr<Util::GameObject> m_BgObj;

    Anm::Vm m_DecoVms[NUM_DECO];
    Anm::Vm m_LabelVms[NUM_LABEL];
    Anm::Vm m_LifeVms[MAX_LIVES];
    Anm::Vm m_BombVms[MAX_BOMBS];

    std::shared_ptr<Util::Text>       m_HiScoreText;
    std::shared_ptr<Util::Text>       m_ScoreText;
    std::shared_ptr<Util::Text>       m_PowerText;
    std::shared_ptr<Util::Text>       m_GrazeText;
    std::shared_ptr<Util::GameObject> m_HiScoreObj;
    std::shared_ptr<Util::GameObject> m_ScoreObj;
    std::shared_ptr<Util::GameObject> m_PowerObj;
    std::shared_ptr<Util::GameObject> m_GrazeObj;

    int m_LastLives   = -1;
    int m_LastBombs   = -1;
    int m_LastScore   = -1;
    int m_LastHiScore = -1;
    int m_LastPower   = -1;
    int m_LastGraze   = -1;
};

#endif  // GUI_HPP
