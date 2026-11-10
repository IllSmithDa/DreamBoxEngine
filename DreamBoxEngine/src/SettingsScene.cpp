
#include "DreamBox/SettingsScene.h"

#include <SDL3/SDL_filesystem.h>
#include <string>

namespace DreamBox
{
    SettingsScene::SettingsScene(
        SDL_Renderer* renderer,
        const EngineSettings& settings,
        int selectedIndex
    )
    {
        if (selectedIndex >= 0 &&
            selectedIndex <
                static_cast<int>(MenuOption::Count))
        {
            m_selectedOption =
                static_cast<MenuOption>(selectedIndex);
        }

        const char* basePath = SDL_GetBasePath();

        if (!basePath)
        {
            SDL_Log(
                "Unable to get application path: %s",
                SDL_GetError()
            );
            return;
        }

        std::string regularFont =
            std::string(basePath) +
            "assets/fonts/Montserrat/Montserrat-Regular.ttf";

        std::string boldFont =
            std::string(basePath) +
            "assets/fonts/Montserrat/Montserrat-Bold.ttf";

        m_titleText = std::make_unique<Text>(
            renderer,
            boldFont,
            42.0f,
            "SETTINGS"
        );

        m_fullscreenText = std::make_unique<Text>(
            renderer,
            regularFont,
            24.0f,
            settings.fullscreen
                ? "FULLSCREEN: ON"
                : "FULLSCREEN: OFF"
        );

        m_vsyncText = std::make_unique<Text>(
            renderer,
            regularFont,
            24.0f,
            settings.vsync
                ? "VSYNC: ON"
                : "VSYNC: OFF"
        );

        m_backText = std::make_unique<Text>(
            renderer,
            regularFont,
            28.0f,
            "BACK"
        );

        m_footerText = std::make_unique<Text>(
            renderer,
            regularFont,
            18.0f,
            "UP/DOWN - Navigate    ENTER - Select    ESC - Back"
        );
    }

    SceneRequest SettingsScene::Update(
        const Input& input,
        float
    )
    {
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            return SceneRequest::MainMenu;
        }

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
                (selected - 1 + optionCount) %
                optionCount;
        }

        m_selectedOption =
            static_cast<MenuOption>(selected);

        if (input.IsKeyPressed(SDL_SCANCODE_RETURN) ||
            input.IsKeyPressed(SDL_SCANCODE_KP_ENTER))
        {
            switch (m_selectedOption)
            {
            case MenuOption::Fullscreen:
                return SceneRequest::ToggleFullscreen;

            case MenuOption::VSync:
                return SceneRequest::ToggleVSync;

            case MenuOption::Back:
                return SceneRequest::MainMenu;

            default:
                break;
            }
        }

        return SceneRequest::None;
    }

    void SettingsScene::DrawButton(
        SDL_Renderer* renderer,
        const SDL_FRect& bounds,
        const Text* label,
        bool selected
    ) const
    {
        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer, 95, 130, 180, 255
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer, 55, 65, 85, 255
            );
        }

        SDL_RenderFillRect(renderer, &bounds);

        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer, 230, 235, 245, 255
            );

            SDL_RenderRect(renderer, &bounds);
        }

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

    void SettingsScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        if (!renderer)
        {
            return;
        }

        // Title
        if (m_titleText)
        {
            m_titleText->RenderCentered(
                renderer, 640.0f, 150.0f
            );

            SDL_FRect underline{
                590.0f,
                150.0f + m_titleText->GetHeight() + 15.0f,
                100.0f,
                3.0f
            };

            SDL_SetRenderDrawColor(
                renderer, 95, 150, 215, 255
            );

            SDL_RenderFillRect(renderer, &underline);
        }

        DrawButton(
            renderer,
            m_fullscreenButton,
            m_fullscreenText.get(),
            m_selectedOption == MenuOption::Fullscreen
        );

        DrawButton(
            renderer,
            m_vsyncButton,
            m_vsyncText.get(),
            m_selectedOption == MenuOption::VSync
        );

        DrawButton(
            renderer,
            m_backButton,
            m_backText.get(),
            m_selectedOption == MenuOption::Back
        );

        if (m_footerText)
        {
            m_footerText->RenderCentered(
                renderer, 640.0f, 650.0f
            );
        }
    }
}
