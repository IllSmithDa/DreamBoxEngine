#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <string>
#include "DreamBox/Tilemap.h"
#include "DreamBox/Collision.h"

namespace DreamBox
{
    Tilemap::Tilemap()
    {
      
        for (int row = 0; row < Rows; ++row)
        {
            for (int col = 0; col < Columns; ++col)
            {
                const bool outerWall =
                    row == 0 ||
                    row == Rows - 1 ||
                    col == 0 ||
                    col == Columns - 1;

                const bool centerWall =
                    col >= 15 &&
                    col <= 16 &&
                    row >= 6 &&
                    row <= 12;

                m_tiles[row * Columns + col] =
                    (outerWall || centerWall)
                        ? Tile::Wall
                        : Tile::Floor;
            }
        }
    }

    SDL_FRect Tilemap::GetTileBounds(
        int column,
        int row
    ) const
    {
        return SDL_FRect{
            column * TileSize,
            row * TileSize,
            TileSize,
            TileSize
        };
    }


    void Tilemap::Render(SDL_Renderer* renderer) const
    {
        if (!renderer)
        {
            return;
        }
      
        constexpr float sourceTileSize = 16.0f;
      
        for (int row = 0; row < Rows; ++row)
        {
            for (int col = 0; col < Columns; ++col)
            {
                const Tile tile =
                    m_tiles[row * Columns + col];
            
                const SDL_FRect destination =
                    GetTileBounds(col, row);
            
                // Draw textured tiles when available.
                if (m_tileset && m_tileset->IsLoaded())
                {
                    const SDL_FRect source{
                        tile == Tile::Floor
                            ? 0.0f
                            : sourceTileSize,
                        0.0f,
                        sourceTileSize,
                        sourceTileSize
                    };
                  
                    m_tileset->Render(
                        renderer,
                        source,
                        destination
                    );
                  
                    continue;
                }
              
                // Fallback: colored rectangles.
                if (tile == Tile::Wall)
                {
                    SDL_SetRenderDrawColor(
                        renderer, 90, 110, 135, 255
                    );
                }
                else if ((row + col) % 2 == 0)
                {
                    SDL_SetRenderDrawColor(
                        renderer, 36, 44, 56, 255
                    );
                }
                else
                {
                    SDL_SetRenderDrawColor(
                        renderer, 42, 50, 62, 255
                    );
                }
              
                SDL_RenderFillRect(
                    renderer,
                    &destination
                );
            }
        }
    }


    bool Tilemap::CollidesWithWall(
        const SDL_FRect& hitbox
    ) const
    {
        for (int row = 0; row < Rows; ++row)
        {
            for (int col = 0; col < Columns; ++col)
            {
                if (m_tiles[row * Columns + col]
                    != Tile::Wall)
                {
                    continue;
                }

                const SDL_FRect bounds =
                    GetTileBounds(col, row);

                if (Collision::Intersects(
                    hitbox,
                    bounds
                ))
                {
                    return true;
                }
            }
        }

        return false;
    }


    void Tilemap::LoadTileset(SDL_Renderer* renderer)
    {
        if (!renderer)
        {
            SDL_Log("Cannot load tileset: renderer is null");
            return;
        }
      
        const char* basePath = SDL_GetBasePath();
      
        if (!basePath)
        {
            SDL_Log("Cannot determine application path");
            return;
        }
      
        const std::string texturePath =
            std::string(basePath) +
            "assets/tiles/dreambox_terrain_tiles.png";
      
        m_tileset = std::make_unique<Texture>(
            renderer,
            texturePath
        );
      
        if (!m_tileset->IsLoaded())
        {
            SDL_Log("Failed to load tilemap texture");
            m_tileset.reset();
        }
        else
        {
            SDL_Log("Tilemap texture loaded successfully");
        }
    }

}
