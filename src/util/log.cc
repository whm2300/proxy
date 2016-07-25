#include "log.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <linux/limits.h>

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>

Log::Log():_log_fd(0) {
    _log_level_name[FATAL] = "[FATAL]";
    _log_level_name[ERROR] = "[ERROR]";
    _log_level_name[WARN] = "[WARN ]";
    _log_level_name[INFO] = "[INFO ]";
    _log_level_name[DEBUG] = "[DEBUG]";
    _log_level_name[TRACE] = "[TRACE]";
}

Log::~Log() {
}

bool Log::OpenLog(const std::string& log_folder, 
                           const std::string &log_level) {
    _log_level = get_log_level(log_level);
    time_t cur_time = time(NULL);
    struct tm *tm = localtime(&cur_time);

    char log_path[PATH_MAX];
    int len = sprintf(log_path, "%s/%04d%02d%02d%02d%02d%02d.log", 
                log_folder.c_str(), tm->tm_year + 1900, tm->tm_mon + 1, 
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    if (len < 0) {
        return false;
    }

    _log_fd = open(log_path, O_WRONLY | O_APPEND | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (_log_fd == -1) {
        return false;
    }
    return true;
}

enum Log::LOG_LEVEL 
Log::get_log_level(const std::string &log_level) {
    if (log_level == std::string("fatal")) {
        return FATAL;
    } else if (log_level == std::string("error")) {
        return ERROR;
    } else if (log_level == std::string("warn")) {
        return WARN;
    } else if (log_level == std::string("info")) {
        return INFO;
    } else if (log_level == std::string("debug")) {
        return DEBUG;
    } else if (log_level == std::string("trace")) {
        return TRACE;
    } else {
        return TRACE;
    }
}

bool Log::CloseLog() {
    if (close(_log_fd) == -1) {
        return false;
    }
    return true;
}

void Log::WriteLog(LOG_LEVEL log_level, const char *fmt, ...) {
    if (log_level > _log_level) {
        return ;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t cur_time = tv.tv_sec;
    struct tm *tm = localtime(&cur_time);

    char buffer[MAX_LOG_LENGTH];
    int len = sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d.%06d]%s", 
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
                tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec), 
                _log_level_name[log_level].c_str());
    if (len < 0) {
        return ;
    }

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer+len, MAX_LOG_LENGTH-len, fmt, ap);
    va_end(ap);

    len = strlen(buffer);
    char *end = buffer + len;
    *end ++ = '\n';
    *end = '\0';
    write(_log_fd, buffer, len + 1);
}
