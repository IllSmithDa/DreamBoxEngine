
#pragma once

#include <SDL3/SDL_rect.h>

namespace DreamBox
{
    class SpriteAnimator
    {
    public:
        enum class Facing
        {
            Down = 0,
            Up = 1,
            Right = 2,
            Left = 3
        };

        SpriteAnimator(
            float frameWidth,
            float frameHeight,
            int frameCount,
            float frameDuration
        );

        void Update(
            float movementX,
            float movementY,
            float deltaTime
        );

        SDL_FRect GetSourceRect() const;

        Facing GetFacing() const;
        int GetCurrentFrame() const;

    private:
        Facing m_facing = Facing::Down;

        int m_currentFrame = 0;
        float m_animationTimer = 0.0f;

        float m_frameWidth;
        float m_frameHeight;

        int m_frameCount;
        float m_frameDuration;
    };
}
