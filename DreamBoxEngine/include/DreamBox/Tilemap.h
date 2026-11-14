
#pragma once

#include <SDL3/SDL_render.h>
#include "DreamBox/Texture.h"
#include <memory>
#include <array>
#include <cstdint>

namespace DreamBox
{
    class Tilemap
    {
    public:
        static constexpr int Columns = 32;
        static constexpr int Rows = 18;
        static constexpr float TileSize = 40.0f;

        void LoadTileset(SDL_Renderer* renderer);

        Tilemap();

        void Render(SDL_Renderer* renderer) const;

        bool CollidesWithWall(
            const SDL_FRect& hitbox
        ) const;

    private:
        enum class Tile : std::uint8_t
        {
            Floor,
            Wall
        };

        std::array<Tile, Columns * Rows> m_tiles{};

        SDL_FRect GetTileBounds(
            int column,
            int row
        ) const;

        std::unique_ptr<Texture> m_tileset;
        
    };
}
