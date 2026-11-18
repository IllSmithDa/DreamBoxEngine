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
            
            m_interactions.Update(
                input.IsKeyPressed(SDL_SCANCODE_E),
                m_player.GetCollisionBounds()
            );
        
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

        m_interactions.Render(
            renderer,
            m_player.GetCollisionBounds()
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