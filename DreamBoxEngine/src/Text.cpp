
#include "DreamBox/Text.h"

Text::Text(
    SDL_Renderer* renderer,
    const std::string& fontPath,
    float fontSize,
    const std::string& content,
    SDL_Color color
)
{
    if (!renderer)
    {
        SDL_Log("Text: Invalid renderer");
        return;
    }

    m_font = TTF_OpenFont(
        fontPath.c_str(),
        fontSize
    );

    if (!m_font)
    {
        SDL_Log(
            "Failed to load font: %s",
            SDL_GetError()
        );
        return;
    }

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            m_font,
            content.c_str(),
            0,
            color
        );

    if (!surface)
    {
        SDL_Log(
            "Failed to render text: %s",
            SDL_GetError()
        );
        return;
    }

    m_texture = SDL_CreateTextureFromSurface(
        renderer,
        surface
    );

    SDL_DestroySurface(surface);

    if (!m_texture)
    {
        SDL_Log(
            "Failed to create text texture: %s",
            SDL_GetError()
        );
        return;
    }

    SDL_GetTextureSize(
        m_texture,
        &m_width,
        &m_height
    );
}

Text::~Text()
{
    SDL_DestroyTexture(m_texture);

    if (m_font)
    {
        TTF_CloseFont(m_font);
    }
}

void Text::Render(
    SDL_Renderer* renderer,
    float x,
    float y
) const
{
    if (!renderer || !m_texture)
    {
        return;
    }

    SDL_FRect destination{
        x,
        y,
        m_width,
        m_height
    };

    SDL_RenderTexture(
        renderer,
        m_texture,
        nullptr,
        &destination
    );
}

void Text::RenderCentered(
    SDL_Renderer* renderer,
    float centerX,
    float y
) const
{
    Render(
        renderer,
        centerX - m_width / 2.0f,
        y
    );
}

float Text::GetWidth() const
{
    return m_width;
}

float Text::GetHeight() const
{
    return m_height;
}

SDL_Texture* Text::GetTexture() const
{
    return m_texture;
}
