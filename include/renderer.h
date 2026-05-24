#ifndef CLI_DYNO_RENDERER_H
#define CLI_DYNO_RENDERER_H

#include <string>
#include <vector>

#include "dino.h"
#include "obstacle.h"

namespace cli_dyno {

constexpr int kScreenWidth = 70;
constexpr int kScreenHeight = 20;
constexpr int kGroundRow = kScreenHeight - 2;
constexpr int kDinoX = 6;

class Renderer {
public:
    Renderer();

    void clear();
    void drawText(int row, int col, const std::string& text);
    void drawGround();
    void drawDino(const Dino& dino, int animFrame);
    void drawObstacle(const Obstacle& obs);
    void drawHud(int score, int highScore);
    void drawGameOver(int score, int highScore);
    void flush();

private:
    std::vector<std::string> buffer_;
};

}

#endif
