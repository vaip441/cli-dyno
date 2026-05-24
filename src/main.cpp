#include "game.h"
#include "input.h"

int main() {
    cli_dyno::TerminalRawMode raw;
    cli_dyno::Game game;
    game.run();
    return 0;
}
