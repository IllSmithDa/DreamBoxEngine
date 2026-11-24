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


    void GameScene::CancelMovePath()
    {
        m_navigation.Cancel();
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
        // Pointer selection and navigation
        // ----------------------------------

        if (input.WasPointPressed())
        {
            const SDL_FPoint point =
                input.GetPointPosition();

            // A new click cancels previous actions.
            CancelMovePath();
            m_pendingInteraction = false;
            m_pendingExit = false;

            const auto exitBounds =
                m_tilemap.GetExitBounds();

            const bool clickedExit =
                exitBounds.has_value() &&
                SDL_PointInRectFloat(
                    &point,
                    &exitBounds.value()
                );

            if (clickedExit)
            {
                // Exit selection takes priority over
                // ordinary floor movement.
                m_interactions.ClearSelection();

                const SDL_FRect playerHitbox =
                    m_player.GetCollisionBounds();

                // Already standing on the exit?
                if (Collision::Intersects(
                        playerHitbox,
                        exitBounds.value()))
                {
                    const int nextLevel =
                        (m_currentLevel == 1) ? 2 : 1;

                    LoadLevel(nextLevel);
                    return SceneRequest::None;
                }

                // Navigate toward the center of the exit.
                const SDL_FPoint destination{
                    exitBounds->x + exitBounds->w / 2.0f,
                    exitBounds->y + exitBounds->h / 2.0f
                };

                if (TrySetMovePath(destination))
                {
                    // Set only after a valid route exists.
                    m_pendingExit = true;

                    SDL_Log(
                        "Exit selected: navigating toward exit"
                    );
                }
                else
                {
                    SDL_Log(
                        "Exit selected but no route available"
                    );
                }
            }
            else
            {
                // Ordinary interactable objects.
                const bool clickedObject =
                    m_interactions.HandlePointerPress(
                        point,
                        m_player.GetCollisionBounds()
                    );

                if (clickedObject)
                {
                    const bool hasSelection =
                        m_interactions
                            .GetSelectedBounds()
                            .has_value();

                    if (hasSelection)
                    {
                        const bool interacted =
                            m_interactions.TryInteractSelected(
                                m_player.GetCollisionBounds()
                            );

                        if (!interacted &&
                            TryNavigateToSelectedObject())
                        {
                            m_pendingInteraction = true;
                        }
                    }
                }
                else
                {
                    // Ordinary floor click.
                    TrySetMovePath(point);
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
            // Manual controls always take priority.
            CancelMovePath();
            m_pendingInteraction = false;
            m_interactions.ClearSelection();
        }
        else if (m_navigation.HasPath())
        {
            movement = m_navigation.CalculateMovement(
                m_player.GetCollisionBounds(),
                deltaTime
            );
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

        m_navigation.UpdateAfterMovement(
            movement,
            SDL_FPoint{movementX, movementY},
            m_player.GetCollisionBounds()
        );


        // ----------------------------------
        // 6. Player animation
        // ----------------------------------

        m_playerAnimator.Update(
            movementX,
            movementY,
            deltaTime
        );


        // ----------------------------------
        // Level exit detection
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

        // Keyboard players must explicitly press E
        // while standing on the exit.
        const bool keyboardExitRequested =
            onExit &&
            input.IsKeyPressed(SDL_SCANCODE_E);

        // A transition now requires player intent.
        if (onExit &&
            (m_pendingExit || keyboardExitRequested))
        {
            const int nextLevel =
                (m_currentLevel == 1) ? 2 : 1;

            if (LoadLevel(nextLevel))
            {
                SDL_Log("Interactive exit activated");
                return SceneRequest::None;
            }

            m_pendingExit = false;
        }

        // If navigation was canceled or blocked before
        // reaching the exit, discard the pending request.
        if (m_pendingExit &&
            !m_navigation.HasPath())
        {
            SDL_Log("Pending exit navigation cancelled");
            m_pendingExit = false;
        }


        // ----------------------------------
        // Keyboard and automatic interactions
        // ----------------------------------

        const bool interactPressed =
            input.IsKeyPressed(SDL_SCANCODE_E);

        // Manual interaction cancels a pending
        // automatic interaction to avoid triggering
        // the same object twice.
        if (interactPressed && m_pendingInteraction)
        {
            m_pendingInteraction = false;
            CancelMovePath();
            m_interactions.ClearSelection();
        }

        // Manual E interactions also cancel an unfinished
        // exit-navigation request.
        if (input.IsKeyPressed(SDL_SCANCODE_E) &&
            m_pendingExit)
        {
            CancelMovePath();
            m_pendingExit = false;
        }

        // Keep forwarding keyboard state every frame.
        m_interactions.Update(
            interactPressed,
            m_player.GetCollisionBounds()
        );

        // ----------------------------------
        // Execute pending interaction
        // ----------------------------------

        if (m_pendingInteraction)
        {
            // The selected object might have been removed
            // or become unavailable.
            if (!m_interactions.GetSelectedBounds().has_value())
            {
                m_pendingInteraction = false;
                CancelMovePath();
            }
            else if (m_interactions.TryInteractSelected(
                         m_player.GetCollisionBounds()))
            {
                SDL_Log("Automatic interaction completed");
            
                // Stop walking immediately after interacting.
                m_pendingInteraction = false;
                CancelMovePath();
            }
            else if (!m_navigation.HasPath())
            {
                // The route ended or was blocked before
                // we could reach interaction range.
                SDL_Log(
                    "Automatic interaction cancelled: "
                    "destination not reached"
                );
            
                m_pendingInteraction = false;
            }
        }

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


        // ----------------------------------
        // Interactive exit highlighting
        // ----------------------------------
        
        const auto exitBounds =
            m_tilemap.GetExitBounds();
        
        if (exitBounds.has_value())
        {
            const bool hovered =
                m_hoverPoint.has_value() &&
                SDL_PointInRectFloat(
                    &m_hoverPoint.value(),
                    &exitBounds.value()
                );
            
            if (hovered)
            {
                // Green highlight when hovered.
                SDL_SetRenderDrawColor(
                    renderer,
                    50, 230, 150, 255
                );
            }
            else
            {
                // Subtle blue outline to identify exits.
                SDL_SetRenderDrawColor(
                    renderer,
                    110, 145, 220, 255
                );
            }
        
            SDL_RenderRect(
                renderer,
                &exitBounds.value()
            );
        }

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

        const auto& moveTarget =
            m_navigation.GetDestination();

        if (moveTarget.has_value())
        {
            SDL_SetRenderDrawColor(
                renderer,
                50, 230, 150, 255
            );

            constexpr float markerSize = 6.0f;

            // Horizontal line.
            SDL_RenderLine(
                renderer,
                moveTarget->x - markerSize,
                moveTarget->y,
                moveTarget->x + markerSize,
                moveTarget->y
            );

            // Vertical line.
            SDL_RenderLine(
                renderer,
                moveTarget->x,
                moveTarget->y - markerSize,
                moveTarget->x,
                moveTarget->y + markerSize
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
        m_pendingInteraction = false;
        m_pendingExit = false;
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
        return m_navigation.TryNavigateTo(
            destination,

            // The actor's current collision dimensions.
            m_player.GetCollisionBounds(),

            // Current map configuration.
            Tilemap::Columns,
            Tilemap::Rows,
            static_cast<float>(Tilemap::TileSize),

            // Scene-specific collision callback.
            [this](const SDL_FRect& candidate)
            {
                return
                    m_tilemap.CollidesWithWall(candidate) ||
                    m_interactions.CollidesWithSolid(candidate);
            }
        );
    }



    bool GameScene::TryNavigateToSelectedObject()
    {
        const auto selectedBounds =
            m_interactions.GetSelectedBounds();

        if (!selectedBounds.has_value())
        {
            return false;
        }

        constexpr float tileSize =
            static_cast<float>(Tilemap::TileSize);

        constexpr float interactionRange = 70.0f;
        constexpr float rangeSquared =
            interactionRange * interactionRange;

        const SDL_FRect playerHitbox =
            m_player.GetCollisionBounds();

        const SDL_FPoint playerCenter{
            playerHitbox.x + playerHitbox.w / 2.0f,
            playerHitbox.y + playerHitbox.h / 2.0f
        };

        const SDL_FPoint objectCenter{
            selectedBounds->x + selectedBounds->w / 2.0f,
            selectedBounds->y + selectedBounds->h / 2.0f
        };

        struct ApproachCandidate
        {
            SDL_FPoint position{};
            float distanceFromPlayer = 0.0f;
        };

        std::vector<ApproachCandidate> candidates;

        // ----------------------------------
        // Find tile centers near the object
        // ----------------------------------

        for (int row = 0; row < Tilemap::Rows; ++row)
        {
            for (int column = 0;
                 column < Tilemap::Columns;
                 ++column)
            {
                const SDL_FPoint position{
                    (static_cast<float>(column) + 0.5f) *
                        tileSize,
                    (static_cast<float>(row) + 0.5f) *
                        tileSize
                };

                const float dx =
                    position.x - objectCenter.x;

                const float dy =
                    position.y - objectCenter.y;

                const float distanceSquared =
                    dx * dx + dy * dy;

                // This tile center must be within
                // the object's interaction range.
                if (distanceSquared > rangeSquared)
                {
                    continue;
                }

                const float playerDx =
                    position.x - playerCenter.x;

                const float playerDy =
                    position.y - playerCenter.y;

                const float playerDistanceSquared =
                    playerDx * playerDx +
                    playerDy * playerDy;

                candidates.push_back({
                    position,
                    playerDistanceSquared
                });
            }
        }

        // ----------------------------------
        // Try nearby candidates first
        // ----------------------------------

        std::sort(
            candidates.begin(),
            candidates.end(),
            [](const ApproachCandidate& a,
               const ApproachCandidate& b)
            {
                return a.distanceFromPlayer <
                       b.distanceFromPlayer;
            }
        );

        for (const ApproachCandidate& candidate : candidates)
        {
            // Reuse our existing collision-aware
            // A* navigation method.
            if (TrySetMovePath(candidate.position))
            {
                SDL_Log(
                    "Approach point selected: %.1f, %.1f",
                    candidate.position.x,
                    candidate.position.y
                );

                return true;
            }
        }

        SDL_Log(
            "No reachable approach point found"
        );

        return false;
    }

}