#include "DreamBox/GoldCoinRenderer.h"

#include <SDL3/SDL_log.h>

#include <cmath>

namespace DreamBox
{
    GoldCoinRenderer::GoldCoinRenderer(
        SDL_Renderer* renderer,
        const std::string& spriteSheetPath
    )
    {
        if (renderer)
        {
            m_texture = std::make_unique<Texture>(
                renderer,
                spriteSheetPath
            );
        }

        if (!m_texture || !m_texture->IsLoaded())
        {
            SDL_Log("GoldCoinRenderer: sprite sheet unavailable; using fallback rectangles");
            m_texture.reset();
        }
        else
        {
            SDL_Log(
                "GoldCoinRenderer: sprite sheet loaded: %s",
                spriteSheetPath.c_str()
            );
        }

    }

    void GoldCoinRenderer::Update(float deltaTime)
    {
        if (deltaTime <= 0.0f)
        {
            return;
        }

        m_animationTime = std::fmod(
            m_animationTime + deltaTime,
            CycleSeconds
        );
    }

    int GoldCoinRenderer::FrameForPickup(
        std::size_t pickupIndex
    ) const
    {
        // Offset each pile so they do not all sparkle together.
        const float phase = std::fmod(
            m_animationTime +
                static_cast<float>(pickupIndex) * 0.43f,
            CycleSeconds
        );

        if (phase >= 2.29f)
        {
            return 3;
        }
        if (phase >= 2.17f)
        {
            return 2;
        }
        if (phase >= 2.05f)
        {
            return 1;
        }
        return 0;
    }

    void GoldCoinRenderer::Render(
        SDL_Renderer* renderer,
        const std::vector<GroundPickup>& pickups
    ) const
    {
        if (!renderer)
        {
            return;
        }

        for (std::size_t i = 0; i < pickups.size(); ++i)
        {
            const GroundPickup& pickup = pickups[i];

            if (pickup.collected ||
                pickup.loot.id != ItemId::GoldCoin)
            {
                continue;
            }

            if (m_texture && m_texture->IsLoaded())
            {
                const SDL_FRect source{
                    static_cast<float>(FrameForPickup(i)) * FrameSize,
                    0.0f,
                    FrameSize,
                    FrameSize
                };

                m_texture->Render(
                    renderer,
                    source,
                    pickup.bounds
                );
            }
            else
            {
                // Preserve the existing fallback if the PNG is missing.
                SDL_SetRenderDrawColor(renderer, 235, 180, 45, 255);
                SDL_RenderFillRect(renderer, &pickup.bounds);
                SDL_SetRenderDrawColor(renderer, 255, 230, 120, 255);
                SDL_RenderRect(renderer, &pickup.bounds);


                // ----------------------------------
                // Additional visible sparkle effect
                // ----------------------------------

                const float phase = std::fmod(
                    m_animationTime +
                        static_cast<float>(i) * 0.43f,
                    CycleSeconds
                );

                // Display a glint for 0.4 seconds
                // during each animation cycle.
                if (phase >= 1.95f && phase < 2.35f)
                {
                    const float centerX =
                        pickup.bounds.x + pickup.bounds.w * 0.78f;
                
                    const float centerY =
                        pickup.bounds.y + pickup.bounds.h * 0.25f;
                
                    const float radius =
                        (phase < 2.1f) ? 4.0f : 7.0f;
                
                    // Bright central sparkle
                    SDL_SetRenderDrawColor(
                        renderer, 255, 255, 235, 255
                    );
                
                    SDL_RenderLine(
                        renderer,
                        centerX - radius, centerY,
                        centerX + radius, centerY
                    );
                
                    SDL_RenderLine(
                        renderer,
                        centerX, centerY - radius,
                        centerX, centerY + radius
                    );
                
                    // Smaller golden diagonal rays
                    SDL_SetRenderDrawColor(
                        renderer, 255, 215, 90, 255
                    );
                
                    const float diagonal = radius * 0.45f;
                
                    SDL_RenderLine(
                        renderer,
                        centerX - diagonal,
                        centerY - diagonal,
                        centerX + diagonal,
                        centerY + diagonal
                    );
                
                    SDL_RenderLine(
                        renderer,
                        centerX - diagonal,
                        centerY + diagonal,
                        centerX + diagonal,
                        centerY - diagonal
                    );
                }

            }
        }
    }
}
