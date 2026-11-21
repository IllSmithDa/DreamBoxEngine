
#include "DreamBox/InteractionSystem.h"

#include "DreamBox/Chest.h"
#include "DreamBox/Door.h"
#include "DreamBox/Collision.h"

#include <SDL3/SDL_log.h>

#include <memory>

namespace DreamBox
{
    void InteractionSystem::Initialize(
        const std::vector<SDL_FRect>& chestBounds,
        const std::vector<SDL_FRect>& doorBounds
    )
    {

        // clear and reset interaction systems
        m_objects.clear();
        m_selectedIndex.reset();
        m_objects.clear();

        m_objects.reserve(
            chestBounds.size() + doorBounds.size()
        );

        for (const SDL_FRect& bounds : chestBounds)
        {
            m_objects.push_back(
                std::make_unique<Chest>(bounds)
            );
        }

        for (const SDL_FRect& bounds : doorBounds)
        {
            m_objects.push_back(
                std::make_unique<Door>(bounds)
            );
        }

        SDL_Log(
            "InteractionSystem: %zu objects loaded",
            m_objects.size()
        );
    }

    float InteractionSystem::DistanceSquared(
        const SDL_FRect& first,
        const SDL_FRect& second
    )
    {
        const float firstX =
            first.x + first.w / 2.0f;

        const float firstY =
            first.y + first.h / 2.0f;

        const float secondX =
            second.x + second.w / 2.0f;

        const float secondY =
            second.y + second.h / 2.0f;

        const float dx = firstX - secondX;
        const float dy = firstY - secondY;

        return dx * dx + dy * dy;
    }

    bool InteractionSystem::IsNearby(
        const SDL_FRect& player,
        const SDL_FRect& object
    )
    {
        constexpr float range = 70.0f;

        return DistanceSquared(player, object) <=
            range * range;
    }

    void InteractionSystem::Update(
        bool interactPressed,
        const SDL_FRect& playerHitbox
    )
    {
        const bool newPress =
            interactPressed && !m_wasInteractPressed;

        // Important: update this every frame.
        m_wasInteractPressed = interactPressed;

        if (!newPress)
        {
            return;
        }

        constexpr float range = 70.0f;

        const float maxDistanceSquared =
            range * range;

        Interactable* nearest = nullptr;
        float nearestDistanceSquared =
            maxDistanceSquared;

        for (const auto& object : m_objects)
        {
            if (!object->CanInteract())
            {
                continue;
            }

            const float distance =
                DistanceSquared(
                    playerHitbox,
                    object->GetBounds()
                );

            if (distance <= maxDistanceSquared &&
                (!nearest ||
                 distance < nearestDistanceSquared))
            {
                nearestDistanceSquared = distance;
                nearest = object.get();
            }
        }

        if (nearest)
        {
            nearest->Interact(playerHitbox);
        }
    }

    bool InteractionSystem::CollidesWithSolid(
        const SDL_FRect& playerHitbox
    ) const
    {
        for (const auto& object : m_objects)
        {
            if (object->IsSolid() &&
                Collision::Intersects(
                    playerHitbox,
                    object->GetBounds()
                ))
            {
                return true;
            }
        }

        return false;
    }

    void InteractionSystem::Render(
        SDL_Renderer* renderer,
        const SDL_FRect& playerHitbox,
        const std::optional<SDL_FPoint>& mousePosition
    ) const
    {
        if (!renderer)
        {
            return;
        }
      
        for (std::size_t i = 0; i < m_objects.size(); ++i)
        {
            const Interactable& object = *m_objects[i];
        
            const SDL_FRect& bounds =
                object.GetBounds();
        
            const bool nearby =
                IsNearby(playerHitbox, bounds);
        
            // Keep our existing object rendering.
            object.Render(renderer, nearby);
        
            if (!object.CanInteract())
            {
                continue;
            }
          
            // Hover does not depend on player proximity.
            const bool hovered =
                mousePosition.has_value() &&
                SDL_PointInRectFloat(
                    &mousePosition.value(),
                    &bounds
                );
              
            const bool selected =
                m_selectedIndex.has_value() &&
                *m_selectedIndex == i;
              
            if (hovered)
            {
                // Green = mouse hovering.
                SDL_SetRenderDrawColor(
                    renderer, 50, 230, 150, 255
                );
              
                SDL_RenderRect(renderer, &bounds);
            }
            else if (selected)
            {
                // Cyan = selected object.
                SDL_SetRenderDrawColor(
                    renderer, 80, 200, 255, 255
                );
              
                SDL_RenderRect(renderer, &bounds);
            }
        }
    }



    bool InteractionSystem::HandlePointerPress(
        const SDL_FPoint& point,
        const SDL_FRect& playerHitbox
    )
    {
        // Replace any previous selection.
        m_selectedIndex.reset();
    
        // Reverse order matches our rendering order.
        for (std::size_t i = m_objects.size(); i > 0; --i)
        {
            const std::size_t index = i - 1;
        
            Interactable& object =
                *m_objects[index];
        
            const SDL_FRect& bounds =
                object.GetBounds();
        
            if (!SDL_PointInRectFloat(&point, &bounds))
            {
                continue;
            }
          
            // Consume clicks on an opened chest, too.
            if (!object.CanInteract())
            {
                return true;
            }
          
            m_selectedIndex = index;
          
            return true;
        }
      
        return false;
    }



    std::optional<SDL_FRect>
    InteractionSystem::GetSelectedBounds() const
    {
        if (!m_selectedIndex.has_value() ||
            *m_selectedIndex >= m_objects.size())
        {
            return std::nullopt;
        }
      
        const Interactable& object =
            *m_objects[*m_selectedIndex];
      
        if (!object.CanInteract())
        {
            return std::nullopt;
        }
      
        return object.GetBounds();
    }

    bool InteractionSystem::TryInteractSelected(
        const SDL_FRect& playerHitbox
    )
    {
        if (!m_selectedIndex.has_value() ||
            *m_selectedIndex >= m_objects.size())
        {
            return false;
        }
      
        Interactable& object =
            *m_objects[*m_selectedIndex];
      
        if (!object.CanInteract())
        {
            m_selectedIndex.reset();
            return false;
        }
      
        if (!IsNearby(playerHitbox, object.GetBounds()))
        {
            return false;
        }
      
        object.Interact(playerHitbox);
      
        // An opened chest is no longer interactable.
        // Doors remain selectable after opening.
        if (!object.CanInteract())
        {
            m_selectedIndex.reset();
        }
      
        return true;
    }


}
