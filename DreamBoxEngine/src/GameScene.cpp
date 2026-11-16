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
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
                m_showCollisionBounds = !m_showCollisionBounds;

            return SceneRequest::MainMenu;
        }

        const SDL_FRect previousBounds = m_player.GetBounds();

        SDL_FPoint movement =
            m_player.GetMovement(
                input,
                deltaTime
            );
        
        m_player.Move(
            movement.x,
            0.0f
        );
    
        if (PlayerCollidesWithWall())
        {
            m_player.Move(
                -movement.x,
                0.0f
            );
        }
    
        m_player.Move(
            0.0f,
            movement.y
        );
    
        if (PlayerCollidesWithWall())
        {
            m_player.Move(
                0.0f,
                -movement.y
            );
        }

        const SDL_FRect currentBounds = m_player.GetBounds();

        const float movementX = currentBounds.x - previousBounds.x;

        const float movementY = currentBounds.y - previousBounds.y;

        m_playerAnimator.Update(
            movementX,
            movementY,
            deltaTime
        );
    
        return SceneRequest::None;
    }


    void GameScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        
        // tile map rendering
        m_tilemap.Render(renderer);

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
        return m_tilemap.CollidesWithWall(
            m_player.GetCollisionBounds()
        );
    }


}