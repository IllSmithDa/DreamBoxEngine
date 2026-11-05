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

        // Play
        SDL_SetRenderDrawColor(
            renderer,
            80,
            180,
            255,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &m_playButton
        );

        // Settings
        SDL_SetRenderDrawColor(
            renderer,
            120,
            120,
            140,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &m_settingsButton
        );

        // Quit
        SDL_SetRenderDrawColor(
            renderer,
            180,
            80,
            80,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &m_quitButton
        );
    }
}