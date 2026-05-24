#ifndef CLI_DYNO_DINO_H
#define CLI_DYNO_DINO_H

namespace cli_dyno {

constexpr float kGravity = -1.2f;
constexpr float kJumpVelocity = 4.0f;

struct HitBox {
    int row;
    int col;
    int height;
    int width;
};

struct Dino {
    float height = 0.0f;
    float velocity = 0.0f;
    bool isJumping = false;
    bool ducking = false;

    bool jump();
    void toggleDuck();
    void update(int dtMs);
    bool isDucking() const;
    int baseRow() const;
    HitBox hitBox() const;
};

}

#endif
