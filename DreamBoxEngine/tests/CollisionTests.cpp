#include <iostream>

#include "DreamBox/Collision.h"
#include "DreamBox/GameObject.h"

int main()
{
    DreamBox::GameObject objectA(
        0.0f,
        0.0f,
        100.0f,
        100.0f
    );

    DreamBox::GameObject objectB(
        50.0f,
        50.0f,
        100.0f,
        100.0f
    );

    DreamBox::GameObject objectC(
        200.0f,
        200.0f,
        100.0f,
        100.0f
    );

    // These objects overlap.
    if (!DreamBox::Collision::Intersects(objectA, objectB))
    {
        std::cerr << "FAILED: Expected objects A and B to collide.\n";
        return 1;
    }

    // These objects are separated.
    if (DreamBox::Collision::Intersects(objectA, objectC))
    {
        std::cerr << "FAILED: Objects A and C should not collide.\n";
        return 1;
    }

    std::cout << "Collision tests passed.\n";

    return 0;
}