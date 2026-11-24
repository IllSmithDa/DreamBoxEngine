# DreamBox Engine

**DreamBox Engine** is a work-in-progress, lightweight 2D game engine built with **C++20**, **SDL3**, and **CMake**. It is being developed incrementally, starting with a Windows top-down game prototype and working toward a reusable foundation for simple, mobile-style 2D games.

The aim is to understand and implement the underlying systems rather than immediately depend on a large, all-in-one game framework. The sample game is a testing ground for movement, collision detection, pathfinding, scene management, and interactive objects.

> **Status:** Early development / playable technical prototype. Windows is the current development target; mobile support is a long-term goal, not a current feature.

## Project goals

- Build a small, understandable, modular 2D engine in modern C++.
- Support both keyboard and mouse/touch-style point-and-click controls.
- Explore the algorithms and mathematics behind game systems.
- Develop reusable systems for tilemaps, animated sprites, collision, navigation, and interactions.
- Practice automated testing and continuous integration as features are added.
- Keep the architecture approachable before introducing larger systems such as advanced physics or an entity-component system (ECS).

## Current features

- **Scenes and menus:** Main Menu, Settings, and gameplay scenes; fullscreen and VSync options.
- **Tile-based maps:** Text-defined levels with a 40-pixel tile grid, walls, spawn points, and exits.
- **Player movement:** WASD controls, animated directional sprites, and frame-rate-independent motion.
- **Collision detection:** A smaller feet-level player hitbox, walls, solid interactables, and axis-separated movement.
- **Click-to-move:** A* pathfinding with world-space waypoints and collision-aware destination checks.
- **Interactive objects:** Chests and doors with mouse hover, selection, proximity checks, and the **E** key.
- **Automatic approach:** Click a distant chest or door to navigate into range and interact.
- **Interactive exits:** Click an exit to navigate and change levels; simply walking over it does not trigger a transition.
- **Automated tests:** CTest suites for collision, buttons, interactions, and pathfinding, run through GitHub Actions.

## Technology

| Component | Technology |
| --- | --- |
| Language | C++20 |
| Rendering, windows, and input | SDL3 |
| Text rendering | SDL3_ttf |
| Build system | CMake (3.24+) |
| Primary development environment | Visual Studio 2022 / Windows |
| Automated testing | CTest + GitHub Actions |

SDL3 and SDL3_ttf are fetched during CMake configuration. In the project's CMake setup, SDL3 is a shared dependency (`SDL3.dll` on Windows), while SDL3_ttf is configured to build statically. Game assets are copied next to the built executable.

## Controls

| Input | Action |
| --- | --- |
| **W / A / S / D** | Move the player; cancels automatic navigation |
| **Left-click empty floor** | Find an A* route and move toward that point |
| **Left-click chest / door** | Interact immediately if near, or approach and interact automatically |
| **Left-click exit** | Navigate to the exit and switch levels upon reaching it |
| **E** | Interact with a nearby object; activate an exit when standing on it |
| **Esc** | Return to the main menu from gameplay |

Mouse hover highlights available objects and exits. The current prototype uses the same point-selection concept that could later support touch input.

## How navigation works

The game has **two coordinate systems**:

- **Grid coordinates** identify tiles: `(column, row)`.
- **World coordinates** identify positions in pixels: `(x, y)`.

The current maps use **40 × 40 pixel tiles**. Pathfinding plans a sequence of grid cells; the navigation controller converts that sequence into world-space waypoints and moves the player toward them.

### 1. A* pathfinding: balancing travel cost and estimated distance

A* chooses which grid cell to explore using:

```text
f(n) = g(n) + h(n)
```

- **`g(n)`**: Cost of the route traveled from the starting cell to cell `n`.
- **`h(n)`**: Estimated remaining cost from cell `n` to the destination.
- **`f(n)`**: Estimated total route cost through cell `n`.

In this implementation, moving one cell up, down, left, or right costs **1**. Because movement is limited to those four directions, A* uses the **Manhattan distance** heuristic:

```text
h(n) = |goalColumn - currentColumn|
     + |goalRow    - currentRow|
```

For example, if the current cell is `(2, 2)` and the goal is `(7, 5)`:

```text
h = |7 - 2| + |5 - 2|
  = 5 + 3
  = 8
```

If reaching `(2, 2)` already cost `4` steps, its estimated total is `f = 4 + 8 = 12`.

A* explores promising cells, ignores blocked ones, and uses parent links to reconstruct a path once it reaches the goal. With uniform four-directional movement and this heuristic, it finds a shortest route through the **walkable grid** when one exists.

**Where in the code:** [`Pathfinder.cpp`](DreamBoxEngine/src/Pathfinder.cpp), [`Pathfinder.h`](DreamBoxEngine/include/DreamBox/Pathfinder.h); called from [`GameScene.cpp`](DreamBoxEngine/src/GameScene.cpp) in `TrySetMovePath()` (navigation planning is a planned extraction).

### 2. Converting grid cells into world positions

For a tile size `T`, the center of a grid cell is:

```text
worldX = (column + 0.5) × T
worldY = (row    + 0.5) × T
```

With `T = 40`, the center of tile `(4, 2)` is:

```text
worldX = (4 + 0.5) × 40 = 180
worldY = (2 + 0.5) × 40 = 100
```

The opposite conversion determines which grid cell contains a world position:

```text
column = floor(worldX / T)
row    = floor(worldY / T)
```

A* works in cells; the player moves in pixels. This conversion connects the two.

**Where in the code:** `GameScene::TrySetMovePath()` in [`GameScene.cpp`](DreamBoxEngine/src/GameScene.cpp); tile dimensions are defined by [`Tilemap.h`](DreamBoxEngine/include/DreamBox/Tilemap.h).

### 3. Smooth movement and delta time

Movement should depend on **elapsed time**, not on how many frames the computer renders. The basic equation is:

```text
distance = speed × deltaTime
```

If the player moves at `200 pixels/second` and a frame takes `0.016 seconds`:

```text
distance = 200 × 0.016 = 3.2 pixels
```

To head toward a waypoint, first compute the difference between its position and the player's hitbox center:

```text
dx = targetX - playerX
dy = targetY - playerY
length = sqrt(dx² + dy²)
```

Then normalize the direction and multiply by the frame's movement distance:

```text
step = min(speed × deltaTime, length)
moveX = (dx / length) × step
moveY = (dy / length) × step
```

**Why normalize?** Without dividing by `length`, moving diagonally could be faster than moving horizontally or vertically. **Why use `min`?** It keeps the player from overshooting a nearby waypoint. The code checks that the length is nonzero before dividing.

**Where in the code:** [`NavigationController.cpp`](DreamBoxEngine/src/NavigationController.cpp), especially `CalculateMovement()` and `UpdateAfterMovement()`; [`GameScene.cpp`](DreamBoxEngine/src/GameScene.cpp) applies the requested movement and checks collisions.

### 4. Rectangle collision detection

Game objects are checked using **axis-aligned bounding boxes (AABBs)**: rectangles whose sides are parallel to the screen axes. Two rectangles overlap when they intersect on **both** axes.

For rectangles A and B:

```text
A.left   < B.right   AND   A.right  > B.left
A.top    < B.bottom  AND   A.bottom > B.top
```

The player sprite may look larger than its actual collision rectangle. DreamBox uses a smaller **feet-level hitbox** (currently approximately **48 × 32 pixels**) so the visible character can overlap background details naturally without treating the entire sprite as solid.

Horizontal and vertical movement are checked separately:

1. Try moving horizontally; undo that component if it collides.
2. Try moving vertically; undo that component if it collides.

This allows the player to **slide along walls** instead of stopping entirely when just one movement axis is blocked.

A* also checks whether the **full hitbox** fits at proposed tile centers. An empty-looking tile is not automatically a valid destination if the player would overlap an adjacent wall or closed door.

**Where in the code:** [`Collision.cpp`](DreamBoxEngine/src/Collision.cpp), [`Player.cpp`](DreamBoxEngine/src/Player.cpp), `GameScene::PlayerCollidesWithWall()`, and `GameScene::TrySetMovePath()`.

### 5. Interaction distance without unnecessary square roots

Nearby object interactions use the distance between the player's and object's centers. The ordinary Euclidean distance is:

```text
distance = sqrt(dx² + dy²)
```

But if the only question is *"Are they within 70 pixels?"*, we can compare squared distances instead:

```text
dx² + dy² <= 70²
```

Both comparisons give the same answer for nonnegative distances, while the squared-distance version avoids computing `sqrt`.

For distant objects, the engine searches nearby candidate tile centers, checks that the player could stand there, and asks A* whether the candidate is reachable. Once the player is close enough, the pending interaction executes automatically.

**Where in the code:** [`InteractionSystem.cpp`](DreamBoxEngine/src/InteractionSystem.cpp) (`IsNearby()` / distance calculations), and `GameScene::TryNavigateToSelectedObject()` in [`GameScene.cpp`](DreamBoxEngine/src/GameScene.cpp).

## Code organization

A simplified view of the current codebase (paths shown relative to the repository root):

```text
DreamBoxEngine/
├── CMakeLists.txt                   # Top-level build configuration
├── DreamBoxEngine/
│   ├── CMakeLists.txt               # Engine and test targets
│   ├── assets/
│   │   ├── maps/                    # level_01.txt, level_02.txt
│   │   ├── sprites/                 # Player and terrain images
│   │   └── fonts/                   # Montserrat font assets
│   ├── include/DreamBox/           # Header files
│   └── src/
│       ├── Engine.cpp              # Main loop / scene coordination
│       ├── GameScene.cpp           # Gameplay coordination
│       ├── NavigationController.cpp # Waypoints and navigation progress
│       ├── Pathfinder.cpp          # A* grid search
│       ├── Tilemap.cpp             # Level data and wall checks
│       ├── InteractionSystem.cpp   # Object selection and interaction
│       ├── Chest.cpp / Door.cpp    # Interactive object behavior
│       ├── Player.cpp              # Player movement / hitbox
│       ├── SpriteAnimator.cpp      # Directional animation
│       └── ...                     # Menus, input, rendering helpers
├── tests/                           # Automated unit tests
└── .github/workflows/               # GitHub Actions CI
```

`GameScene` still coordinates several gameplay systems. Refactoring is ongoing: the first step moves route-following state and waypoint progression into `NavigationController`, with route planning and other responsibilities to be separated incrementally.

## Building on Windows

**Requirements:** Windows, Visual Studio 2022 with the C++ development tools, CMake 3.24+, Git, and network access for CMake to fetch SDL dependencies on first configuration.

From the **repository root**, run:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The executable and copied assets are placed in the CMake build output (typically `build/bin/Release/` with this generator). Keep the copied `assets/` directory and `SDL3.dll` alongside the executable when running it outside Visual Studio.

If you already configured `build/` in Visual Studio or with another generator, reuse that configuration rather than changing generators in the same build directory.

## Running tests

```powershell
ctest --test-dir build -C Release --output-on-failure
```

Current test suites:

| Test | Focus |
| --- | --- |
| `CollisionTests` | Rectangle collision behavior |
| `ButtonTests` | UI button behavior |
| `InteractionTests` | Chests, doors, selection, and interaction ranges |
| `PathfinderTests` | Valid A* routes, obstacle avoidance, unreachable targets, and edge cases |

GitHub Actions builds the project and runs these tests on pushed changes. A separate `NavigationControllerTests` suite is a planned improvement as the navigation refactor progresses.

## Roadmap

These are development directions, not completed features:

- [ ] Finish separating path planning, waypoint following, and gameplay coordination.
- [ ] Add focused automated tests for the navigation controller and interactive exits.
- [ ] Improve paths around tight corners and reconsider routes when obstacles change.
- [ ] Expand interactables, levels, and game-object behaviors.
- [ ] Explore saving/loading and additional game systems as needed.
- [ ] Evaluate mobile platform support after the desktop foundation is stable.

## Development philosophy

DreamBox Engine is both a working prototype and a learning project. Each new feature is an opportunity to understand **why an algorithm works**, **where it belongs in the architecture**, and **how to test it**.

The goal is not to compete with established engines. It is to build a small foundation that is easy to reason about, modify, and grow one system at a time.
