#ifndef CONFIG_H_
#define CONFIG_H_

#include "util/define.h"
#include "util/singleton.hpp"

#include <string>
#include <vector>

class Config {
    public:
        Config();
        ~Config();

        bool ReadConfig(const std::string &config_path);
        void PrintConfig();

        FORCEINLINE const std::string &get_pidfile_path() { return _pidfile_path; }
        FORCEINLINE const std::string &get_log_level() { return _log_level; }
        FORCEINLINE const std::string &get_log_folder() { return _log_folder; }
        FORCEINLINE const std::string &get_ip() { return _ip; }
        FORCEINLINE const uint16_t get_port() { return _port; }
        FORCEINLINE bool is_daemon() { return _is_daemon; }

    private:
        std::string _pidfile_path;
        std::string _log_level;
        std::string _log_folder;
        std::string _ip;
        uint16_t    _port;  //管理连接端口
        bool        _is_daemon;
};

typedef Singleton<Config> SingletonConfig;
#define CONFIG SingletonConfig::GetInstance()

#endif  // CONFIG_H_
