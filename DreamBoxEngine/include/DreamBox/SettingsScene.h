
#pragma once

#include "DreamBox/Scene.h"
#include "DreamBox/Text.h"
#include "DreamBox/EngineSettings.h"

#include <memory>

namespace DreamBox
{
    class SettingsScene : public Scene
    {
    public:
        explicit SettingsScene(
            SDL_Renderer* renderer,
            const EngineSettings& settings,
            int selectedIndex = 0
        );

        ~SettingsScene() override = default;

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
            Fullscreen,
            VSync,
            Back,
            Count
        };

        void DrawButton(
            SDL_Renderer* renderer,
            const SDL_FRect& bounds,
            const Text* label,
            bool selected
        ) const;

        MenuOption m_selectedOption =
            MenuOption::Fullscreen;

        std::unique_ptr<Text> m_titleText;
        std::unique_ptr<Text> m_fullscreenText;
        std::unique_ptr<Text> m_vsyncText;
        std::unique_ptr<Text> m_backText;
        std::unique_ptr<Text> m_footerText;

        SDL_FRect m_fullscreenButton{
            490.0f, 280.0f, 300.0f, 60.0f
        };

        SDL_FRect m_vsyncButton{
            490.0f, 360.0f, 300.0f, 60.0f
        };

        SDL_FRect m_backButton{
            490.0f, 440.0f, 300.0f, 60.0f
        };
    };
}
