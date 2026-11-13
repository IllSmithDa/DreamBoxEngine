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
            255,
            50,
            50,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &m_bounds
        );
    }
    
    SDL_FRect Player::GetCollisionBounds() const
    {
        // Full player bounds used for rendering.
        const SDL_FRect visual = GetBounds();
    
        // Collision dimensions.
        constexpr float hitboxWidth = 48.0f;
        constexpr float hitboxHeight = 32.0f;
    
        // Distance above the sprite's bottom edge.
        constexpr float bottomOffset = 8.0f;
    
        SDL_FRect collision{
            visual.x + (visual.w - hitboxWidth) / 2.0f,
            visual.y + visual.h - hitboxHeight - bottomOffset,
            hitboxWidth,
            hitboxHeight
        };
    
        return collision;
    }

}