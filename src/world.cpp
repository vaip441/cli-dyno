#include "world.h"

#include <algorithm>
#include <fstream>
#include <random>

#include "audio.h"
#include "renderer.h"

namespace cli_dyno {

namespace {

bool boxesOverlap(const HitBox& a, const HitBox& b) {
    if (a.col + a.width  <= b.col) return false;
    if (b.col + b.width  <= a.col) return false;
    if (a.row + a.height <= b.row) return false;
    if (b.row + b.height <= a.row) return false;
    return true;
}

}

World::World()
    : rng_(std::random_device{}()) {
    std::uniform_real_distribution<float> dist(kMinSpawnGap, kMinSpawnGap + kSpawnGapJitter);
    nextSpawnGap_ = dist(rng_);
    loadHighScore();
}

void World::reset() {
    dino_ = Dino{};
    obstacles_.clear();
    speed_ = kSpeedStart;
    score_ = 0;
    distanceSinceSpawn_ = 0.0f;
    std::uniform_real_distribution<float> dist(kMinSpawnGap, kMinSpawnGap + kSpawnGapJitter);
    nextSpawnGap_ = dist(rng_);
}

bool World::update(int dtMs) {
    dino_.update(dtMs);

    for (auto& obs : obstacles_) {
        obs.x -= speed_;
    }
    obstacles_.erase(
        std::remove_if(obstacles_.begin(), obstacles_.end(),
            [](const Obstacle& o) { return o.x < kObstacleDespawnX; }),
        obstacles_.end());

    distanceSinceSpawn_ += speed_;
    if (distanceSinceSpawn_ >= nextSpawnGap_) {
        spawnObstacle();
        distanceSinceSpawn_ = 0.0f;
        const float minGap = std::max(static_cast<float>(kMinSpawnGap), kSpawnGapMinAtSpeed / speed_);
        std::uniform_real_distribution<float> dist(minGap, minGap + kSpawnGapJitter);
        nextSpawnGap_ = dist(rng_);
    }

    ++score_;
    speed_ = std::min(kSpeedMax, kSpeedStart + score_ / kSpeedScoreDivisor);

    return collides();
}

void World::jumpDino() {
    if (dino_.jump()) {
        playJumpSound();
    }
}

void World::toggleDuck() {
    dino_.toggleDuck();
}

void World::spawnObstacle() {
    Obstacle obs;
    obs.x = static_cast<float>(kScreenWidth + 2);

    if (score_ <= kBirdScoreThreshold) {
        obs.kind = ObstacleKind::Cactus;
    } else {
        std::uniform_int_distribution<int> pick(0, 99);
        const int roll = pick(rng_);
        if (roll < 60) obs.kind = ObstacleKind::Cactus;
        else if (roll < 85) obs.kind = ObstacleKind::BirdLow;
        else obs.kind = ObstacleKind::BirdHigh;
    }

    obstacles_.push_back(obs);
}

bool World::collides() const {
    const HitBox dinoBox = dino_.hitBox();
    for (const auto& obs : obstacles_) {
        if (boxesOverlap(dinoBox, hitBoxFor(obs))) {
            return true;
        }
    }
    return false;
}

void World::loadHighScore() {
    std::ifstream in(kHighScoreFile);
    if (!in) {
        highScore_ = 0;
        return;
    }
    int value = 0;
    if (in >> value && value >= 0) {
        highScore_ = value;
    } else {
        highScore_ = 0;
    }
}

void World::saveHighScore() const {
    std::ofstream out(kHighScoreFile, std::ios::trunc);
    if (!out) return;
    out << highScore_ << '\n';
}

bool World::updateHighScoreIfBeaten() {
    if (score_ > highScore_) {
        highScore_ = score_;
        saveHighScore();
        return true;
    }
    return false;
}

}
