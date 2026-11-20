#include "DreamBox/GameScene.h"
#include "DreamBox/Collision.h"
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

        if (input.WasPointPressed())
        {
            const SDL_FPoint point =
                input.GetPointPosition();
        
            // Give interactable objects first priority.
            const bool clickedObject =
                m_interactions.HandlePointerPress(
                    point,
                    m_player.GetCollisionBounds()
                );
            
            if (clickedObject)
            {
                // Stop any previous click-to-move action.
                m_moveTarget.reset();
            }
            else
            {
                // No object was selected.
                // Treat the click as a movement destination.
            
                SDL_FRect targetHitbox =
                    m_player.GetCollisionBounds();
            
                targetHitbox.x =
                    point.x - targetHitbox.w / 2.0f;
            
                targetHitbox.y =
                    point.y - targetHitbox.h / 2.0f;
            
                constexpr float worldWidth =
                    Tilemap::Columns * Tilemap::TileSize;
            
                constexpr float worldHeight =
                    Tilemap::Rows * Tilemap::TileSize;
            
                const bool insideWorld =
                    targetHitbox.x >= 0.0f &&
                    targetHitbox.y >= 0.0f &&
                    targetHitbox.x + targetHitbox.w <= worldWidth &&
                    targetHitbox.y + targetHitbox.h <= worldHeight;
            
                const bool blocked =
                    m_tilemap.CollidesWithWall(targetHitbox) ||
                    m_interactions.CollidesWithSolid(targetHitbox);
            
                if (insideWorld && !blocked)
                {
                    m_moveTarget = point;
                }
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
            m_moveTarget.reset();
            m_interactions.ClearSelection();
        }
        else if (m_moveTarget.has_value())
        {
            const SDL_FRect hitbox =
                m_player.GetCollisionBounds();

            const float centerX =
                hitbox.x + hitbox.w / 2.0f;

            const float centerY =
                hitbox.y + hitbox.h / 2.0f;

            const float dx =
                m_moveTarget->x - centerX;

            const float dy =
                m_moveTarget->y - centerY;

            const float distance =
                std::sqrt(dx * dx + dy * dy);

            constexpr float arrivalTolerance = 1.0f;

            if (distance <= arrivalTolerance)
            {
                m_moveTarget.reset();
            }
            else
            {
                // Match the existing player movement speed.
                constexpr float moveSpeed = 200.0f;

                const float step =
                    std::min(
                        moveSpeed * std::max(deltaTime, 0.0f),
                        distance
                    );

                // Normalize direction and apply movement.
                movement.x = (dx / distance) * step;
                movement.y = (dy / distance) * step;
            }
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
        // 5. Check click-to-move progress
        // ----------------------------------

        if (m_moveTarget.has_value())
        {
            const SDL_FRect hitbox =
                m_player.GetCollisionBounds();

            const float centerX =
                hitbox.x + hitbox.w / 2.0f;

            const float centerY =
                hitbox.y + hitbox.h / 2.0f;

            const float dx =
                m_moveTarget->x - centerX;

            const float dy =
                m_moveTarget->y - centerY;

            const float remainingDistance =
                std::sqrt(dx * dx + dy * dy);

            constexpr float arrivalTolerance = 1.0f;
            constexpr float movementEpsilon = 0.0001f;

            const bool movementRequested =
                std::fabs(movement.x) > movementEpsilon ||
                std::fabs(movement.y) > movementEpsilon;

            const bool actuallyMoved =
                std::fabs(movementX) > movementEpsilon ||
                std::fabs(movementY) > movementEpsilon;

            // Stop when the destination is reached
            // or movement is completely blocked.
            if (remainingDistance <= arrivalTolerance ||
                (movementRequested && !actuallyMoved))
            {
                m_moveTarget.reset();
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
        m_moveTarget.reset();
        m_hoverPoint.reset();
        
        SDL_Log(
            "Successfully loaded level %d",
            m_currentLevel
        );
        return true;
    }

}