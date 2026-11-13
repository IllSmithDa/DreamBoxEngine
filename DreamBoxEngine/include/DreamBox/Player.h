#pragma once

#include <SDL3/SDL.h>

#include "DreamBox/GameObject.h"
#include "DreamBox/Input.h"

namespace DreamBox
{
    class Player : public GameObject
    {
    public:
        Player(
            float x,
            float y,
            float width,
            float height,
            float speed
        );

        void Update(
            const Input& input,
            float deltaTime
        );

        void Render(
            SDL_Renderer* renderer
        ) const override;

        SDL_FPoint GetMovement(
            const Input& input,
            float deltaTime
        ) const;
        SDL_FRect GetCollisionBounds() const;
    private:
        float m_speed;
    };
}