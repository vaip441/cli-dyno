#include "dino.h"

#include <cmath>

#include "renderer.h"

namespace cli_dyno {

bool Dino::jump() {
    if (isJumping) return false;
    velocity = kJumpVelocity;
    isJumping = true;
    ducking = false;
    return true;
}

void Dino::toggleDuck() {
    if (isJumping) return;
    ducking = !ducking;
}

void Dino::update(int dtMs) {
    (void)dtMs;
    if (!isJumping) return;
    height += velocity;
    velocity += kGravity;
    if (height <= 0.0f) {
        height = 0.0f;
        velocity = 0.0f;
        isJumping = false;
    }
}

bool Dino::isDucking() const {
    return ducking;
}

int Dino::baseRow() const {
    return kGroundRow - 4 - static_cast<int>(std::round(height));
}

HitBox Dino::hitBox() const {
    const int base = baseRow();
    if (isDucking()) {
        return HitBox{base + 2, kDinoX, 2, 6};
    }
    return HitBox{base, kDinoX, 4, 5};
}

}
