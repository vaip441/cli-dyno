#ifndef CLI_DYNO_INPUT_H
#define CLI_DYNO_INPUT_H

#include <termios.h>

namespace cli_dyno {

class TerminalRawMode {
public:
    TerminalRawMode();
    ~TerminalRawMode();
    TerminalRawMode(const TerminalRawMode&) = delete;
    TerminalRawMode& operator=(const TerminalRawMode&) = delete;

private:
    termios original_{};
    bool enabled_ = false;
};

bool keyAvailable();
int readKey();

}

#endif
