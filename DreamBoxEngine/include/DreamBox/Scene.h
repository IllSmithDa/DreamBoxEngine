#pragma once

#include <SDL3/SDL.h>

#include "DreamBox/Input.h"

namespace DreamBox
{
    enum class SceneRequest
    {
        None,
        StartGame,
        Quit
    };

    class Scene
    {
    public:
        virtual ~Scene() = default;

        virtual SceneRequest Update(
            const Input& input,
            float deltaTime
        ) = 0;

        virtual void Render(
            SDL_Renderer* renderer
        ) const = 0;
    };
}