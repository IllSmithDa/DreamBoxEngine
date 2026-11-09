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
        const char* basePath =
            SDL_GetBasePath();
    
        if (!basePath)
        {
            SDL_Log(
                "Unable to get application path: %s",
                SDL_GetError()
            );
        
            return;
        }
    
        std::string fontPath =
            std::string(basePath) +
            "assets/fonts/Montserrat/Montserrat-Regular.ttf";
    
        m_font = TTF_OpenFont(
            fontPath.c_str(),
            28.0f
        );
    
        if (!m_font)
        {
            SDL_Log(
                "Unable to load font: %s",
                SDL_GetError()
            );
        
            return;
        }

        // load bold font with size 42
        std::string boldFontPath =
            std::string(basePath) +
            "assets/fonts/Montserrat/Montserrat-Bold.ttf";

        m_titleFont = TTF_OpenFont(
            boldFontPath.c_str(),
            42.0f
        );
    
        m_titleText =
            CreateTextTexture(
                renderer,
                m_titleFont,
                "DREAMBOX ENGINE"
            );
        
        m_playText =
            CreateTextTexture(
                renderer,
                m_font,
                "PLAY"
            );
        
        m_settingsText =
            CreateTextTexture(
                renderer,
                m_font,
                "SETTINGS"
            );
        
        m_quitText =
            CreateTextTexture(
                renderer,
                m_font,
                "QUIT"
            );
    }

    // destructor
    MainMenuScene::~MainMenuScene()
    {
        SDL_DestroyTexture(m_playText);
        SDL_DestroyTexture(m_settingsText);
        SDL_DestroyTexture(m_quitText);
        SDL_DestroyTexture(m_titleText);

        if (m_titleFont)
        {
            TTF_CloseFont(m_titleFont);
            m_titleFont = nullptr;
        }
        if (m_font)
        {
            TTF_CloseFont(m_font);
            m_font = nullptr;
        }
    }

    // create the textures for the scene
    SDL_Texture* MainMenuScene::CreateTextTexture(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const char* text
    )
    {
        SDL_Color color{
            255,
            255,
            255,
            255
        };
    
        SDL_Surface* surface =
            TTF_RenderText_Blended(
                font,
                text,
                0,
                color
            );
        
        if (!surface)
        {
            return nullptr;
        }
    
        SDL_Texture* texture =
            SDL_CreateTextureFromSurface(
                renderer,
                surface
            );
        
        SDL_DestroySurface(surface);
        
        return texture;
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
        SDL_Texture* textTexture,
        bool selected
    ) const
    {
        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer,
                95,
                130,
                180,
                255
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer,
                55,
                65,
                85,
                255
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
                230,
                235,
                245,
                255
            );
        
            SDL_RenderRect(
                renderer,
                &button
            );
        }
    
        if (!textTexture)
        {
            return;
        }
    
        float textWidth = 0.0f;
        float textHeight = 0.0f;
    
        SDL_GetTextureSize(
            textTexture,
            &textWidth,
            &textHeight
        );
    
        SDL_FRect textRect{
            button.x + (button.w - textWidth) / 2.0f,
            button.y + (button.h - textHeight) / 2.0f,
            textWidth,
            textHeight
        };
    
        SDL_RenderTexture(
            renderer,
            textTexture,
            nullptr,
            &textRect
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

        if (m_titleText)
        {
            float titleWidth = 0.0f;
            float titleHeight = 0.0f;

            SDL_GetTextureSize(
                m_titleText,
                &titleWidth,
                &titleHeight
            );

            SDL_FRect titleRect{
                640.0f - titleWidth / 2.0f,
                175.0f,
                titleWidth,
                titleHeight
            };

            SDL_RenderTexture(
                renderer,
                m_titleText,
                nullptr,
                &titleRect
            );
        }

        DrawButton(
            renderer,
            m_playButton,
            m_playText,
            m_selectedOption == MenuOption::Play
        );

        DrawButton(
            renderer,
            m_settingsButton,
            m_settingsText,
            m_selectedOption == MenuOption::Settings
        );

        DrawButton(
            renderer,
            m_quitButton,
            m_quitText,
            m_selectedOption == MenuOption::Quit
        );
    }

}