#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include "define.h"
#include "singleton.hpp"

#define MAX_LOG_LENGTH 1024  //单个消息最大长度

class Log
{
    public:
        enum LOG_LEVEL {
            FATAL = 0,
            ERROR = 1,
            WARN = 2,
            INFO = 3,
            DEBUG = 4,
            TRACE = 5,
        };

    public:
        Log();
        ~Log();
        bool OpenLog(const std::string& log_folder, 
                     const std::string &log_level);
        bool CloseLog();
        void WriteLog(LOG_LEVEL log_level, const char *fmt, ...);

        int get_fd() {return _log_fd; }

    private:
        enum LOG_LEVEL get_log_level(const std::string &log_level);

    public:
        int _log_fd;

        LOG_LEVEL _log_level;
        std::string _log_level_name[6];
};

typedef Singleton<Log> SingletonLog;

#define log_fatal(fmt, args...) \
    SingletonLog::GetInstance()->WriteLog(Log::FATAL, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_error(fmt, args...) \
    SingletonLog::GetInstance()->WriteLog(Log::ERROR, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_warn(fmt, args...) \
    SingletonLog::GetInstance()->WriteLog(Log::WARN, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_info(fmt, args...) \
    SingletonLog::GetInstance()->WriteLog(Log::INFO, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_debug(fmt, args...) \
    SingletonLog::GetInstance()->WriteLog(Log::DEBUG, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#define log_trace(fmt, args...) \
    SingletonLog::GetInstance()->WriteLog(Log::TRACE, "[%s(%d)]: " fmt, __FILE__, __LINE__, ##args)

#endif //_LOG_H_
