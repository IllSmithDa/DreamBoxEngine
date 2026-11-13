#include "DreamBox/Collision.h"

namespace DreamBox
{
    bool Collision::Intersects(
        const GameObject& first,
        const GameObject& second
    )
    {
        return SDL_HasRectIntersectionFloat(
            &first.GetBounds(),
            &second.GetBounds()
        );
    }

    bool Collision::Intersects(
        const SDL_FRect& first,
        const SDL_FRect& second
    )
    {
        return SDL_HasRectIntersectionFloat(
            &first,
            &second
        );
    }
}