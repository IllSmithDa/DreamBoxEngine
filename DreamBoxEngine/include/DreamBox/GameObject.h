#pragma once

#include <SDL3/SDL.h>

namespace DreamBox
{
    class GameObject
    {
    public:
        GameObject(
            float x,
            float y,
            float width,
            float height
        );

        virtual ~GameObject() = default;

        virtual void Update(float deltaTime);
        virtual void Render(SDL_Renderer* renderer) const;

        const SDL_FRect& GetBounds() const;

        void SetPosition(float x, float y);
        void Move(float x, float y);

    protected:
        SDL_FRect m_bounds;
    };
}