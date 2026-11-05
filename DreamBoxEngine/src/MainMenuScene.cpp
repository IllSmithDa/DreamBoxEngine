#include "DreamBox/MainMenuScene.h"

namespace DreamBox
{
    void MainMenuScene::Update(
        const Input& input,
        float deltaTime
    )
    {
        // Menu input will be added next.
    }

    void MainMenuScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        if (!renderer)
        {
            return;
        }

        DrawButton(
            renderer,
            m_playButton,
            m_selectedOption == MenuOption::Play
        );

        DrawButton(
            renderer,
            m_settingsButton,
            m_selectedOption == MenuOption::Settings
        );

        DrawButton(
            renderer,
            m_quitButton,
            m_selectedOption == MenuOption::Quit
        );
    }

    void MainMenuScene::Update(
        const Input& input,
        float deltaTime
    )
    {
        int selected =
            static_cast<int>(m_selectedOption);

        const int optionCount =
            static_cast<int>(MenuOption::Count);

        if (input.IsKeyPressed(SDL_SCANCODE_DOWN) ||
            input.IsKeyPressed(SDL_SCANCODE_S))
        {
            selected =
                (selected + 1) % optionCount;
        }

        if (input.IsKeyPressed(SDL_SCANCODE_UP) ||
            input.IsKeyPressed(SDL_SCANCODE_W))
        {
            selected =
                (selected - 1 + optionCount)
                % optionCount;
        }

        m_selectedOption =
            static_cast<MenuOption>(selected);
    }
}