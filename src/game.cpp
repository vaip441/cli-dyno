#include "game.h"

#include <chrono>
#include <thread>

#include "audio.h"
#include "input.h"

namespace cli_dyno {

void Game::run() {
    bool running = true;
    while (running) {
        const auto frameStart = std::chrono::steady_clock::now();

        handleInput(running);
        if (!running) break;

        if (state_ == State::Playing) {
            const bool died = world_.update(kFrameMs);
            if (died) {
                world_.updateHighScoreIfBeaten();
                state_ = State::GameOver;
                playGameOverSound();
            }
        }

        renderFrame();
        ++frameCount_;

        const auto frameDuration = std::chrono::steady_clock::now() - frameStart;
        const auto target = std::chrono::milliseconds(kFrameMs);
        if (frameDuration < target) {
            std::this_thread::sleep_for(target - frameDuration);
        }
    }
}

void Game::handleInput(bool& running) {
    while (keyAvailable()) {
        const int key = readKey();
        if (key < 0) {
            running = false;
            return;
        }
        if (key == 'q' || key == 'Q') {
            running = false;
            return;
        }
        if (state_ == State::Playing) {
            if (key == ' ' || key == 'w' || key == 'W') {
                world_.jumpDino();
            } else if (key == 's' || key == 'S') {
                world_.toggleDuck();
            }
        } else if (state_ == State::GameOver) {
            if (key == 'r' || key == 'R') {
                world_.reset();
                state_ = State::Playing;
            }
        }
    }
}

void Game::renderFrame() {
    renderer_.clear();
    renderer_.drawGround();
    for (const auto& obs : world_.obstacles()) {
        renderer_.drawObstacle(obs);
    }
    renderer_.drawDino(world_.dino(), static_cast<int>(frameCount_ / 3));
    renderer_.drawHud(world_.score(), world_.highScore());
    if (state_ == State::GameOver) {
        renderer_.drawGameOver(world_.score(), world_.highScore());
    }
    renderer_.flush();
}

}
