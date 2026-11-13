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

            const char* basePath = SDL_GetBasePath();
            if (!basePath)
            {
                SDL_Log(
                    "Unable to get application path: %s",
                    SDL_GetError()
                );
                return;
            }
    

        // Outer walls
        m_walls.emplace_back(
            50.0f,
            50.0f,
            40.0f,
            600.0f
        );

        m_walls.emplace_back(
            1190.0f,
            50.0f,
            40.0f,
            600.0f
        );

        m_walls.emplace_back(
            50.0f,
            50.0f,
            1180.0f,
            40.0f
        );

        m_walls.emplace_back(
            50.0f,
            610.0f,
            1180.0f,
            40.0f
        );

        // Interior obstacle
        m_walls.emplace_back(
            600.0f,
            250.0f,
            60.0f,
            250.0f
        );

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

        UpdateAnimation(
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
        
        
const SDL_FRect sourceFrame{
    static_cast<float>(m_animationFrame) * FRAME_SIZE,
    static_cast<float>(m_facing) * FRAME_SIZE,
    FRAME_SIZE,
    FRAME_SIZE
};

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


        for (const Wall& wall : m_walls)
        {
            wall.Render(renderer);
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

        for (const Wall& wall : m_walls)
        {
            if (Collision::Intersects(
                playerHitbox,
                wall.GetBounds()
            ))
            {
                return true;
            }
        }

        return false;
    }


    void GameScene::UpdateAnimation(
        float movementX,
        float movementY,
        float deltaTime
    )
    {
        constexpr float epsilon = 0.0001f;

        const bool isMoving =
            std::fabs(movementX) > epsilon ||
            std::fabs(movementY) > epsilon;

        // Return to idle animation when stationary.
        if (!isMoving)
        {
            m_animationFrame = 0;
            m_animationTimer = 0.0f;
            return;
        }

        // Determine which direction the character faces.
        if (std::fabs(movementX) >
            std::fabs(movementY))
        {
            m_facing = movementX > 0.0f
                ? Facing::Right
                : Facing::Left;
        }
        else
        {
            m_facing = movementY > 0.0f
                ? Facing::Down
                : Facing::Up;
        }

        // Advance animation using delta time.
        m_animationTimer += deltaTime;

        while (m_animationTimer >= FRAME_DURATION)
        {
            m_animationTimer -= FRAME_DURATION;

            m_animationFrame =
                (m_animationFrame + 1) % FRAME_COUNT;
        }
    }

}