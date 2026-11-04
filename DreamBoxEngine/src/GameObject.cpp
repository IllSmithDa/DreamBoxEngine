#include "DreamBox/GameObject.h"

namespace DreamBox
{
    GameObject::GameObject(
        float x,
        float y,
        float width,
        float height
    )
        : m_bounds{ x, y, width, height }
    {
    }


    void GameObject::Update(float deltaTime)
    {
        // Base game objects have no default update behavior.
    }


    void GameObject::Render(
        SDL_Renderer* renderer
    ) const
    {
        // Base game objects have no default appearance.
    }


    const SDL_FRect& GameObject::GetBounds() const
    {
        return m_bounds;
    }

    void GameObject::SetPosition(float x, float y)
    {
        m_bounds.x = x;
        m_bounds.y = y;
    }

    void GameObject::Move(float x, float y)
    {
        m_bounds.x += x;
        m_bounds.y += y;
    }
}