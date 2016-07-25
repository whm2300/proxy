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
    return true;
}

void Config::PrintConfig() {
    log_info("---------config begin---------------");
    log_info("pidfile:%s", _pidfile_path.c_str());
    log_info("is daemon:%d", _is_daemon);
    log_info("log level:%s", _log_level.c_str());
    log_info("log folder:%s", _log_folder.c_str());
    log_info("ip:%s, port:%d", _ip.c_str(), _port);
    log_info("---------config end-----------------");
}
