#include "audio.h"

#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

namespace cli_dyno {

namespace {

void silenceChildOutput() {
    const int devnull = open("/dev/null", O_WRONLY);
    if (devnull >= 0) {
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
    }
}

void uploadSample(const char* path, const char* name) {
    const pid_t pid = fork();
    if (pid == 0) {
        silenceChildOutput();
        execlp("pactl", "pactl", "upload-sample", path, name,
               static_cast<char*>(nullptr));
        _exit(127);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    }
}

bool initAudio() {
    uploadSample("sounds/jump.wav", "jump");
    uploadSample("sounds/hitHurt.wav", "hitHurt");
    signal(SIGCHLD, SIG_IGN);
    return true;
}

void ensureInitialized() {
    static const bool initialized = initAudio();
    (void)initialized;
}

void playSample(const char* name) {
    ensureInitialized();
    const pid_t pid = fork();
    if (pid == 0) {
        silenceChildOutput();
        execlp("pactl", "pactl", "play-sample", name,
               static_cast<char*>(nullptr));
        _exit(127);
    }
}

}

void playJumpSound() {
    playSample("jump");
}

void playGameOverSound() {
    playSample("hitHurt");
}

}
