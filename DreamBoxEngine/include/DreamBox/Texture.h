
#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace DreamBox
{
    class Texture
    {
    public:
        Texture(
            SDL_Renderer* renderer,
            const std::string& filePath
        );

        ~Texture();

        // Prevent accidental copying of SDL resources.
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Render(
            SDL_Renderer* renderer,
            const SDL_FRect& destination
        ) const;

        // overload Render with version for sprite sheets
        void Render(
            SDL_Renderer* renderer,
            const SDL_FRect& source,
            const SDL_FRect& destination
        ) const;

        bool IsLoaded() const;

        float GetWidth() const;
        float GetHeight() const;

    private:
        SDL_Texture* m_texture = nullptr;

        float m_width = 0.0f;
        float m_height = 0.0f;
    };
}
