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
}