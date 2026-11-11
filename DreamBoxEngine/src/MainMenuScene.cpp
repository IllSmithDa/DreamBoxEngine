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


    // update the render
    SceneRequest MainMenuScene::Update(
        const Input& input,
        float
    )
    {
        int selected =
            static_cast<int>(m_selectedOption);

        const int optionCount =
            static_cast<int>(MenuOption::Count);

        // mouse coordinates check
        if (input.WasMouseMoved())
        {
            const float mouseX = input.GetMouseX();
            const float mouseY = input.GetMouseY();
            
            // temporary mouse coordinate input check
            SDL_Log(
                "Mouse position: X=%.1f, Y=%.1f",
                input.GetMouseX(),
                input.GetMouseY()
            );

            // Clear the previous hover state.
            m_hoveredOption.reset();
        
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
            selected = (selected + 1) % optionCount;
            m_hoveredOption.reset();
        }

        if (input.IsKeyPressed(SDL_SCANCODE_UP) ||
            input.IsKeyPressed(SDL_SCANCODE_W))
        {
            selected = (selected - 1 + optionCount) % optionCount;
            m_hoveredOption.reset();
        }

        m_selectedOption =
            static_cast<MenuOption>(selected);

        if (input.IsKeyPressed(SDL_SCANCODE_RETURN) ||
            input.IsKeyPressed(SDL_SCANCODE_KP_ENTER))
        {
            switch (m_selectedOption)
            {
            case MenuOption::Play:
                return SceneRequest::StartGame;

            case MenuOption::Settings:
                // SettingsScene doesn't exist yet.
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
                    !m_hoveredOption.has_value(),
                m_hoveredOption == MenuOption::Play
            );
        }

        if (m_settingsButton)
        {
            m_settingsButton->Render(
                renderer,
                m_selectedOption == MenuOption::Settings &&
                    !m_hoveredOption.has_value(),
                m_hoveredOption == MenuOption::Settings
            );
        }

        if (m_quitButton)
        {
            m_quitButton->Render(
                renderer,
                m_selectedOption == MenuOption::Quit &&
                    !m_hoveredOption.has_value(),
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