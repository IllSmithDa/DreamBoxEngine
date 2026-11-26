
#pragma once

#include "DreamBox/Input.h"
#include "DreamBox/PauseMenuModel.h"
#include "DreamBox/Text.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <memory>
#include <string>

namespace DreamBox
{
    enum class PauseMenuAction
    {
        None,
        Resume,
        MainMenu
    };

    class PauseMenu
    {
    public:
        PauseMenu(
            SDL_Renderer* renderer,
            const std::string& fontPath
        );

        void Reset();

        PauseMenuAction Update(const Input& input);

        void Render(SDL_Renderer* renderer) const;

    private:
        PauseMenuModel m_model;

        std::unique_ptr<Text> m_titleText;
        std::unique_ptr<Text> m_resumeText;
        std::unique_ptr<Text> m_mainMenuText;
        std::unique_ptr<Text> m_footerText;

        SDL_FRect m_resumeButton{
            490.0f, 322.0f, 300.0f, 64.0f
        };

        SDL_FRect m_mainMenuButton{
            490.0f, 404.0f, 300.0f, 64.0f
        };

        void DrawButton(
            SDL_Renderer* renderer,
            const SDL_FRect& bounds,
            const Text* label,
            bool selected
        ) const;
    };
}
