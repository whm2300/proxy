#include "config.h"
#include "server.h"
#include "util/log.h"

#include <stdio.h>
#include <unistd.h>

bool InitialConfig(const std::string &config_path) {
    CONFIG->ReadConfig(config_path);
    if (!SingletonLog::GetInstance()->InitialLog(CONFIG->get_log_folder(), 
                    CONFIG->get_log_level(), 100))
    {
        printf("open log fail\n");
        return false;
    }
    CONFIG->PrintConfig();
    return true;
}


int main(int argc, char *argv[]) {
    if (!InitialConfig("../conf/peer_proxy.conf")) {
        printf("initial config error");
        return -1;
    }
    if (CONFIG->is_daemon()) {
        daemon(1, 1);
    }

    Server *server = new Server;
    if (!server->InitServer()) {
        printf("initial server error");
        return -1;
    }
    if (!server->RunServer()) {
        printf("run server error");
        return -1;
    }
    delete server;

    return 0;
}
