#include "config.h"
#include "util/log.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <stdio.h>

Config::Config():_is_daemon(false) {
}

Config::~Config() {
}

bool Config::ReadConfig(const std::string &config_path) {
    using namespace boost::property_tree;
    ptree pt;
    read_info(config_path.c_str(), pt);

    _pidfile_path = pt.get<std::string>("pidfile");
    std::string is_daemon = pt.get<std::string>("daemon");
    if (is_daemon == std::string("on")) {
        _is_daemon = true;
    } else {
        _is_daemon = false;
    }

    _log_level = pt.get<std::string>("log.level");
    _log_folder = pt.get<std::string>("log.folder");

    _ip = pt.get<std::string>("server.ip");
    _port = pt.get<int>("server.port");
    int port_count = pt.get<int>("server.port_count");
    _peer_local_pair.reserve(port_count);
    for (int i = 0; i < port_count; ++i) {
        PortPair port_pair;
        memset(&port_pair, 0, sizeof(port_pair));
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "server.port%d.peer", i);
        port_pair.peer_port = pt.get<int>(buffer);
        snprintf(buffer, sizeof(buffer), "server.port%d.local", i);
        port_pair.local_port = pt.get<int>(buffer);
        _peer_local_pair.push_back(port_pair);
    }
    return true;
}

void Config::PrintConfig() {
    log_info("---------config begin---------------");
    log_info("pidfile:%s", _pidfile_path.c_str());
    log_info("is daemon:%d", _is_daemon);
    log_info("log level:%s", _log_level.c_str());
    log_info("log folder:%s", _log_folder.c_str());
    log_info("ip:%s, port:%d", _ip.c_str(), _port);
    log_info("port pair count:%d", _peer_local_pair.size());
    for (size_t i = 0; i < _peer_local_pair.size(); ++i) {
        log_info("peer port:%d, local port:%d", 
                    _peer_local_pair[i].peer_port, 
                    _peer_local_pair[i].local_port);
    }
    log_info("---------config end-----------------");
}
