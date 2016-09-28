#include "log.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (!SingletonLog::GetInstance()->InitialLog("./", "info", 10)) {
        printf("error");
    }
    for (int i = 0; i < 1000000; ++i) {
        log_fatal("asdfsf");
        log_error("asdfasdfsadfa");
        log_warn("asdfasdfsadfa");
        log_info("asdfasdfsadfa");
        log_debug("asdfasdfsadfa");
        log_trace("asdfasdfsadfa");
    }
    return 0;
}
