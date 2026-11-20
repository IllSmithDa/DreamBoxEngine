
#include "DreamBox/Pathfinder.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace DreamBox;

void Require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void VerifyPath(
    const std::vector<GridPoint>& path,
    const std::vector<std::string>& map,
    GridPoint start,
    GridPoint goal
)
{
    Require(!path.empty(), "Expected a path");
    Require(path.front() == start, "Wrong start");
    Require(path.back() == goal, "Wrong destination");

    for (std::size_t i = 0; i < path.size(); ++i)
    {
        const GridPoint point = path[i];

        Require(
            map[point.y][point.x] != '#',
            "Path crosses a wall"
        );

        if (i > 0)
        {
            const int distance =
                std::abs(point.x - path[i - 1].x) +
                std::abs(point.y - path[i - 1].y);

            Require(
                distance == 1,
                "Path contains a nonadjacent step"
            );
        }
    }
}

int main()
{
    try
    {
        // Test 1: Straight path.
        const std::vector<std::string> openMap{
            ".....",
            ".....",
            "....."
        };

        auto openWalkable = [&](int x, int y)
        {
            return openMap[y][x] != '#';
        };

        auto straight = Pathfinder::FindPath(
            {0, 1}, {4, 1},
            5, 3, openWalkable
        );

        VerifyPath(
            straight, openMap, {0, 1}, {4, 1}
        );

        Require(
            straight.size() == 5,
            "Straight path should contain 5 tiles"
        );

        std::cout << "PASS: Straight path\n";

        // Test 2: Route around a wall.
        const std::vector<std::string> wallMap{
            ".......",
            "...#...",
            "...#...",
            "...#...",
            "......."
        };

        auto wallWalkable = [&](int x, int y)
        {
            return wallMap[y][x] != '#';
        };

        auto detour = Pathfinder::FindPath(
            {1, 2}, {5, 2},
            7, 5, wallWalkable
        );

        VerifyPath(
            detour, wallMap, {1, 2}, {5, 2}
        );

        Require(
            detour.size() == 9,
            "Expected shortest detour around wall"
        );

        std::cout << "PASS: Wall avoidance\n";

        // Test 3: Unreachable destination.
        const std::vector<std::string> blockedMap{
            "..#..",
            "..#..",
            "..#.."
        };

        auto blockedWalkable = [&](int x, int y)
        {
            return blockedMap[y][x] != '#';
        };

        auto unreachable = Pathfinder::FindPath(
            {0, 1}, {4, 1},
            5, 3, blockedWalkable
        );

        Require(
            unreachable.empty(),
            "Unreachable target should return no path"
        );

        std::cout << "PASS: Unreachable target\n";

        // Test 4: Start and goal are identical.
        auto samePosition = Pathfinder::FindPath(
            {1, 1}, {1, 1},
            5, 3, blockedWalkable
        );

        Require(
            samePosition.size() == 1,
            "Identical start/goal should give one tile"
        );

        std::cout << "PASS: Same start and goal\n";

        // Test 5: Destination is blocked.
        auto blockedGoal = Pathfinder::FindPath(
            {0, 0}, {2, 0},
            5, 3, blockedWalkable
        );

        Require(
            blockedGoal.empty(),
            "Blocked goal should return no path"
        );

        std::cout << "PASS: Blocked destination\n";

        std::cout << "\nAll Pathfinder tests passed!\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "FAIL: " << error.what()
            << '\n';

        return 1;
    }
}