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

            m_playText = std::make_unique<Text>(
                renderer,
                regularFontPath,
                28.0f,
                "PLAY"
            );

            m_settingsText = std::make_unique<Text>(
                renderer,
                regularFontPath,
                28.0f,
                "SETTINGS"
            );

            m_quitText = std::make_unique<Text>(
                renderer,
                regularFontPath,
                28.0f,
                "QUIT"
            );

            m_footerText = std::make_unique<Text>(
                renderer,
                regularFontPath,
                18.0f,
                "UP/DOWN or W/S - Navigate     ENTER - Select"
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
                (selected - 1 + optionCount)
                % optionCount;
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
                return SceneRequest::None;

            case MenuOption::Quit:
                return SceneRequest::Quit;

            default:
                return SceneRequest::None;
            }
        }

        return SceneRequest::None;
    }

    // Draw button using the Montserrat font.

    void MainMenuScene::DrawButton(
        SDL_Renderer* renderer,
        const SDL_FRect& button,
        const Text* text,
        bool selected
    ) const
    {
        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer,
                95, 130, 180, 255
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer,
                55, 65, 85, 255
            );
        }

        SDL_RenderFillRect(
            renderer,
            &button
        );

        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer,
                230, 235, 245, 255
            );

            SDL_RenderRect(
                renderer,
                &button
            );
        }

        if (!text)
        {
            return;
        }

        float textX =
            button.x + button.w / 2.0f;

        float textY =
            button.y +
            (button.h - text->GetHeight()) / 2.0f;

        text->RenderCentered(
            renderer,
            textX,
            textY
        );
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
        DrawButton(
            renderer,
            m_playButton,
            m_playText.get(),
            m_selectedOption == MenuOption::Play
        );

        DrawButton(
            renderer,
            m_settingsButton,
            m_settingsText.get(),
            m_selectedOption == MenuOption::Settings
        );

        DrawButton(
            renderer,
            m_quitButton,
            m_quitText.get(),
            m_selectedOption == MenuOption::Quit
        );

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