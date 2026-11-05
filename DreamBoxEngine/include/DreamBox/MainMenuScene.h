#pragma once

#include "DreamBox/Scene.h"

namespace DreamBox
{
    class MainMenuScene : public Scene
    {
    public:
        MainMenuScene() = default;

        void Update(
            const Input& input,
            float deltaTime
        ) override;

        void Render(
            SDL_Renderer* renderer
        ) const override;

    private:
        SDL_FRect m_playButton{
            490.0f,
            280.0f,
            300.0f,
            60.0f
        };

        SDL_FRect m_settingsButton{
            490.0f,
            360.0f,
            300.0f,
            60.0f
        };

        SDL_FRect m_quitButton{
            490.0f,
            440.0f,
            300.0f,
            60.0f
        };
    };
}