#include "DreamBox/MainMenuScene.h"
#include <SDL3/SDL_filesystem.h>
#include <string>

namespace DreamBox
    // constructor
    {
        MainMenuScene::MainMenuScene(
        SDL_Renderer* renderer
    )
    {
        const char* basePath = SDL_GetBasePath();
        if (!basePath)
        {
            SDL_Log(
                "Unable to get application path: %s",
                SDL_GetError()
            );
            return;
        }
        std::string regularFontPath =
            std::string(basePath) +
            "assets/fonts/Montserrat/Montserrat-Regular.ttf";
        std::string boldFontPath =
            std::string(basePath) +
            "assets/fonts/Montserrat/Montserrat-Bold.ttf";
        m_titleText = std::make_unique<Text>(
            renderer,
            boldFontPath,
            42.0f,
            "DREAMBOX ENGINE"
        );
        m_footerText = std::make_unique<Text>(
            renderer,
            regularFontPath,
            18.0f,
            "UP/DOWN or W/S - Navigate     ENTER - Select"
        );
        m_playButton = std::make_unique<Button>(
            renderer,
            SDL_FRect{490.0f, 280.0f, 300.0f, 60.0f},
            regularFontPath,
            28.0f,
            "PLAY"
        );
        
        m_settingsButton = std::make_unique<Button>(
            renderer,
            SDL_FRect{490.0f, 360.0f, 300.0f, 60.0f},
            regularFontPath,
            28.0f,
            "SETTINGS"
        );
        
        m_quitButton = std::make_unique<Button>(
            renderer,
            SDL_FRect{490.0f, 440.0f, 300.0f, 60.0f},
            regularFontPath,
            28.0f,
            "QUIT"
        );
    }


    // Update menu selection and dispatch user actions
    SceneRequest MainMenuScene::Update(
        const Input& input,
        float
    )
    {
        int selected =
            static_cast<int>(m_selectedOption);

        const int optionCount =
            static_cast<int>(MenuOption::Count);

        // Mouse movement takes control of the visual focus.
        // Leaving all buttons clears the highlight entirely.
        if (input.WasMouseMoved())
        {
            m_keyboardSelectionActive = false;
            m_hoveredOption.reset();

            const float mouseX = input.GetMouseX();
            const float mouseY = input.GetMouseY();

            if (m_playButton &&
                m_playButton->Contains(mouseX, mouseY))
            {
                m_hoveredOption = MenuOption::Play;
            }
            else if (m_settingsButton &&
                     m_settingsButton->Contains(mouseX, mouseY))
            {
                m_hoveredOption = MenuOption::Settings;
            }
            else if (m_quitButton &&
                     m_quitButton->Contains(mouseX, mouseY))
            {
                m_hoveredOption = MenuOption::Quit;
            }
        }

        // mouse input check
        if (input.WasLeftMousePressed())
        {
            const float mouseX = input.GetClickX();
            const float mouseY = input.GetClickY();
        
            if (m_playButton &&
                m_playButton->Contains(mouseX, mouseY))
            {
                return SceneRequest::StartGame;
            }
        
            if (m_settingsButton &&
                m_settingsButton->Contains(mouseX, mouseY))
            {
                return SceneRequest::Settings;
            }
        
            if (m_quitButton &&
                m_quitButton->Contains(mouseX, mouseY))
            {
                return SceneRequest::Quit;
            }
        }

        // keyboard checks
        if (input.IsKeyPressed(SDL_SCANCODE_DOWN) ||
            input.IsKeyPressed(SDL_SCANCODE_S))
        {
            if (!m_keyboardSelectionActive)
            {
                selected = 0;
            }
            else
            {
                selected = (selected + 1) % optionCount;
            }
        
            m_keyboardSelectionActive = true;
            m_hoveredOption.reset();
        }

        if (input.IsKeyPressed(SDL_SCANCODE_UP) ||
            input.IsKeyPressed(SDL_SCANCODE_W))
        {
            if (!m_keyboardSelectionActive)
            {
                selected = optionCount - 1;
            }
            else
            {
                selected =
                    (selected - 1 + optionCount) % optionCount;
            }
        
            m_keyboardSelectionActive = true;
            m_hoveredOption.reset();
        }

        m_selectedOption =
            static_cast<MenuOption>(selected);

        // Enter only activates a visibly keyboard-selected option.
        if (m_keyboardSelectionActive &&
            (input.IsKeyPressed(SDL_SCANCODE_RETURN) ||
             input.IsKeyPressed(SDL_SCANCODE_KP_ENTER)))
        {
            switch (m_selectedOption)
            {
            case MenuOption::Play:
                return SceneRequest::StartGame;

            case MenuOption::Settings:
                return SceneRequest::Settings;

            case MenuOption::Quit:
                return SceneRequest::Quit;

            default:
                return SceneRequest::None;
            }
        }

        return SceneRequest::None;
    }
    

    void MainMenuScene::Render(
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
                renderer,
                640.0f,
                150.0f
            );

            SDL_FRect underline{
                590.0f,
                150.0f + m_titleText->GetHeight() + 15.0f,
                100.0f,
                3.0f
            };

            SDL_SetRenderDrawColor(
                renderer,
                95, 150, 215, 255
            );

            SDL_RenderFillRect(
                renderer,
                &underline
            );
        }

        // Menu buttons
    if (m_playButton)
    {
        m_playButton->Render(
            renderer,
            m_selectedOption == MenuOption::Play &&
                m_keyboardSelectionActive,
            m_hoveredOption == MenuOption::Play
        );
    }

        if (m_settingsButton)
        {
            m_settingsButton->Render(
                renderer,
                m_selectedOption == MenuOption::Settings &&
                    m_keyboardSelectionActive,
                m_hoveredOption == MenuOption::Settings
            );
        }

        if (m_quitButton)
        {
            m_quitButton->Render(
                renderer,
                m_selectedOption == MenuOption::Quit &&
                    m_keyboardSelectionActive,
                m_hoveredOption == MenuOption::Quit
            );
        }

        // Footer
        if (m_footerText)
        {
            m_footerText->RenderCentered(
                renderer,
                640.0f,
                650.0f
            );
        }
    }


}