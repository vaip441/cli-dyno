# CLI Dino — Design

## Goal

A CLI clone of the Google Chrome dinosaur game, written in modern C++ with a clean multi-file layout suitable for a school project. The existing single-file foundation in `src/main.cpp` (dino physics, screen buffer, raw-mode input, game loop at ~22 fps) is refactored into well-bounded modules and extended with obstacles, collisions, scoring, game-over/restart, difficulty progression, birds with ducking, and a persisted high score.


## Scope

In scope:

- Cactus obstacles
- Flying birds (low and high altitudes); low birds force ducking, high birds can be jumped or ducked under
- Collision detection
- Live score and persisted best score
- Game-over overlay with restart
- Gradual speed increase; birds appear only after score crosses a threshold
- Multi-file C++17 project with a Makefile, `src/`, `include/`, `obj/`, `bin/`

Out of scope:

- Day/night cycle
- Sound
- Windows-native support (POSIX-only: Linux, macOS, WSL — matches the existing foundation)
- Networking, leaderboards beyond the single local best score
- Unit tests (school project; manual verification is sufficient)

## Directory layout

```
cli-dyno/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── dino.cpp
│   ├── obstacle.cpp
│   ├── world.cpp
│   ├── renderer.cpp
│   ├── input.cpp
│   └── game.cpp
├── include/
│   ├── dino.h
│   ├── obstacle.h
│   ├── world.h
│   ├── renderer.h
│   ├── input.h
│   └── game.h
├── obj/        (generated, .o files)
└── bin/        (generated, cli-dyno executable)
```

## Build system

`Makefile` using `g++ -std=c++17 -Wall -Wextra -Iinclude`. A pattern rule `obj/%.o: src/%.cpp` compiles each translation unit; the link step produces `bin/cli-dyno`. Targets:

- `all` (default) — build `bin/cli-dyno`
- `clean` — remove `obj/` and `bin/`
- `run` — build and execute `bin/cli-dyno`

`obj/` and `bin/` are created on demand by the build rules.

## Modules

### `dino` — physics and posture

```cpp
struct Dino {
    float height;
    float velocity;
    bool isJumping;
    bool isDucking;

    void jump();
    void setDucking(bool down);
    void update();
};
```

Owns vertical physics (same gravity/jump-velocity constants as the foundation). `jump()` is ignored while already in the air. `setDucking(true)` lowers/widens the dino sprite; ignored while jumping. `update()` advances height and velocity per frame and clamps to the ground. Also exposes `hitBox()` returning a small rectangle in screen coordinates (smaller vertical extent when ducking).

### `obstacle` — cacti and birds

```cpp
enum class ObstacleKind { Cactus, BirdLow, BirdHigh };

struct Obstacle {
    ObstacleKind kind;
    float x;        // screen column (float for sub-frame motion)
    int rowOffset;  // vertical offset from ground, derived from kind
};
```

Free helpers: `spriteFor(kind)` returns the rows of characters for drawing; `hitBoxFor(kind, x)` returns the rectangle used for collision. No inheritance — the kind enum keeps the code simple.

Cacti sit on the ground. `BirdLow` flies just above the ground (must duck). `BirdHigh` flies at jump-apex altitude (jump or duck both work).

### `world` — gameplay state

Owns the `Dino`, the `std::vector<Obstacle>`, the current `speed` (columns/frame), `score`, `highScore`, and a `std::mt19937` RNG seeded from `std::random_device` at construction. Public surface:

- `void reset()` — clears obstacles, resets score and dino state, keeps `highScore`.
- `bool update()` — one tick: scroll obstacles left by `speed`, drop off-screen ones, maybe spawn a new one, increment `score`, recompute `speed`, check collision. Returns `true` when the dino just died this tick.
- `void loadHighScore()` — read `highscore.txt` in cwd; missing/unreadable → `highScore = 0`.
- `void saveHighScore()` — overwrite `highscore.txt` with a single decimal integer.
- Const accessors for `dino`, `obstacles`, `score`, `highScore`.

**Speed curve:** `speed = min(2.5, 1.0 + score / 500.0)`.

**Spawn cadence:** track distance since last spawn; when it exceeds a randomized gap (gap minimum shrinks as `speed` rises), spawn a new obstacle.

**Obstacle mix:** if `score <= 300` → always `Cactus`. Otherwise: 60% `Cactus`, 25% `BirdLow`, 15% `BirdHigh`.

**Collision:** rectangle overlap between `dino.hitBox()` and each `hitBoxFor(obs.kind, obs.x)`.

### `renderer` — screen buffer and output

Owns a `std::vector<std::string>` buffer sized to the play area (`kScreenWidth` × `kScreenHeight`). Methods:

- `void clear()` — fill with spaces.
- `void drawText(row, col, text)` — clipped write.
- `void drawGround()` — bottom line of dashes.
- `void drawDino(dino, animFrame)` — picks standing/running/jumping/ducking sprite based on dino state and animation frame counter.
- `void drawObstacle(obs)` — uses `spriteFor(obs.kind)`.
- `void drawHud(score, highScore)` — top-right "SCORE 00123  HI 00456".
- `void drawGameOver(score, highScore)` — centered overlay: "GAME OVER — R to restart, Q to quit".
- `void flush()` — ANSI clear+home then write buffer to stdout.

### `input` — keyboard

- `class TerminalRawMode` — RAII wrapper (already present in the foundation, moved here).
- `bool keyAvailable()` — non-blocking `select()` check.
- `int readKey()` — `std::getchar()`.

No game logic; just bytes.

### `game` — loop and state machine

```cpp
class Game {
public:
    void run();
private:
    enum class State { Playing, GameOver };
    World world_;
    Renderer renderer_;
    State state_;
    std::size_t frameCount_;

    void handleInput(bool& running);
    void tick();
    void render();
};
```

Owns the loop, frame timing (same ~45 ms target as the foundation), state machine, and input → action mapping:

- Playing: Space/W → `dino.jump()`; S held → `dino.setDucking(true)`, released → `setDucking(false)`; Q → quit.
- GameOver: R → `world.reset()`, `state_ = Playing`; Q → quit.

Each tick: drain input, if `Playing` call `world.update()` and transition to `GameOver` on collision (saving high score if beaten); then render unconditionally.

### `main` — entry point

Three lines: construct `TerminalRawMode`, construct `Game`, call `run()`. Returns 0.

## Data flow

```
keyboard --> input --> game --> world (dino + obstacles)
                                  |
                                  v
                              renderer --> stdout
```

`game` is the only module that knows about all the others. `world` does not know about `renderer` or `input`. `renderer` reads `dino` and `obstacle` data but does not mutate them. `input` knows nothing about game state.

## State machine

```
            collision
 Playing ----------------> GameOver
    ^                         |
    |       press R           |
    +-------------------------+
```

Quit (Q) is accepted from either state and exits the loop.

## Frame budget

Target frame time `45 ms` (~22 fps), preserved from the foundation. The loop sleeps for the remainder of each frame after work is done.

## Ducking semantics

S is a hold-to-duck control. The game polls "is S currently down" each tick rather than treating keypresses as edges, because terminals only deliver discrete byte events. To emulate hold, the game tracks the last time an S byte was seen; if no S byte arrives for ~80 ms, ducking is released. This is a known terminal limitation and is acceptable for a school project.

## High-score persistence

- File path: `highscore.txt` in the current working directory.
- Format: one decimal integer, no trailing content.
- Loaded once at `World` construction; saved on game-over only when the new score exceeds the stored best.
- Failures to read/write are swallowed silently — they never crash the game.

## Constants (initial values)

| Name | Value |
|---|---|
| `kScreenWidth` | 70 |
| `kScreenHeight` | 20 |
| `kGroundRow` | 18 |
| `kDinoX` | 6 |
| `kGravity` | -0.8 |
| `kJumpVelocity` | 4.0 |
| `kFrameMs` | 45 |
| `kSpeedStart` | 1.0 |
| `kSpeedMax` | 2.5 |
| `kSpeedScoreDivisor` | 500.0 |
| `kBirdScoreThreshold` | 300 |
| `kDuckHoldoffMs` | 80 |

These live as `constexpr` in the relevant headers (`renderer.h` for screen dims, `dino.h` for physics, `world.h` for spawn/speed tuning, `game.h` for frame timing).

## Manual verification checklist

After implementation, the game should:

1. Launch into Playing state; dino runs in place over scrolling ground.
2. Space/W jumps; gravity returns the dino to the ground.
3. Cacti spawn and scroll left; jumping over them avoids collision.
4. Holding S ducks; releasing returns to standing.
5. Hitting a cactus → GameOver overlay; obstacles freeze.
6. Pressing R restarts cleanly; score resets, dino resets, high score is preserved.
7. Score above 300 starts spawning birds; low birds force a duck, high birds work with jump or duck.
8. Speed visibly increases as score climbs, capped well before being unplayable.
9. Beating the best score persists across program runs.
10. Q quits from any state; terminal returns to a normal state (raw mode restored by RAII).
