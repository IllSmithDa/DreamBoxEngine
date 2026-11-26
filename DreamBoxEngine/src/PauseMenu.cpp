
#include "DreamBox/PauseMenu.h"

namespace DreamBox
{
    PauseMenu::PauseMenu(
        SDL_Renderer* renderer,
        const std::string& fontPath
    )
    {
        m_titleText = std::make_unique<Text>(
            renderer,
            fontPath,
            36.0f,
            "GAME PAUSED"
        );

        m_resumeText = std::make_unique<Text>(
            renderer,
            fontPath,
            26.0f,
            "RESUME"
        );

        m_mainMenuText = std::make_unique<Text>(
            renderer,
            fontPath,
            26.0f,
            "MAIN MENU"
        );

        m_footerText = std::make_unique<Text>(
            renderer,
            fontPath,
            15.0f,
            "ESC - Resume   UP/DOWN - Navigate   ENTER - Select"
        );
    }

    void PauseMenu::Reset()
    {
        m_model.Reset();
    }

    PauseMenuAction PauseMenu::Update(
        const Input& input
    )
    {
        // Escape always resumes gameplay.
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            return PauseMenuAction::Resume;
        }

        // ----------------------------------
        // Keyboard navigation
        // ----------------------------------

        if (input.IsKeyPressed(SDL_SCANCODE_UP) ||
            input.IsKeyPressed(SDL_SCANCODE_W))
        {
            m_model.MoveSelection(-1);
        }
        else if (
            input.IsKeyPressed(SDL_SCANCODE_DOWN) ||
            input.IsKeyPressed(SDL_SCANCODE_S))
        {
            m_model.MoveSelection(1);
        }

        // ----------------------------------
        // Mouse hover
        // ----------------------------------

        if (input.WasMouseMoved())
        {
            const SDL_FPoint point{
                input.GetMouseX(),
                input.GetMouseY()
            };

            if (SDL_PointInRectFloat(
                    &point,
                    &m_resumeButton))
            {
                m_model.Select(
                    PauseMenuOption::Resume
                );
            }
            else if (SDL_PointInRectFloat(
                         &point,
                         &m_mainMenuButton))
            {
                m_model.Select(
                    PauseMenuOption::MainMenu
                );
            }
        }

        // ----------------------------------
        // Mouse clicks
        // ----------------------------------

        if (input.WasPointPressed())
        {
            const SDL_FPoint point =
                input.GetPointPosition();

            if (SDL_PointInRectFloat(
                    &point,
                    &m_resumeButton))
            {
                return PauseMenuAction::Resume;
            }

            if (SDL_PointInRectFloat(
                    &point,
                    &m_mainMenuButton))
            {
                return PauseMenuAction::MainMenu;
            }
        }

        // ----------------------------------
        // Keyboard selection
        // ----------------------------------

        if (input.IsKeyPressed(SDL_SCANCODE_RETURN) ||
            input.IsKeyPressed(SDL_SCANCODE_KP_ENTER))
        {
            switch (m_model.GetSelected())
            {
            case PauseMenuOption::Resume:
                return PauseMenuAction::Resume;

            case PauseMenuOption::MainMenu:
                return PauseMenuAction::MainMenu;

            default:
                break;
            }
        }

        return PauseMenuAction::None;
    }

    void PauseMenu::DrawButton(
        SDL_Renderer* renderer,
        const SDL_FRect& bounds,
        const Text* label,
        bool selected
    ) const
    {
        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer,
                82, 121, 168, 255
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer,
                53, 73, 96, 255
            );
        }

        SDL_RenderFillRect(renderer, &bounds);

        SDL_SetRenderDrawColor(
            renderer,
            selected ? 230 : 110,
            selected ? 235 : 130,
            selected ? 245 : 155,
            255
        );

        SDL_RenderRect(renderer, &bounds);

        if (label)
        {
            const float centerX =
                bounds.x + bounds.w / 2.0f;

            const float textY =
                bounds.y +
                (bounds.h - label->GetHeight()) / 2.0f;

            label->RenderCentered(
                renderer,
                centerX,
                textY
            );
        }
    }

    void PauseMenu::Render(
        SDL_Renderer* renderer
    ) const
    {
        if (!renderer)
        {
            return;
        }

        SDL_BlendMode previousBlendMode =
            SDL_BLENDMODE_NONE;

        SDL_GetRenderDrawBlendMode(
            renderer,
            &previousBlendMode
        );

        SDL_SetRenderDrawBlendMode(
            renderer,
            SDL_BLENDMODE_BLEND
        );

        // Darken the game world.
        const SDL_FRect backdrop{
            0.0f, 0.0f, 1280.0f, 720.0f
        };

        SDL_SetRenderDrawColor(
            renderer,
            0, 0, 0, 175
        );

        SDL_RenderFillRect(
            renderer,
            &backdrop
        );

        // Main pause panel.
        const SDL_FRect panel{
            420.0f, 190.0f, 440.0f, 340.0f
        };

        SDL_SetRenderDrawColor(
            renderer,
            36, 48, 68, 245
        );

        SDL_RenderFillRect(
            renderer,
            &panel
        );

        SDL_SetRenderDrawColor(
            renderer,
            110, 140, 180, 255
        );

        SDL_RenderRect(
            renderer,
            &panel
        );

        // Title.
        if (m_titleText)
        {
            m_titleText->RenderCentered(
                renderer,
                640.0f,
                248.0f
            );
        }

        // Menu options.
        DrawButton(
            renderer,
            m_resumeButton,
            m_resumeText.get(),
            m_model.GetSelected() ==
                PauseMenuOption::Resume
        );

        DrawButton(
            renderer,
            m_mainMenuButton,
            m_mainMenuText.get(),
            m_model.GetSelected() ==
                PauseMenuOption::MainMenu
        );

        // Footer.
        if (m_footerText)
        {
            m_footerText->RenderCentered(
                renderer,
                640.0f,
                495.0f
            );
        }

        SDL_SetRenderDrawBlendMode(
            renderer,
            previousBlendMode
        );
    }
}
