#include "input.h"

#include <cstdio>

#include <sys/select.h>
#include <unistd.h>

namespace cli_dyno {

TerminalRawMode::TerminalRawMode() {
    enabled_ = tcgetattr(STDIN_FILENO, &original_) == 0;
    if (!enabled_) {
        return;
    }

    termios raw = original_;
    raw.c_lflag &= static_cast<unsigned long>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
        enabled_ = false;
        return;
    }

    std::fputs("\x1B[?25l\x1B[2J\x1B[H", stdout);
    std::fflush(stdout);
}

TerminalRawMode::~TerminalRawMode() {
    if (enabled_) {
        std::fputs("\x1B[?25h\x1B[2J\x1B[H", stdout);
        std::fflush(stdout);
        tcsetattr(STDIN_FILENO, TCSANOW, &original_);
    }
}

bool keyAvailable() {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(STDIN_FILENO, &readSet);

    timeval timeout{};
    const int ready = select(STDIN_FILENO + 1, &readSet, nullptr, nullptr, &timeout);
    return ready > 0;
}

int readKey() {
    return std::getchar();
}

}
