#include "DreamBox/GameScene.h"
#include "DreamBox/Collision.h"
#include <SDL3/SDL_filesystem.h>
#include <string>
#include <cmath>


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
        

        // Load the external map layout.
        const std::string mapPath =
            std::string(basePath) +
            "assets/maps/level_01.txt";


        if (m_tilemap.LoadFromFile(mapPath))
        {
            const auto spawn = m_tilemap.GetPlayerSpawn();
        
            if (spawn.has_value())
            {
                // Current collision rectangle.
                const SDL_FRect hitbox =
                    m_player.GetCollisionBounds();
            
                const float hitboxCenterX =
                    hitbox.x + hitbox.w / 2.0f;
            
                const float hitboxCenterY =
                    hitbox.y + hitbox.h / 2.0f;
            
                // Move the player's hitbox center
                // to the center of the spawn tile.
                const float moveX =
                    spawn->x - hitboxCenterX;
            
                const float moveY =
                    spawn->y - hitboxCenterY;
            
                m_player.Move(moveX, moveY);
            
                // Reject a spawn that intersects a wall.
                if (m_tilemap.CollidesWithWall(
                        m_player.GetCollisionBounds()))
                {
                    SDL_Log(
                        "Player spawn intersects a wall. "
                        "Using default position."
                    );
                
                    m_player.Move(-moveX, -moveY);
                }
                else
                {
                    SDL_Log(
                        "Player spawned at (%.1f, %.1f)",
                        spawn->x,
                        spawn->y
                    );
                }
            }
            else
            {
                SDL_Log(
                    "Map has no player spawn. "
                    "Using default position."
                );
            }
        }
        else
        {
            SDL_Log("Using default tilemap layout");
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
        // Return to Main Menu.
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            return SceneRequest::MainMenu;
        }
    
        // ----------------------------------
        // Player movement and collision
        // ----------------------------------
    
        const SDL_FRect previousBounds =
            m_player.GetBounds();
    
        SDL_FPoint movement =
            m_player.GetMovement(
                input,
                deltaTime
            );
        
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
    
        // ----------------------------------
        // Player animation
        // ----------------------------------
    
        const SDL_FRect currentBounds =
            m_player.GetBounds();
    
        const float movementX =
            currentBounds.x - previousBounds.x;
    
        const float movementY =
            currentBounds.y - previousBounds.y;
    
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
        // Interactable objects
        // ----------------------------------
    
        const bool interactPressed =
            input.IsKeyPressed(SDL_SCANCODE_E);
    
        const bool newInteractPress =
            interactPressed && !m_wasInteractPressed;
    
        if (newInteractPress)
        {
            constexpr float interactionRange = 70.0f;
        
            const float maxDistanceSquared =
                interactionRange * interactionRange;
        
            const SDL_FRect playerBounds =
                m_player.GetCollisionBounds();
        
            const float playerCenterX =
                playerBounds.x + playerBounds.w / 2.0f;
        
            const float playerCenterY =
                playerBounds.y + playerBounds.h / 2.0f;
        
            // ----------------------------------
            // Find nearest unopened chest
            // ----------------------------------
        
            Chest* nearestChest = nullptr;
        
            float nearestChestDistance =
                maxDistanceSquared;
        
            for (Chest& chest : m_chests)
            {
                if (chest.opened)
                {
                    continue;
                }
            
                const float chestCenterX =
                    chest.bounds.x + chest.bounds.w / 2.0f;
            
                const float chestCenterY =
                    chest.bounds.y + chest.bounds.h / 2.0f;
            
                const float dx =
                    playerCenterX - chestCenterX;
            
                const float dy =
                    playerCenterY - chestCenterY;
            
                const float distanceSquared =
                    dx * dx + dy * dy;
            
                if (distanceSquared <= nearestChestDistance)
                {
                    nearestChestDistance = distanceSquared;
                    nearestChest = &chest;
                }
            }
        
            if (nearestChest)
            {
                nearestChest->opened = true;
            
                SDL_Log("Treasure chest opened!");
            }
            else
            {
                // ----------------------------------
                // Find nearest door
                // ----------------------------------
            
                Door* nearestDoor = nullptr;
            
                float nearestDoorDistance =
                    maxDistanceSquared;
            
                for (Door& door : m_doors)
                {
                    const float doorCenterX =
                        door.bounds.x + door.bounds.w / 2.0f;
                
                    const float doorCenterY =
                        door.bounds.y + door.bounds.h / 2.0f;
                
                    const float dx =
                        playerCenterX - doorCenterX;
                
                    const float dy =
                        playerCenterY - doorCenterY;
                
                    const float distanceSquared =
                        dx * dx + dy * dy;
                
                    if (distanceSquared <= nearestDoorDistance)
                    {
                        nearestDoorDistance = distanceSquared;
                        nearestDoor = &door;
                    }
                }
            
                if (nearestDoor)
                {
                    if (nearestDoor->open)
                    {
                        // Don't close the door on the player.
                        if (!Collision::Intersects(
                                playerBounds,
                                nearestDoor->bounds))
                        {
                            nearestDoor->open = false;
                        
                            SDL_Log("Door closed");
                        }
                    }
                    else
                    {
                        nearestDoor->open = true;
                    
                        SDL_Log("Door opened");
                    }
                }
            }
        }
    
        // Update key state AFTER all interactions.
        m_wasInteractPressed = interactPressed;
    
        return SceneRequest::None;
    }



    void GameScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        
        // tile map rendering
        m_tilemap.Render(renderer);

        // chest rendering
        // ----------------------------------
        // Render treasure chests
        // ----------------------------------

        for (const Chest& chest : m_chests)
        {
            // Add padding around the chest graphic.
            SDL_FRect chestVisual = chest.bounds;
        
            chestVisual.x += 5.0f;
            chestVisual.y += 8.0f;
        
            chestVisual.w -= 10.0f;
            chestVisual.h -= 12.0f;
        
            if (chest.opened)
            {
                // Opened chest: darker brown.
                SDL_SetRenderDrawColor(
                    renderer,
                    95, 65, 40, 255
                );
            }
            else
            {
                // Closed chest: brighter brown.
                SDL_SetRenderDrawColor(
                    renderer,
                    165, 100, 45, 255
                );
            }
        
            SDL_RenderFillRect(
                renderer,
                &chestVisual
            );
        
            // Draw chest border.
            SDL_SetRenderDrawColor(
                renderer,
                40, 25, 15, 255
            );
        
            SDL_RenderRect(
                renderer,
                &chestVisual
            );
        
            // Highlight unopened chests when nearby.
            if (!chest.opened &&
                IsPlayerNearChest(chest.bounds))
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    255, 220, 70, 255
                );
            
                SDL_RenderRect(
                    renderer,
                    &chest.bounds
                );
            }
        }


        // ----------------------------------
        // Render doors
        // ----------------------------------

        for (const Door& door : m_doors)
        {
            SDL_FRect doorVisual = door.bounds;
        
            doorVisual.x += 6.0f;
            doorVisual.y += 2.0f;
            doorVisual.w -= 12.0f;
            doorVisual.h -= 4.0f;
        
            if (door.open)
            {
                // Open door: darker and smaller/off to the side
                SDL_SetRenderDrawColor(
                    renderer,
                    90, 60, 35, 255
                );
            }
            else
            {
                // Closed door: brighter brown
                SDL_SetRenderDrawColor(
                    renderer,
                    145, 90, 45, 255
                );
            }
        
            SDL_RenderFillRect(
                renderer,
                &doorVisual
            );
        
            SDL_SetRenderDrawColor(
                renderer,
                40, 25, 15, 255
            );
        
            SDL_RenderRect(
                renderer,
                &doorVisual
            );
        
            // Highlight if nearby.
            if (!door.open && IsPlayerNearDoor(door.bounds))
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    255, 220, 70, 255
                );
            
                SDL_RenderRect(
                    renderer,
                    &door.bounds
                );
            }
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

    }
    
    bool GameScene::PlayerCollidesWithWall() const
    {
        const SDL_FRect playerHitbox =
            m_player.GetCollisionBounds();

        // Tilemap walls.
        if (m_tilemap.CollidesWithWall(playerHitbox))
        {
            return true;
        }

        // Treasure chests.
        for (const Chest& chest : m_chests)
        {
            if (Collision::Intersects(
                    playerHitbox,
                    chest.bounds))
            {
                return true;
            }
        }

        // Closed doors only.
        for (const Door& door : m_doors)
        {
            if (!door.open &&
                Collision::Intersects(
                    playerHitbox,
                    door.bounds))
            {
                return true;
            }
        }

        return false;
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

        m_chests.clear();

        // door rendering 
        for (const SDL_FRect& bounds : m_tilemap.GetChestBounds())
        {
            m_chests.push_back(
                Chest{
                    bounds,
                    false
                }
            );
        }

        m_doors.clear();
        for (const SDL_FRect& bounds : m_tilemap.GetDoorBounds())
        {
            m_doors.push_back(
                Door{
                    bounds,
                    false
                }
            );
        }

        // Clear chests from the previous level.
        m_chests.clear();

        // Create chests from the new map.
        for (const SDL_FRect& bounds :
             m_tilemap.GetChestBounds())
        {
            m_chests.push_back(
                Chest{
                    bounds,
                    false
                }
            );
        }

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

        SDL_Log(
            "Successfully loaded level %d",
            m_currentLevel
        );

        return true;
    }

    bool GameScene::IsPlayerNearChest(
        const SDL_FRect& chestBounds
    ) const
    {
        const SDL_FRect playerBounds =
            m_player.GetCollisionBounds();

        // Player hitbox center.
        const float playerCenterX =
            playerBounds.x + playerBounds.w / 2.0f;

        const float playerCenterY =
            playerBounds.y + playerBounds.h / 2.0f;

        // Chest center.
        const float chestCenterX =
            chestBounds.x + chestBounds.w / 2.0f;

        const float chestCenterY =
            chestBounds.y + chestBounds.h / 2.0f;

        const float dx =
            playerCenterX - chestCenterX;

        const float dy =
            playerCenterY - chestCenterY;

        // Interaction range in logical world units.
        constexpr float interactionRange = 70.0f;

        // Compare squared distances.
        return dx * dx + dy * dy <=
            interactionRange * interactionRange;
    }

    bool GameScene::IsPlayerNearDoor(
        const SDL_FRect& doorBounds
    ) const
    {
        const SDL_FRect playerBounds =
            m_player.GetCollisionBounds();

        const float playerCenterX =
            playerBounds.x + playerBounds.w / 2.0f;

        const float playerCenterY =
            playerBounds.y + playerBounds.h / 2.0f;

        const float doorCenterX =
            doorBounds.x + doorBounds.w / 2.0f;

        const float doorCenterY =
            doorBounds.y + doorBounds.h / 2.0f;

        const float dx =
            playerCenterX - doorCenterX;

        const float dy =
            playerCenterY - doorCenterY;

        constexpr float interactionRange = 70.0f;

        return dx * dx + dy * dy <=
            interactionRange * interactionRange;
    }

}