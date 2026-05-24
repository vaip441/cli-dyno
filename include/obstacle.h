#ifndef CLI_DYNO_OBSTACLE_H
#define CLI_DYNO_OBSTACLE_H

#include <string>
#include <vector>

#include "dino.h"

namespace cli_dyno {

enum class ObstacleKind { Cactus, BirdLow, BirdHigh };

struct Obstacle {
    ObstacleKind kind;
    float x;
};

const std::vector<std::string>& spriteFor(ObstacleKind kind);
int topRowFor(ObstacleKind kind);
HitBox hitBoxFor(const Obstacle& obs);

}

#endif
