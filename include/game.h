#ifndef CLI_DYNO_GAME_H
#define CLI_DYNO_GAME_H

#include <cstdint>

#include "renderer.h"
#include "world.h"

namespace cli_dyno {

constexpr int kFrameMs = 45;

class Game {
public:
    void run();

private:
    enum class State { Playing, GameOver };

    void handleInput(bool& running);
    void renderFrame();

    World world_;
    Renderer renderer_;
    State state_ = State::Playing;
    std::size_t frameCount_ = 0;
};

}

#endif
