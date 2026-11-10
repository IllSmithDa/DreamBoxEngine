
#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

class Text
{
public:
    Text(
        SDL_Renderer* renderer,
        const std::string& fontPath,
        float fontSize,
        const std::string& content,
        SDL_Color color = {255, 255, 255, 255}
    );

    ~Text();

    // Prevent accidental copying of SDL resources.
    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;

    void Render(
        SDL_Renderer* renderer,
        float x,
        float y
    ) const;

    void RenderCentered(
        SDL_Renderer* renderer,
        float centerX,
        float y
    ) const;

    float GetWidth() const;
    float GetHeight() const;

    SDL_Texture* GetTexture() const;

private:
    TTF_Font* m_font = nullptr;
    SDL_Texture* m_texture = nullptr;

    float m_width = 0.0f;
    float m_height = 0.0f;
};
