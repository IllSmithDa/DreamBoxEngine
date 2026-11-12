#include "DreamBox/GameScene.h"
#include "DreamBox/Collision.h"

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
    }


    SceneRequest GameScene::Update(
        const Input& input,
        float deltaTime
    )
    {
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            return SceneRequest::MainMenu;
        }

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
    
        return SceneRequest::None;
    }


    void GameScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        
        const SDL_FRect idleFrame{
            0.0f,
            0.0f,
            16.0f,
            16.0f
        };
        
        if (m_playerTexture &&
            m_playerTexture->IsLoaded())
        {
            m_playerTexture->Render(
                renderer,
                idleFrame,
                m_player.GetBounds()
            );
        }
        else
        {
            // Fall back to the original player rectangle.
            m_player.Render(renderer);
        }


        for (const Wall& wall : m_walls)
        {
            wall.Render(renderer);
        }
    }


    bool GameScene::PlayerCollidesWithWall() const
    {
        for (const Wall& wall : m_walls)
        {
            if (Collision::Intersects(
                    m_player,
                    wall))
            {
                return true;
            }
        }

        return false;
    }
}