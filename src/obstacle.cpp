#include "obstacle.h"

#include "renderer.h"

namespace cli_dyno {

namespace {

const std::vector<std::string> kCactusSprite = {
    "\\|/",
    " | ",
    " | "
};

const std::vector<std::string> kBirdSprite = {
    "~v~"
};

}

const std::vector<std::string>& spriteFor(ObstacleKind kind) {
    switch (kind) {
        case ObstacleKind::Cactus:   return kCactusSprite;
        case ObstacleKind::BirdLow:  return kBirdSprite;
        case ObstacleKind::BirdHigh: return kBirdSprite;
    }
    __builtin_unreachable();
}

int topRowFor(ObstacleKind kind) {
    switch (kind) {
        case ObstacleKind::Cactus:   return kGroundRow - 3;
        case ObstacleKind::BirdLow:  return kGroundRow - 3;
        case ObstacleKind::BirdHigh: return kGroundRow - 6;
    }
    __builtin_unreachable();
}

HitBox hitBoxFor(const Obstacle& obs) {
    const auto& sprite = spriteFor(obs.kind);
    const int top = topRowFor(obs.kind);
    const int height = static_cast<int>(sprite.size());
    const int width = static_cast<int>(sprite.front().size());
    return HitBox{top, static_cast<int>(obs.x), height, width};
}

}
