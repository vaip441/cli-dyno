#include "renderer.h"

#include <cstdio>
#include <iostream>

namespace cli_dyno {

namespace {

constexpr int kHudRightPad = 1;

std::string padScore(int n) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%05d", n);
    return std::string(buf);
}

}

Renderer::Renderer()
    : buffer_(kScreenHeight, std::string(kScreenWidth, ' ')) {}

void Renderer::clear() {
    for (auto& line : buffer_) {
        line.assign(kScreenWidth, ' ');
    }
}

void Renderer::drawText(int row, int col, const std::string& text) {
    if (row < 0 || row >= static_cast<int>(buffer_.size())) {
        return;
    }
    for (size_t index = 0; index < text.size(); ++index) {
        const int x = col + static_cast<int>(index);
        if (x >= 0 && x < static_cast<int>(buffer_[row].size())) {
            buffer_[row][x] = text[index];
        }
    }
}

void Renderer::drawGround() {
    for (int col = 0; col < kScreenWidth; ++col) {
        buffer_[kGroundRow][col] = '-';
    }
}

void Renderer::flush() {
    std::string frame;
    frame.reserve(buffer_.size() * (kScreenWidth + 4) + 8);
    frame += "\x1B[H";
    for (size_t i = 0; i < buffer_.size(); ++i) {
        frame += buffer_[i];
        frame += "\x1B[K";
        if (i + 1 < buffer_.size()) frame += '\n';
    }
    std::cout.write(frame.data(), static_cast<std::streamsize>(frame.size()));
    std::cout.flush();
}

void Renderer::drawDino(const Dino& dino, int animFrame) {
    const int baseRow = dino.baseRow();

    if (dino.isDucking()) {
        drawText(kGroundRow - 1, kDinoX, "  __");
        if (animFrame % 2 == 0) {
            drawText(kGroundRow, kDinoX, "==/_ \\>");
        } else {
            drawText(kGroundRow, kDinoX, "==/_  \\>");
        }
        return;
    }

    drawText(baseRow, kDinoX, "  __");
    drawText(baseRow + 1, kDinoX, " / _)");
    drawText(baseRow + 2, kDinoX, "/ /  ");
    drawText(baseRow + 3, kDinoX, "\\_\\__");

    static constexpr const char* kRunCycle[4] = {"\\\\", "||", "//", "||"};
    const char* legs = dino.isJumping ? "/\\" : kRunCycle[animFrame % 4];
    drawText(baseRow + 4, kDinoX + 1, legs);
}

void Renderer::drawObstacle(const Obstacle& obs) {
    const auto& sprite = spriteFor(obs.kind);
    const int top = topRowFor(obs.kind);
    const int col = static_cast<int>(obs.x);
    for (size_t row = 0; row < sprite.size(); ++row) {
        drawText(top + static_cast<int>(row), col, sprite[row]);
    }
}

void Renderer::drawHud(int score, int highScore) {
    const std::string text = "HI " + padScore(highScore) + "  SCORE " + padScore(score);
    drawText(0, kScreenWidth - static_cast<int>(text.size()) - kHudRightPad, text);
}

void Renderer::drawGameOver(int score, int highScore) {
    const std::string line1 = "G A M E   O V E R";
    const std::string line2 = "score " + padScore(score) + "   best " + padScore(highScore);
    const std::string line3 = "R restart    Q quit";
    const int center = kScreenWidth / 2;
    drawText(kScreenHeight / 2 - 2, center - static_cast<int>(line1.size()) / 2, line1);
    drawText(kScreenHeight / 2,     center - static_cast<int>(line2.size()) / 2, line2);
    drawText(kScreenHeight / 2 + 2, center - static_cast<int>(line3.size()) / 2, line3);
}

}
