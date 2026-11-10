#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include "DreamBox/Scene.h"

namespace DreamBox
{
    class MainMenuScene : public Scene
    {
    public:
        explicit MainMenuScene(
            SDL_Renderer* renderer
        );

        ~MainMenuScene() override;

        SceneRequest Update(
            const Input& input,
            float deltaTime
        ) override;

        void Render(
            SDL_Renderer* renderer
        ) const override;

    private:
        enum class MenuOption
        {
            Play,
            Settings,
            Quit,
            Count
        };

        SDL_Texture* CreateTextTexture(
            SDL_Renderer* renderer,
            TTF_Font* font,
            const char* text
        );

        void DrawButton(
            SDL_Renderer* renderer,
            const SDL_FRect& button,
            SDL_Texture* textTexture,
            bool selected
        ) const;

        MenuOption m_selectedOption =
            MenuOption::Play;

        SDL_FRect m_playButton{
            490.0f, 280.0f, 300.0f, 60.0f
        };

        SDL_FRect m_settingsButton{
            490.0f, 360.0f, 300.0f, 60.0f
        };

        SDL_FRect m_quitButton{
            490.0f, 440.0f, 300.0f, 60.0f
        };

        // font related variables 
        TTF_Font* m_font = nullptr;
        TTF_Font* m_titleFont = nullptr;

        SDL_Texture* m_titleText = nullptr;

        SDL_Texture* m_playText = nullptr;
        SDL_Texture* m_settingsText = nullptr;
        SDL_Texture* m_quitText = nullptr;

        // footer 
        TTF_Font* m_footerFont = nullptr;
        SDL_Texture* m_footerText = nullptr;
    };
}