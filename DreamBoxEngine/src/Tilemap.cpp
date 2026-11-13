
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

    void Tilemap::Render(
        SDL_Renderer* renderer
    ) const
    {
        if (!renderer)
        {
            return;
        }

        for (int row = 0; row < Rows; ++row)
        {
            for (int col = 0; col < Columns; ++col)
            {
                const Tile tile =
                    m_tiles[row * Columns + col];

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

                const SDL_FRect bounds =
                    GetTileBounds(col, row);

                SDL_RenderFillRect(
                    renderer,
                    &bounds
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
}
