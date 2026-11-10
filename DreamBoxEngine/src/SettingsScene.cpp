
#include "DreamBox/SettingsScene.h"

#include <SDL3/SDL_filesystem.h>
#include <string>

namespace DreamBox
{
    SettingsScene::SettingsScene(
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
            "FULLSCREEN: OFF (Coming Soon)"
        );

        m_vsyncText = std::make_unique<Text>(
            renderer,
            regularFont,
            24.0f,
            "VSYNC: ON (Coming Soon)"
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
            "ESC or ENTER - Return to Main Menu"
        );
    }

    SceneRequest SettingsScene::Update(
        const Input& input,
        float
    )
    {
        if (
            input.IsKeyPressed(SDL_SCANCODE_ESCAPE) ||
            input.IsKeyPressed(SDL_SCANCODE_RETURN) ||
            input.IsKeyPressed(SDL_SCANCODE_KP_ENTER)
        )
        {
            return SceneRequest::MainMenu;
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

        // Settings information
        if (m_fullscreenText)
        {
            m_fullscreenText->RenderCentered(
                renderer,
                640.0f,
                290.0f
            );
        }

        if (m_vsyncText)
        {
            m_vsyncText->RenderCentered(
                renderer,
                640.0f,
                370.0f
            );
        }

        // Back button background
        SDL_SetRenderDrawColor(
            renderer,
            95, 130, 180, 255
        );

        SDL_RenderFillRect(
            renderer,
            &m_backButton
        );

        // Back button border
        SDL_SetRenderDrawColor(
            renderer,
            230, 235, 245, 255
        );

        SDL_RenderRect(
            renderer,
            &m_backButton
        );

        // Center Back label within button
        if (m_backText)
        {
            float textY =
                m_backButton.y +
                (m_backButton.h -
                 m_backText->GetHeight()) / 2.0f;

            m_backText->RenderCentered(
                renderer,
                m_backButton.x +
                    m_backButton.w / 2.0f,
                textY
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
