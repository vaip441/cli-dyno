#ifndef CLI_DYNO_WORLD_H
#define CLI_DYNO_WORLD_H

#include <random>
#include <vector>

#include "dino.h"
#include "obstacle.h"

namespace cli_dyno {

constexpr float kSpeedStart = 1.4f;
constexpr float kSpeedMax = 4.0f;
constexpr float kSpeedScoreDivisor = 500.0f;
constexpr int kBirdScoreThreshold = 300;
constexpr int kMinSpawnGap = 18;
constexpr int kSpawnGapJitter = 22;
constexpr float kSpawnGapMinAtSpeed = 12.0f;
constexpr float kObstacleDespawnX = -10.0f;
constexpr const char* kHighScoreFile = "highscore.txt";

class World {
public:
    World();

    void reset();
    bool update(int dtMs);
    void jumpDino();
    void toggleDuck();
    void loadHighScore();
    void saveHighScore() const;
    bool updateHighScoreIfBeaten();

    const Dino& dino() const { return dino_; }
    const std::vector<Obstacle>& obstacles() const { return obstacles_; }
    int score() const { return score_; }
    int highScore() const { return highScore_; }

private:
    void spawnObstacle();
    bool collides() const;

    Dino dino_;
    std::vector<Obstacle> obstacles_;
    float speed_ = kSpeedStart;
    int score_ = 0;
    int highScore_ = 0;
    float distanceSinceSpawn_ = 0.0f;
    float nextSpawnGap_ = 0.0f;
    std::mt19937 rng_;
};

}

#endif
