
#pragma once

#include "DreamBox/Scene.h"
#include "DreamBox/Text.h"
#include "DreamBox/EngineSettings.h"
#include "DreamBox/Button.h"
#include <memory>
#include <optional>

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
        std::unique_ptr<Button> m_fullscreenButton;
        std::unique_ptr<Button> m_vsyncButton;
        std::unique_ptr<Button> m_backButton;
        std::unique_ptr<Text> m_footerText;
        std::optional<MenuOption> m_hoveredOption;
        bool m_keyboardSelectionActive = false;
    };
}
