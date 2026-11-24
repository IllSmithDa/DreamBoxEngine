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


    void GameScene::CancelAutomaticAction()
    {
        // Stop navigation.
        CancelMovePath();

        // Discard the requested action.
        m_pendingAction = PendingAction::None;
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
            HandlePointerNavigation(
                input.GetPointPosition()
            );
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
            // Manual controls override automatic actions.
            CancelAutomaticAction();
        
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

        // ----------------------------------
        // Activate explicitly requested exits
        // ----------------------------------

        const bool keyboardExitRequested =
            onExit &&
            input.IsKeyPressed(SDL_SCANCODE_E);

        const bool pendingExitRequested =
            m_pendingAction == PendingAction::LevelExit;

        if (onExit &&
            (pendingExitRequested || keyboardExitRequested))
        {
            const int nextLevel =
                (m_currentLevel == 1) ? 2 : 1;
        
            if (LoadLevel(nextLevel))
            {
                SDL_Log("Interactive exit activated");
                return SceneRequest::None;
            }
        
            // Transition failed.
            CancelAutomaticAction();
        }

        // The route ended before reaching the exit.
        if (m_pendingAction == PendingAction::LevelExit &&
            !m_navigation.HasPath())
        {
            SDL_Log("Pending exit navigation cancelled");
        
            CancelAutomaticAction();
        }

        // ----------------------------------
        // Keyboard interactions
        // ----------------------------------

        const bool interactPressed =
            input.IsKeyPressed(SDL_SCANCODE_E);

        if (interactPressed &&
            m_pendingAction != PendingAction::None)
        {
            // Manual E takes priority over queued actions.
            if (m_pendingAction ==
                PendingAction::ObjectInteraction)
            {
                m_interactions.ClearSelection();
            }
        
            CancelAutomaticAction();
        }

        // Continue forwarding the keyboard state
        // every frame for correct edge detection.
        m_interactions.Update(
            interactPressed,
            m_player.GetCollisionBounds()
        );

        // ----------------------------------
        // Automatic object interaction
        // ----------------------------------

        if (m_pendingAction ==
            PendingAction::ObjectInteraction)
        {
            if (!m_interactions
                    .GetSelectedBounds()
                    .has_value())
            {
                CancelAutomaticAction();
            }
            else if (m_interactions.TryInteractSelected(
                         m_player.GetCollisionBounds()))
            {
                SDL_Log("Automatic interaction completed");
            
                CancelAutomaticAction();
            }
            else if (!m_navigation.HasPath())
            {
                SDL_Log(
                    "Automatic interaction cancelled: "
                    "destination not reached"
                );
            
                CancelAutomaticAction();
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
            
        CancelAutomaticAction();
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
        return m_navigation.TryNavigateNear(
            destination,
            m_player.GetCollisionBounds(),
        
            Tilemap::Columns,
            Tilemap::Rows,
            static_cast<float>(Tilemap::TileSize),
        
            [this](const SDL_FRect& candidate)
            {
                return
                    m_tilemap.CollidesWithWall(candidate) ||
                    m_interactions.CollidesWithSolid(candidate);
            },
        
            // Maximum distance from original click.
            32.0f
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

        constexpr float interactionRange = 70.0f;

        return m_navigation.TryNavigateWithinRange(
            selectedBounds.value(),

            m_player.GetCollisionBounds(),

            interactionRange,

            Tilemap::Columns,
            Tilemap::Rows,
            static_cast<float>(Tilemap::TileSize),

            [this](const SDL_FRect& candidate)
            {
                return
                    m_tilemap.CollidesWithWall(candidate) ||
                    m_interactions.CollidesWithSolid(candidate);
            }
        );
    }

    void GameScene::HandlePointerNavigation(
        const SDL_FPoint& point
    )
    {
        // Every new click replaces the previous command.
        CancelAutomaticAction();

        const SDL_FRect playerHitbox =
            m_player.GetCollisionBounds();

        // ----------------------------------
        // 1. Check for a level exit
        // ----------------------------------

        const auto exitBounds =
            m_tilemap.GetExitBounds();

        if (exitBounds.has_value() &&
            SDL_PointInRectFloat(
                &point,
                &exitBounds.value()))
        {
            m_interactions.ClearSelection();

            // Already standing on the exit.
            // The normal Update() transition check
            // will activate it this frame.
            if (Collision::Intersects(
                    playerHitbox,
                    exitBounds.value()))
            {
                m_pendingAction = PendingAction::LevelExit;
                return;
            }

            // Navigate to the exit center.
            const SDL_FPoint destination{
                exitBounds->x + exitBounds->w / 2.0f,
                exitBounds->y + exitBounds->h / 2.0f
            };

            if (TrySetMovePath(destination))
            {
                m_pendingAction = PendingAction::LevelExit;

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

            return;
        }

        // ----------------------------------
        // 2. Check for an interactable object
        // ----------------------------------

        const bool clickedObject =
            m_interactions.HandlePointerPress(
                point,
                playerHitbox
            );

        if (clickedObject)
        {
            // Opened chests may consume a click
            // without providing an active selection.
            if (!m_interactions
                    .GetSelectedBounds()
                    .has_value())
            {
                return;
            }

            // First attempt an immediate interaction.
            if (m_interactions.TryInteractSelected(
                    playerHitbox))
            {
                return;
            }

            // Otherwise approach the selected object.
            if (TryNavigateToSelectedObject())
            {
                m_pendingAction =
                    PendingAction::ObjectInteraction;
            }

            return;
        }

        // ----------------------------------
        // 3. Ordinary floor navigation
        // ----------------------------------

        TrySetMovePath(point);
    }


}