
#include "DreamBox/SpriteAnimator.h"

#include <cmath>

namespace DreamBox
{
    SpriteAnimator::SpriteAnimator(
        float frameWidth,
        float frameHeight,
        int frameCount,
        float frameDuration
    )
        : m_frameWidth(frameWidth),
          m_frameHeight(frameHeight),
          m_frameCount(frameCount > 0 ? frameCount : 1),
          m_frameDuration(
              frameDuration > 0.0f
                  ? frameDuration
                  : 0.15f
          )
    {
    }

    void SpriteAnimator::Update(
        float movementX,
        float movementY,
        float deltaTime
    )
    {
        constexpr float epsilon = 0.0001f;

        const bool moving =
            std::fabs(movementX) > epsilon ||
            std::fabs(movementY) > epsilon;

        // Stationary: return to idle frame.
        if (!moving)
        {
            m_currentFrame = 0;
            m_animationTimer = 0.0f;
            return;
        }

        Facing newFacing = m_facing;

        // Determine direction of movement.
        if (std::fabs(movementX) >
            std::fabs(movementY))
        {
            newFacing = movementX > 0.0f
                ? Facing::Right
                : Facing::Left;
        }
        else
        {
            newFacing = movementY > 0.0f
                ? Facing::Down
                : Facing::Up;
        }

        // Reset animation when direction changes.
        if (newFacing != m_facing)
        {
            m_facing = newFacing;
            m_currentFrame = 0;
            m_animationTimer = 0.0f;
        }

        // Advance animation using delta time.
        if (deltaTime <= 0.0f)
        {
            return;
        }

        m_animationTimer += deltaTime;

        while (m_animationTimer >= m_frameDuration)
        {
            m_animationTimer -= m_frameDuration;

            m_currentFrame =
                (m_currentFrame + 1) % m_frameCount;
        }
    }

    SDL_FRect SpriteAnimator::GetSourceRect() const
    {
        return SDL_FRect{
            static_cast<float>(m_currentFrame) *
                m_frameWidth,

            static_cast<float>(m_facing) *
                m_frameHeight,

            m_frameWidth,
            m_frameHeight
        };
    }

    SpriteAnimator::Facing
    SpriteAnimator::GetFacing() const
    {
        return m_facing;
    }

    int SpriteAnimator::GetCurrentFrame() const
    {
        return m_currentFrame;
    }
}
