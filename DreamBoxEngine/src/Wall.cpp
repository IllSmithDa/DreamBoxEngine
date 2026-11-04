#include "DreamBox/Wall.h"

namespace DreamBox
{
    Wall::Wall(
        float x,
        float y,
        float width,
        float height
    )
        : GameObject(x, y, width, height)
    {
    }


    void Wall::Render(
        SDL_Renderer* renderer
    ) const
    {
        if (!renderer)
        {
            return;
        }

        SDL_SetRenderDrawColor(
            renderer,
            120,
            120,
            120,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &m_bounds
        );
    }
}