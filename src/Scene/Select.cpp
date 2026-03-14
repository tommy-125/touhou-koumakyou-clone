#include "Scene/Select.hpp"

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include <memory>

#include "Scene/Title.hpp"

Select::Select() : m_EnterSelectBlackMask(2.0f, 1.0f) {
    // Background
    auto bgImage = std::make_shared<Util::Image>(
        GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/select00.jpg"
    );
    m_BgObj = std::make_shared<Util::GameObject>(bgImage, 0.0f);
    m_Renderer.AddChild(m_BgObj);

    // Load ANM
    std::vector<Anm::LoadedEntry> loaded = {
        {&Anm::SELECT01, m_Anm.LoadAnm(Anm::SELECT01.folder, Anm::SELECT01.txt, Anm::SELECT01.offset)},
        {&Anm::SELECT02, m_Anm.LoadAnm(Anm::SELECT02.folder, Anm::SELECT02.txt, Anm::SELECT02.offset)},
        {&Anm::SELECT03, m_Anm.LoadAnm(Anm::SELECT03.folder, Anm::SELECT03.txt, Anm::SELECT03.offset)},
        {&Anm::SELECT04, m_Anm.LoadAnm(Anm::SELECT04.folder, Anm::SELECT04.txt, Anm::SELECT04.offset)},
        {&Anm::SELECT05, m_Anm.LoadAnm(Anm::SELECT05.folder, Anm::SELECT05.txt, Anm::SELECT05.offset)},
        {&Anm::SLPL00A, m_Anm.LoadAnm(Anm::SLPL00A.folder, Anm::SLPL00A.txt, Anm::SLPL00A.offset)},
        {&Anm::SLPL00B, m_Anm.LoadAnm(Anm::SLPL00B.folder, Anm::SLPL00B.txt, Anm::SLPL00B.offset)},
        {&Anm::SLPL01A, m_Anm.LoadAnm(Anm::SLPL01A.folder, Anm::SLPL01A.txt, Anm::SLPL01A.offset)},
        {&Anm::SLPL01B, m_Anm.LoadAnm(Anm::SLPL01B.folder, Anm::SLPL01B.txt, Anm::SLPL01B.offset)},
    };

    int total = 0;
    for (auto &e : loaded) total += e.scriptCount;
    m_Vms.resize(total);
    m_Objs.resize(total);


    m_DifficultyItemVms.resize(SELECT_DIFFICULTY_COUNT);
    m_DifficultyItemObjs.resize(SELECT_DIFFICULTY_COUNT);
    // Init each VM and GameObject
    int vmIdx = 0;
    int difficultyVmIdx = 0;
    for (auto &e : loaded) {
        for (int i = 0; i < e.scriptCount; i++, vmIdx++) {
            m_Vms[vmIdx].scriptIdx    = e.entry->offset + i;
            m_Vms[vmIdx].spriteOffset = e.entry->offset;

            m_Objs[vmIdx] = std::make_shared<Util::GameObject>(nullptr, 1.0f, glm::vec2{0, 0}, false);
            m_Renderer.AddChild(m_Objs[vmIdx]);

            if(e.entry == &Anm::SELECT01 || e.entry == &Anm::SELECT02) { // skip idx of 0 since it is difficulty select title
                if(difficultyVmIdx == 0) {
                    m_DifficultyTitleVm = &m_Vms[vmIdx];
                } else {
                    m_DifficultyItemVms[difficultyVmIdx-1] = &m_Vms[vmIdx];
                    m_DifficultyItemObjs[difficultyVmIdx-1] = m_Objs[vmIdx];
                }
                difficultyVmIdx++;
            }
        }
    }
    m_EnterSelectBlackMask.Fade(30, 0.0f); // start with black mask fully transparent
    m_Renderer.AddChild(m_EnterSelectBlackMask.GetObj());
    HandleInterruptEvent(SELECT_EVENT_ENTER_DIFFICULTY_SELECT);
}

void Select::Update() {
    m_EnterSelectBlackMask.Update();
    switch (m_CurrentState) {
        case SelectState::Difficulty:
            if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
                m_SelectedDifficultyItemIdx = (m_SelectedDifficultyItemIdx - 1 + SELECT_DIFFICULTY_COUNT) % SELECT_DIFFICULTY_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
                m_SelectedDifficultyItemIdx = (m_SelectedDifficultyItemIdx + 1) % SELECT_DIFFICULTY_COUNT;
            } else if(Util::Input::IsKeyDown(Util::Keycode::Z)) {
                m_CurrentState = SelectState::Character;
                HandleInterruptEvent(SELECT_EVENT_ENTER_CHARA_SELECT);

            } else if (Util::Input::IsKeyDown(Util::Keycode::X)) {
                HandleInterruptEvent(SELECT_EVENT_RETURN_TITLE);
                m_EnterSelectBlackMask.Fade(30, 1.0f); // fade to black before returning to title
                m_Quitting = true; // start quit timer and animation
            }
            m_SelectedDifficultyItem = static_cast<DifficultyItem>(m_SelectedDifficultyItemIdx);
            
            break;
        case SelectState::Character:
            if (Util::Input::IsKeyDown(Util::Keycode::LEFT)) {
                m_SelectedCharacterItemIdx = (m_SelectedCharacterItemIdx - 1 + SELECT_CHARACTER_COUNT) % SELECT_CHARACTER_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
                m_SelectedCharacterItemIdx = (m_SelectedCharacterItemIdx + 1) % SELECT_CHARACTER_COUNT;
            } else if(Util::Input::IsKeyDown(Util::Keycode::Z)) {
                m_CurrentState = SelectState::SpellCard;

            } else if (Util::Input::IsKeyDown(Util::Keycode::X)) {
                m_CurrentState = SelectState::Difficulty;
                HandleInterruptEvent(SELECT_EVENT_ENTER_DIFFICULTY_SELECT);
            }
            m_SelectedCharacterItem = static_cast<CharacterItem>(m_SelectedCharacterItemIdx);
            break;
        case SelectState::SpellCard:
            if (Util::Input::IsKeyDown(Util::Keycode::LEFT)) {
                m_SelectedSpellCardItemIdx = (m_SelectedSpellCardItemIdx - 1 + SELECT_SPELL_CARD_COUNT) % SELECT_SPELL_CARD_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
                m_SelectedSpellCardItemIdx = (m_SelectedSpellCardItemIdx + 1) % SELECT_SPELL_CARD_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
                m_Done = true; // TODO: Proceed to next scene based on selected character and spell card
            } else if (Util::Input::IsKeyDown(Util::Keycode::X)) {
                m_CurrentState = SelectState::Character;
            }

            switch (m_SelectedCharacterItem) {
                case CharacterItem::Reimu:
                    m_SelectedReimuSpellCardItem = static_cast<ReimuSpellCardItem>(m_SelectedSpellCardItemIdx);
                    break;
                case CharacterItem::Marisa:
                    m_SelectedMarisaSpellCardItem = static_cast<MarisaSpellCardItem>(m_SelectedSpellCardItemIdx);
                    break;
            }
            break;
    }

    if(m_Quitting) {
        m_QuitTimer++;
        if(m_QuitTimer >= 30) { // 30 frames of animation
            m_Done = true;
        }
    }
    
    m_Anm.UpdateObjects(m_Vms, m_Objs);

    // overwrite alpha value of difficulty items of the original instructions
    for(int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
        if(m_CurrentState == SelectState::Difficulty) {
            if(i == m_SelectedDifficultyItemIdx) {
                m_DifficultyItemObjs[i]->SetAlpha(1.0f);
            } else {
                m_DifficultyItemObjs[i]->SetAlpha(0.5f);
            }
        }
    }

    m_Renderer.Update();
}

std::unique_ptr<Scene> Select::NextScene() {
    if(m_Done) {
        switch (m_CurrentState) {
            case SelectState::Difficulty:
                return std::make_unique<Title>(); // go back to title
            case SelectState::Character:
                break;
            case SelectState::SpellCard:
                break;
        }
    }
    return nullptr;
}

void Select::HandleInterruptEvent(const int interruptEvent) {
    if(interruptEvent == SELECT_EVENT_ENTER_DIFFICULTY_SELECT) {
        m_Anm.SendInterrupt(*m_DifficultyTitleVm, SELECT_INTERRUPT_ENTER_DIFFICULTY_SELECT);
        for(int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
            m_Anm.SendInterrupt(*m_DifficultyItemVms[i], SELECT_INTERRUPT_ENTER_DIFFICULTY_SELECT);
        }
    } else if (interruptEvent == SELECT_EVENT_ENTER_CHARA_SELECT) {
        for(int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
            m_Anm.SendInterrupt(*m_DifficultyTitleVm, SELECT_INTERRUPT_UNSELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT);
            if(i == m_SelectedDifficultyItemIdx) {
                m_Anm.SendInterrupt(*m_DifficultyItemVms[i], SELECT_INTERRUPT_SELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT);
            } else {
                m_Anm.SendInterrupt(*m_DifficultyItemVms[i], SELECT_INTERRUPT_UNSELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT);
            }
        }
    } else if (interruptEvent == SELECT_EVENT_RETURN_TITLE) {
        m_Anm.SendInterrupt(*m_DifficultyTitleVm, SELECT_INTERRUPT_RETURN_TITLE);
        for(int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
            m_Anm.SendInterrupt(*m_DifficultyItemVms[i], SELECT_INTERRUPT_RETURN_TITLE);
        }
    }
}