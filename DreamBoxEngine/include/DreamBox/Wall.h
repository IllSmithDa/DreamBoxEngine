#pragma once

#include "DreamBox/GameObject.h"

namespace DreamBox
{
    class Wall : public GameObject
    {
    public:
        Wall(
            float x,
            float y,
            float width,
            float height
        );

        void Render(
            SDL_Renderer* renderer
        ) const override;
    };
}