
#pragma once

#include <SDL3/SDL_render.h>
#include <optional>
#include <SDL3/SDL_rect.h>
#include <memory>
#include <array>
#include <cstdint>
#include <vector>
#include "DreamBox/Texture.h"

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

        bool LoadFromFile(const std::string& filePath);

        std::optional<SDL_FPoint> GetPlayerSpawn() const
        {
            return m_playerSpawn;
        }

        std::optional<SDL_FRect> GetExitBounds() const
        {
            return m_exitBounds;
        }
        const std::vector<SDL_FRect>& GetChestBounds() const
        {
            return m_chestBounds;
        }
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
        std::optional<SDL_FPoint> m_playerSpawn;
        std::optional<SDL_FRect> m_exitBounds;
        std::vector<SDL_FRect> m_chestBounds;
    };
}
