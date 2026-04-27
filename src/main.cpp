#include <chrono>
#include <conio.h>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr int kScreenWidth = 70;
constexpr int kScreenHeight = 20;
constexpr int kGroundRow = kScreenHeight - 2;
constexpr int kDinoX = 6;
constexpr float kGravity = -0.7f;
constexpr float kJumpVelocity = 8.5f;

struct Dino {
    float height = 0.0f;
    float velocity = 0.0f;
    bool isJumping = false;

    void jump() {
        if (!isJumping) {
            velocity = kJumpVelocity;
            isJumping = true;
        }
    }

    void update() {
        if (!isJumping) {
            return;
        }

        height += velocity;
        velocity += kGravity;

        if (height <= 0.0f) {
            height = 0.0f;
            velocity = 0.0f;
            isJumping = false;
        }
    }
};

void clearScreen() {
    std::cout << "\x1B[2J\x1B[H";
}

void drawBuffer(const std::vector<std::string>& buffer) {
    clearScreen();
    for (const auto& line : buffer) {
        std::cout << line << '\n';
    }
}

void drawText(std::vector<std::string>& buffer, int row, int col, const std::string& text) {
    if (row < 0 || row >= static_cast<int>(buffer.size())) {
        return;
    }

    for (size_t index = 0; index < text.size(); ++index) {
        const int x = col + static_cast<int>(index);
        if (x >= 0 && x < static_cast<int>(buffer[row].size())) {
            buffer[row][x] = text[index];
        }
    }
}

void drawGround(std::vector<std::string>& buffer) {
    for (int col = 0; col < kScreenWidth; ++col) {
        buffer[kGroundRow][col] = '-';
    }
}

void drawDino(std::vector<std::string>& buffer, const Dino& dino, bool runFrame) {
    const int baseRow = kGroundRow - 4 - static_cast<int>(std::round(dino.height));

    drawText(buffer, baseRow, kDinoX, "  __");
    drawText(buffer, baseRow + 1, kDinoX, " / _)");
    drawText(buffer, baseRow + 2, kDinoX, "/ /  ");
    drawText(buffer, baseRow + 3, kDinoX, "\\_\\__");

    if (runFrame) {
        drawText(buffer, baseRow + 4, kDinoX + 1, " / ");
    } else {
        drawText(buffer, baseRow + 4, kDinoX + 1, "/  ");
    }
}

void processInput(bool& running, Dino& dino) {
    while (_kbhit()) {
        const int key = _getch();
        if (key == 'q' || key == 'Q') {
            running = false;
        } else if (key == ' ' || key == 'w' || key == 'W' || key == 72) {
            dino.jump();
        }
    }
}

}  // namespace

int main() {
    Dino dino;
    bool running = true;
    std::size_t frameCount = 0;

    std::cout << "CLI Dino\n";
    std::cout << "Space/W to jump, Q to quit.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    while (running) {
        const auto frameStart = std::chrono::steady_clock::now();

        processInput(running, dino);
        if (!running) {
            break;
        }

        dino.update();

        std::vector<std::string> buffer(kScreenHeight, std::string(kScreenWidth, ' '));
        drawGround(buffer);
        drawText(buffer, 1, 2, "Space/W = jump   Q = quit");
        drawDino(buffer, dino, (frameCount / 6) % 2 == 0);

        drawBuffer(buffer);

        ++frameCount;

        const auto frameDuration = std::chrono::steady_clock::now() - frameStart;
        const auto targetFrameTime = std::chrono::milliseconds(45);
        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameDuration);
        }
    }

    clearScreen();
    std::cout << "Game closed.\n";
    return 0;
}