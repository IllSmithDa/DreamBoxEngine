
#pragma once

#include "DreamBox/Scene.h"
#include "DreamBox/Text.h"

#include <memory>

namespace DreamBox
{
    class SettingsScene : public Scene
    {
    public:
        explicit SettingsScene(
            SDL_Renderer* renderer
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
        std::unique_ptr<Text> m_titleText;

        std::unique_ptr<Text> m_fullscreenText;
        std::unique_ptr<Text> m_vsyncText;

        std::unique_ptr<Text> m_backText;
        std::unique_ptr<Text> m_footerText;

        SDL_FRect m_backButton{
            490.0f,
            440.0f,
            300.0f,
            60.0f
        };
    };
}
