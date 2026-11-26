#pragma once

#include "DreamBox/GroundPickup.h"
#include "DreamBox/Texture.h"

#include <SDL3/SDL_render.h>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace DreamBox
{
    // Visuals only. PickupSystem owns collection and inventory changes.
    class GoldCoinRenderer
    {
    public:
        GoldCoinRenderer(
            SDL_Renderer* renderer,
            const std::string& spriteSheetPath
        );

        void Update(float deltaTime);

        void Render(
            SDL_Renderer* renderer,
            const std::vector<GroundPickup>& pickups
        ) const;

    private:
        int FrameForPickup(std::size_t pickupIndex) const;

        std::unique_ptr<Texture> m_texture;
        float m_animationTime = 0.0f;

        static constexpr float CycleSeconds = 2.4f;
        static constexpr float FrameSize = 32.0f;
    };
}
