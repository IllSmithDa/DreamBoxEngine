
#pragma once

#include "DreamBox/Text.h"
#include "DreamBox/Scene.h"
#include "DreamBox/Button.h"
#include <memory>


namespace DreamBox
{
    class MainMenuScene : public Scene
    {
    public:
        explicit MainMenuScene(
            SDL_Renderer* renderer
        );

        ~MainMenuScene() override = default;

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

        void DrawButton(
            SDL_Renderer* renderer,
            const SDL_FRect& button,
            const Text* text,
            bool selected
        ) const;

        MenuOption m_selectedOption =
            MenuOption::Play;

        std::unique_ptr<Button> m_playButton{
            490.0f, 280.0f, 300.0f, 60.0f
        };

        std::unique_ptr<Button> m_settingsButton{
            490.0f, 360.0f, 300.0f, 60.0f
        };

        std::unique_ptr<Button> m_quitButton{
            490.0f, 440.0f, 300.0f, 60.0f
        };

        std::unique_ptr<Text> m_titleText;

        std::unique_ptr<Text> m_playText;
        std::unique_ptr<Text> m_settingsText;
        std::unique_ptr<Text> m_quitText;

        std::unique_ptr<Text> m_footerText;
    };
}
