#include "DreamBox/Player.h"
#include <cmath>

namespace DreamBox
{
    Player::Player(
        float x,
        float y,
        float width,
        float height,
        float speed
    )
        : GameObject(x, y, width, height),
        m_speed(speed)
    {
    }


    void Player::Update(
        const Input& input,
        float deltaTime
    )
    {
        // Vertical movement
        if (input.IsKeyDown(SDL_SCANCODE_W) ||
            input.IsKeyDown(SDL_SCANCODE_UP))
        {
            m_bounds.y -= m_speed * deltaTime;
        }

        if (input.IsKeyDown(SDL_SCANCODE_S) ||
            input.IsKeyDown(SDL_SCANCODE_DOWN))
        {
            m_bounds.y += m_speed * deltaTime;
        }

        // Horizontal movement
        if (input.IsKeyDown(SDL_SCANCODE_A) ||
            input.IsKeyDown(SDL_SCANCODE_LEFT))
        {
            m_bounds.x -= m_speed * deltaTime;
        }

        if (input.IsKeyDown(SDL_SCANCODE_D) ||
            input.IsKeyDown(SDL_SCANCODE_RIGHT))
        {
            m_bounds.x += m_speed * deltaTime;
        }
    }

    SDL_FPoint Player::GetMovement(
        const Input& input,
        float deltaTime
    ) const
    {
        SDL_FPoint direction{
            0.0f,
            0.0f
        };

        // Determine movement direction.
        if (input.IsKeyDown(SDL_SCANCODE_W) ||
            input.IsKeyDown(SDL_SCANCODE_UP))
        {
            direction.y -= 1.0f;
        }

        if (input.IsKeyDown(SDL_SCANCODE_S) ||
            input.IsKeyDown(SDL_SCANCODE_DOWN))
        {
            direction.y += 1.0f;
        }

        if (input.IsKeyDown(SDL_SCANCODE_A) ||
            input.IsKeyDown(SDL_SCANCODE_LEFT))
        {
            direction.x -= 1.0f;
        }

        if (input.IsKeyDown(SDL_SCANCODE_D) ||
            input.IsKeyDown(SDL_SCANCODE_RIGHT))
        {
            direction.x += 1.0f;
        }

        // Normalize diagonal movement so it has the same
        // overall speed as horizontal or vertical movement.
        const float length = std::sqrt(
            direction.x * direction.x +
            direction.y * direction.y
        );

        if (length > 0.0f)
        {
            direction.x /= length;
            direction.y /= length;
        }

        const float distance =
            m_speed * deltaTime;

        return SDL_FPoint{
            direction.x * distance,
            direction.y * distance
        };
    }


    void Player::Render(SDL_Renderer* renderer) const
    {
        if (!renderer)
        {
            return;
        }

        SDL_SetRenderDrawColor(
            renderer,
            80,
            180,
            255,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &m_bounds
        );
    }
}