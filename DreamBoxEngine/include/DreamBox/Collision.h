#pragma once

#include "DreamBox/GameObject.h"

namespace DreamBox
{
    class Collision
    {
    public:
        static bool Intersects(
            const GameObject& first,
            const GameObject& second
        );
        static bool Intersects(
            const SDL_FRect& first,
            const SDL_FRect& second
        );
    };
}