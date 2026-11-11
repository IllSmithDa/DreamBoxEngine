
#include "DreamBox/Button.h"

namespace DreamBox
{
    Button::Button(
        SDL_Renderer* renderer,
        const SDL_FRect& bounds,
        const std::string& fontPath,
        float fontSize,
        const std::string& label
    )
        : m_bounds(bounds)
    {
        m_label = std::make_unique<Text>(
            renderer,
            fontPath,
            fontSize,
            label
        );
    }

    void Button::Render(
        SDL_Renderer* renderer,
        bool selected
    ) const
    {
        if (!renderer)
        {
            return;
        }

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
            &m_bounds
        );

        if (selected)
        {
            SDL_SetRenderDrawColor(
                renderer,
                230, 235, 245, 255
            );

            SDL_RenderRect(
                renderer,
                &m_bounds
            );
        }

        if (!m_label)
        {
            return;
        }

        const float centerX =
            m_bounds.x + m_bounds.w / 2.0f;

        const float textY =
            m_bounds.y +
            (m_bounds.h - m_label->GetHeight()) / 2.0f;

        m_label->RenderCentered(
            renderer,
            centerX,
            textY
        );
    }

    const SDL_FRect& Button::GetBounds() const
    {
        return m_bounds;
    }

    bool Button::Contains(float x, float y) const
    {
        return x >= m_bounds.x &&
               x < m_bounds.x + m_bounds.w &&
               y >= m_bounds.y &&
               y < m_bounds.y + m_bounds.h;
    }
}
