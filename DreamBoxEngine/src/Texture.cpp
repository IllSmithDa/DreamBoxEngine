
#include "DreamBox/Texture.h"

namespace DreamBox
{
    Texture::Texture(
        SDL_Renderer* renderer,
        const std::string& filePath
    )
    {
        if (!renderer)
        {
            SDL_Log("Texture: Invalid renderer");
            return;
        }

        // Load PNG image from disk.
        SDL_Surface* surface =
            SDL_LoadPNG(filePath.c_str());

        if (!surface)
        {
            SDL_Log(
                "Failed to load PNG '%s': %s",
                filePath.c_str(),
                SDL_GetError()
            );

            return;
        }

        // Convert the image into an SDL texture.
        m_texture = SDL_CreateTextureFromSurface(
            renderer,
            surface
        );

        //sharpen pixel art rendering

        if (!SDL_SetTextureScaleMode(
            m_texture,
            SDL_SCALEMODE_NEAREST
        ))
        {
            SDL_Log(
                "Failed to configure texture scaling: %s",
                SDL_GetError()
            );
        }

        // The surface is no longer needed.
        SDL_DestroySurface(surface);

        if (!m_texture)
        {
            SDL_Log(
                "Failed to create texture: %s",
                SDL_GetError()
            );

            return;
        }

        // Cache texture dimensions.
        if (!SDL_GetTextureSize(
            m_texture,
            &m_width,
            &m_height
        ))
        {
            SDL_Log(
                "Failed to get texture size: %s",
                SDL_GetError()
            );

            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }

    Texture::~Texture()
    {
        SDL_DestroyTexture(m_texture);
    }

    void Texture::Render(
        SDL_Renderer* renderer,
        const SDL_FRect& destination
    ) const
    {
        if (!renderer || !m_texture)
        {
            return;
        }

        SDL_RenderTexture(
            renderer,
            m_texture,
            nullptr,
            &destination
        );
    }

    void Texture::Render(
        SDL_Renderer* renderer,
        const SDL_FRect& source,
        const SDL_FRect& destination
    ) const
    {
        if (!renderer || !m_texture)
        {
            return;
        }
      
        SDL_RenderTexture(
            renderer,
            m_texture,
            &source,
            &destination
        );
    }

    bool Texture::IsLoaded() const
    {
        return m_texture != nullptr;
    }

    float Texture::GetWidth() const
    {
        return m_width;
    }

    float Texture::GetHeight() const
    {
        return m_height;
    }
}
