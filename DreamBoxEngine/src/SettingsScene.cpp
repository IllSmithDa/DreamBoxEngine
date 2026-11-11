
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

        m_fullscreenButton = std::make_unique<Button>(
            renderer,
            SDL_FRect{490.0f, 280.0f, 300.0f, 60.0f},
            regularFont,
            24.0f,
            settings.fullscreen
                ? "FULLSCREEN: ON"
                : "FULLSCREEN: OFF"
        );

        m_vsyncButton = std::make_unique<Button>(
            renderer,
            SDL_FRect{490.0f, 360.0f, 300.0f, 60.0f},
            regularFont,
            24.0f,
            settings.vsync
                ? "VSYNC: ON"
                : "VSYNC: OFF"
        );

        m_backButton = std::make_unique<Button>(
            renderer,
            SDL_FRect{490.0f, 440.0f, 300.0f, 60.0f},
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

        int selected = static_cast<int>(m_selectedOption);
        const int optionCount = static_cast<int>(MenuOption::Count);

        if (input.IsKeyPressed(SDL_SCANCODE_DOWN) ||
            input.IsKeyPressed(SDL_SCANCODE_S))
        {
            selected = m_keyboardSelectionActive
                ? (selected + 1) % optionCount
                : 0;

            m_keyboardSelectionActive = true;
            m_hoveredOption.reset();
        }
        else if (input.IsKeyPressed(SDL_SCANCODE_UP) ||
                 input.IsKeyPressed(SDL_SCANCODE_W))
        {
            selected = m_keyboardSelectionActive
                ? (selected - 1 + optionCount) % optionCount
                : optionCount - 1;

            m_keyboardSelectionActive = true;
            m_hoveredOption.reset();
        }

        m_selectedOption = static_cast<MenuOption>(selected);

        // Mouse movement switches visual focus away from the keyboard.
        if (input.WasMouseMoved())
        {
            m_keyboardSelectionActive = false;
            m_hoveredOption.reset();

            const float mouseX = input.GetMouseX();
            const float mouseY = input.GetMouseY();

            if (m_fullscreenButton &&
                m_fullscreenButton->Contains(mouseX, mouseY))
            {
                m_hoveredOption = MenuOption::Fullscreen;
            }
            else if (m_vsyncButton &&
                     m_vsyncButton->Contains(mouseX, mouseY))
            {
                m_hoveredOption = MenuOption::VSync;
            }
            else if (m_backButton &&
                     m_backButton->Contains(mouseX, mouseY))
            {
                m_hoveredOption = MenuOption::Back;
            }
        }

        // Mouse clicks activate the button at the click location.
        if (input.WasLeftMousePressed())
        {
            const float mouseX = input.GetClickX();
            const float mouseY = input.GetClickY();

            if (m_fullscreenButton &&
                m_fullscreenButton->Contains(mouseX, mouseY))
            {
                return SceneRequest::ToggleFullscreen;
            }

            if (m_vsyncButton &&
                m_vsyncButton->Contains(mouseX, mouseY))
            {
                return SceneRequest::ToggleVSync;
            }

            if (m_backButton &&
                m_backButton->Contains(mouseX, mouseY))
            {
                return SceneRequest::MainMenu;
            }
        }

        // Do not activate an invisible/stale keyboard selection.
        if (m_keyboardSelectionActive &&
            (input.IsKeyPressed(SDL_SCANCODE_RETURN) ||
             input.IsKeyPressed(SDL_SCANCODE_KP_ENTER)))
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

        if (m_fullscreenButton)
        {
            m_fullscreenButton->Render(
                renderer,
                m_keyboardSelectionActive &&
                    m_selectedOption == MenuOption::Fullscreen,
                m_hoveredOption == MenuOption::Fullscreen
            );
        }
        
        if (m_vsyncButton)
        {
            m_vsyncButton->Render(
                renderer,
                m_keyboardSelectionActive &&
                    m_selectedOption == MenuOption::VSync,
                m_hoveredOption == MenuOption::VSync
            );
        }
        
        if (m_backButton)
        {
            m_backButton->Render(
                renderer,
                m_keyboardSelectionActive &&
                    m_selectedOption == MenuOption::Back,
                m_hoveredOption == MenuOption::Back
            );
        }

        if (m_footerText)
        {
            m_footerText->RenderCentered(
                renderer, 640.0f, 650.0f
            );
        }
    }
}
