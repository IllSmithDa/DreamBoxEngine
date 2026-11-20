#include "DreamBox/GameScene.h"
#include "DreamBox/Collision.h"
#include "DreamBox/Pathfinder.h"
#include <SDL3/SDL_filesystem.h>
#include <string>
#include <cmath>
#include <algorithm>



namespace DreamBox
{
    GameScene::GameScene(SDL_Renderer* renderer)
        : m_player{
            300.0f,
            200.0f,
            100.0f,
            100.0f,
            200.0f
        }
    {

        if (!LoadLevel(1))
        {
            SDL_Log(
                "Unable to load Level 1. "
                "Using the default tilemap."
            );
        }


        m_tilemap.LoadTileset(renderer);
        const char* basePath = SDL_GetBasePath( );
        if (!basePath)
        {
            SDL_Log(
                "Unable to get application path: %s",
                SDL_GetError()
            );
            return;
        }

        std::string texturePath =
            std::string(basePath) +
            "assets/sprites/characters/player_sprites.png";
        
        m_playerTexture = std::make_unique<Texture>(
            renderer,
            texturePath
        );
    
        if (!m_playerTexture->IsLoaded())
        {
            SDL_Log("Failed to load player sprite sheet");
        }
    }


    void GameScene::CancelMovePath()
    {
        m_moveTarget.reset();
        m_waypoints.clear();
        m_nextWaypoint = 0;
    }


    SceneRequest GameScene::Update(
        const Input& input,
        float deltaTime
    )
    {
        // ----------------------------------
        // 1. Scene input
        // ----------------------------------

        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            return SceneRequest::MainMenu;
        }

        // ----------------------------------
        // Mouse hover position
        // ----------------------------------
            
        if (input.WasMouseMoved())
        {
            m_hoverPoint = SDL_FPoint{
                input.GetMouseX(),
                input.GetMouseY()
            };
        }

        // ----------------------------------
        // 2. Click-to-move destination
        // ----------------------------------

        
    // ----------------------------------
    // 2. Pointer selection and pathfinding
    // ----------------------------------
        
    if (input.WasPointPressed())
    {
        const SDL_FPoint point =
            input.GetPointPosition();
    
        // Interactable objects have first priority.
        const bool clickedObject =
            m_interactions.HandlePointerPress(
                point,
                m_player.GetCollisionBounds()
            );
        
        if (clickedObject)
        {
            // Object selection cancels existing navigation.
            // Automatic approach comes in Phase 11D.
            CancelMovePath();
        }
        else
        {
            // Generate a walkable route to the floor click.
            TrySetMovePath(point);
        }
    }


        // ----------------------------------
        // 3. Calculate player movement
        // ----------------------------------

        const SDL_FRect previousBounds =
            m_player.GetBounds();

        // Start with normal keyboard movement.
        SDL_FPoint movement =
            m_player.GetMovement(input, deltaTime);

        const bool keyboardMoving =
            movement.x != 0.0f ||
            movement.y != 0.0f;

        if (keyboardMoving)
        {
            // Manual controls always take priority.
            CancelMovePath();
            m_interactions.ClearSelection();
        }
        else if (m_moveTarget.has_value())
        {
            // Follow our A* waypoints instead.
            movement = CalculatePathMovement(deltaTime);
        }

        // ----------------------------------
        // 4. Player movement and collision
        // ----------------------------------

        // Horizontal movement.
        m_player.Move(movement.x, 0.0f);

        if (PlayerCollidesWithWall())
        {
            m_player.Move(-movement.x, 0.0f);
        }

        // Vertical movement.
        m_player.Move(0.0f, movement.y);

        if (PlayerCollidesWithWall())
        {
            m_player.Move(0.0f, -movement.y);
        }

        const SDL_FRect currentBounds =
            m_player.GetBounds();

        const float movementX =
            currentBounds.x - previousBounds.x;

        const float movementY =
            currentBounds.y - previousBounds.y;
        
        // ----------------------------------
        // 5. Check pathfinding progress
        // ----------------------------------

        if (m_moveTarget.has_value())
        {
            constexpr float movementEpsilon = 0.0001f;
            constexpr float arrivalTolerance = 1.0f;
        
            const bool movementRequested =
                std::fabs(movement.x) > movementEpsilon ||
                std::fabs(movement.y) > movementEpsilon;
        
            const bool actuallyMoved =
                std::fabs(movementX) > movementEpsilon ||
                std::fabs(movementY) > movementEpsilon;
        
            // If collision completely blocks movement,
            // cancel the path rather than retrying forever.
            if (movementRequested && !actuallyMoved)
            {
                SDL_Log("A*: movement blocked, cancelling path");
                CancelMovePath();
            }
            else if (m_nextWaypoint < m_waypoints.size())
            {
                const SDL_FRect hitbox =
                    m_player.GetCollisionBounds();
            
                const SDL_FPoint center{
                    hitbox.x + hitbox.w / 2.0f,
                    hitbox.y + hitbox.h / 2.0f
                };
            
                const SDL_FPoint& waypoint =
                    m_waypoints[m_nextWaypoint];
            
                const float dx = waypoint.x - center.x;
                const float dy = waypoint.y - center.y;
            
                const float distance =
                    std::sqrt(dx * dx + dy * dy);
            
                if (distance <= arrivalTolerance)
                {
                    ++m_nextWaypoint;
                
                    if (m_nextWaypoint >= m_waypoints.size())
                    {
                        CancelMovePath();
                    }
                }
            }
        }


        // ----------------------------------
        // 6. Player animation
        // ----------------------------------

        m_playerAnimator.Update(
            movementX,
            movementY,
            deltaTime
        );

        // ----------------------------------
        // 7. Level exit detection
        // ----------------------------------

        const auto exitBounds =
            m_tilemap.GetExitBounds();

        bool onExit = false;

        if (exitBounds.has_value())
        {
            onExit = Collision::Intersects(
                m_player.GetCollisionBounds(),
                exitBounds.value()
            );
        }

        if (onExit && !m_wasOnExit)
        {
            m_wasOnExit = true;

            const int nextLevel =
                (m_currentLevel == 1) ? 2 : 1;

            LoadLevel(nextLevel);
        }
        else if (!onExit)
        {
            m_wasOnExit = false;
        }

        // ----------------------------------
        // 8. Interactable objects
        // ----------------------------------

        m_interactions.Update(
            input.IsKeyPressed(SDL_SCANCODE_E),
            m_player.GetCollisionBounds()
        );

        return SceneRequest::None;
    }




    void GameScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        
        // tile map rendering
        m_tilemap.Render(renderer);

        m_interactions.Render(
            renderer,
            m_player.GetCollisionBounds(),
            m_hoverPoint
        );


        const SDL_FRect sourceFrame = m_playerAnimator.GetSourceRect();

        if (m_playerTexture &&
            m_playerTexture->IsLoaded())
        {
            m_playerTexture->Render(
                renderer,
                sourceFrame,
                m_player.GetBounds()
            );
        }
        else
        {
            m_player.Render(renderer);
        }


        if (m_showCollisionBounds)
        {
            // Outline the player's visual bounds.
            const SDL_FRect visualBounds =
                m_player.GetBounds();
        
            SDL_SetRenderDrawColor(
                renderer,
                255, 190, 50, 255
            );
        
            SDL_RenderRect(
                renderer,
                &visualBounds
            );
        
            // Outline the actual collision hitbox.
            const SDL_FRect collisionBounds =
                m_player.GetCollisionBounds();
        
            SDL_SetRenderDrawColor(
                renderer,
                50, 230, 150, 255
            );
        
            SDL_RenderRect(
                renderer,
                &collisionBounds
            );
        }


        // ----------------------------------
        // Click-to-move target marker
        // ----------------------------------

        if (m_moveTarget.has_value())
        {
            SDL_SetRenderDrawColor(
                renderer,
                50, 230, 150, 255
            );
        
            constexpr float markerSize = 6.0f;
        
            // Horizontal line.
            SDL_RenderLine(
                renderer,
                m_moveTarget->x - markerSize,
                m_moveTarget->y,
                m_moveTarget->x + markerSize,
                m_moveTarget->y
            );
        
            // Vertical line.
            SDL_RenderLine(
                renderer,
                m_moveTarget->x,
                m_moveTarget->y - markerSize,
                m_moveTarget->x,
                m_moveTarget->y + markerSize
            );
        }

    }
    
    bool GameScene::PlayerCollidesWithWall() const
    {
        const SDL_FRect hitbox =
            m_player.GetCollisionBounds();

        return m_tilemap.CollidesWithWall(hitbox) ||
               m_interactions.CollidesWithSolid(hitbox);
    }

    bool GameScene::LoadLevel(int level)
    {
        const char* basePath = SDL_GetBasePath();

        if (!basePath)
        {
            SDL_Log(
                "Cannot load level: application path unavailable."
            );
            return false;
        }

        // Supports level_01.txt through level_09.txt
        // for this first implementation.
        const std::string mapPath =
            std::string(basePath) +
            "assets/maps/level_0" +
            std::to_string(level) +
            ".txt";

        if (!m_tilemap.LoadFromFile(mapPath))
        {
            SDL_Log(
                "Failed to load level %d",
                level
            );
            return false;
        }

        // use interactions tilemaps door and door rendering
        m_interactions.Initialize(
            m_tilemap.GetChestBounds(),
            m_tilemap.GetDoorBounds()
        );

        // Move the player to the new level's spawn.
        const auto spawn = m_tilemap.GetPlayerSpawn();

        if (spawn.has_value())
        {
            const SDL_FRect hitbox =
                m_player.GetCollisionBounds();

            const float currentCenterX =
                hitbox.x + hitbox.w / 2.0f;

            const float currentCenterY =
                hitbox.y + hitbox.h / 2.0f;

            m_player.Move(
                spawn->x - currentCenterX,
                spawn->y - currentCenterY
            );

            SDL_Log(
                "Player spawned at %.1f, %.1f",
                spawn->x,
                spawn->y
            );
        }
        else
        {
            SDL_Log(
                "Level %d has no spawn marker; "
                "keeping current player position.",
                level
            );
        }

        m_currentLevel = level;

        // reset player pos
        CancelMovePath();
        m_hoverPoint.reset();

        SDL_Log(
            "Successfully loaded level %d",
                m_currentLevel
        );
        return true;
    }


    bool GameScene::TrySetMovePath(
        const SDL_FPoint& destination
    )
    {
        // A new click replaces the old route.
        CancelMovePath();

        const SDL_FRect playerHitbox =
            m_player.GetCollisionBounds();

        constexpr float tileSize =
            static_cast<float>(Tilemap::TileSize);

        constexpr float worldWidth =
            Tilemap::Columns * tileSize;

        constexpr float worldHeight =
            Tilemap::Rows * tileSize;

        // Can the player's entire collision hitbox
        // occupy a given world-space position?
        const auto canStandAt =
            [&](const SDL_FPoint& center) -> bool
        {
            const SDL_FRect candidate{
                center.x - playerHitbox.w / 2.0f,
                center.y - playerHitbox.h / 2.0f,
                playerHitbox.w,
                playerHitbox.h
            };

            const bool insideWorld =
                candidate.x >= 0.0f &&
                candidate.y >= 0.0f &&
                candidate.x + candidate.w <= worldWidth &&
                candidate.y + candidate.h <= worldHeight;

            if (!insideWorld)
            {
                return false;
            }

            return
                !m_tilemap.CollidesWithWall(candidate) &&
                !m_interactions.CollidesWithSolid(candidate);
        };

        // Reject destinations where the player cannot fit.
        if (!canStandAt(destination))
        {
            SDL_Log("A*: destination is blocked");
            return false;
        }

        // Current position based on collision hitbox.
        const SDL_FPoint playerCenter{
            playerHitbox.x + playerHitbox.w / 2.0f,
            playerHitbox.y + playerHitbox.h / 2.0f
        };

        // Convert world coordinates into grid coordinates.
        const GridPoint start{
            static_cast<int>(playerCenter.x / tileSize),
            static_cast<int>(playerCenter.y / tileSize)
        };

        const GridPoint goal{
            static_cast<int>(destination.x / tileSize),
            static_cast<int>(destination.y / tileSize)
        };

        // Convert a grid position back to a tile center.
        const auto tileCenter =
            [&](const GridPoint& cell) -> SDL_FPoint
        {
            return SDL_FPoint{
                (static_cast<float>(cell.x) + 0.5f) *
                    tileSize,
                (static_cast<float>(cell.y) + 0.5f) *
                    tileSize
            };
        };

        // A* asks whether each grid cell is walkable.
        const auto isWalkable =
            [&](int column, int row) -> bool
        {
            // The player already occupies the start cell.
            // Its exact position may not be tile-centered.
            if (column == start.x && row == start.y)
            {
                return true;
            }

            return canStandAt(
                tileCenter(GridPoint{column, row})
            );
        };

        const std::vector<GridPoint> path =
            Pathfinder::FindPath(
                start,
                goal,
                Tilemap::Columns,
                Tilemap::Rows,
                isWalkable
            );

        if (path.empty())
        {
            SDL_Log("A*: no reachable path found");
            return false;
        }

        // Convert the A* grid route into world positions.
        m_waypoints.reserve(path.size() + 1);

        // First align with the starting tile center
        // when that position is physically walkable.
        const SDL_FPoint startCenter =
            tileCenter(start);

        if (canStandAt(startCenter))
        {
            m_waypoints.push_back(startCenter);
        }

        // Skip grid index 0 because it is the start cell.
        for (std::size_t i = 1; i < path.size(); ++i)
        {
            m_waypoints.push_back(
                tileCenter(path[i])
            );
        }

        // Finish at the exact clicked position,
        // not just the center of the destination tile.
        m_waypoints.push_back(destination);

        m_nextWaypoint = 0;
        m_moveTarget = destination;

        SDL_Log(
            "A*: route created with %zu grid nodes "
            "and %zu waypoints",
            path.size(),
            m_waypoints.size()
        );

        return true;
    }


    SDL_FPoint GameScene::CalculatePathMovement(
        float deltaTime
    )
    {
        if (!m_moveTarget.has_value())
        {
            return SDL_FPoint{0.0f, 0.0f};
        }

        constexpr float moveSpeed = 200.0f;
        constexpr float arrivalTolerance = 1.0f;

        const SDL_FRect hitbox =
            m_player.GetCollisionBounds();

        const SDL_FPoint playerCenter{
            hitbox.x + hitbox.w / 2.0f,
            hitbox.y + hitbox.h / 2.0f
        };

        while (m_nextWaypoint < m_waypoints.size())
        {
            const SDL_FPoint& waypoint =
                m_waypoints[m_nextWaypoint];

            const float dx =
                waypoint.x - playerCenter.x;

            const float dy =
                waypoint.y - playerCenter.y;

            const float distance =
                std::sqrt(dx * dx + dy * dy);

            // Skip waypoints we've already reached.
            if (distance <= arrivalTolerance)
            {
                ++m_nextWaypoint;
                continue;
            }

            const float step =
                std::min(
                    moveSpeed * std::max(deltaTime, 0.0f),
                    distance
                );

            return SDL_FPoint{
                (dx / distance) * step,
                (dy / distance) * step
            };
        }

        // All waypoints completed.
        CancelMovePath();

        return SDL_FPoint{0.0f, 0.0f};
    }

}