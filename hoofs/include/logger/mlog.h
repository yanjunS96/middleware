#ifndef __H_MIDDLEWARE_LOG_H__
#define __H_MIDDLEWARE_LOG_H__

#include "mlog_impl.h"

void LoggerInit(const std::string& config_file, const std::string appname = "default");

#define MLOG_INIT(config_, app_name) \
    LoggerInit(config_, app_name)

#define MLOG_DEBUG(...)                                                        \
    middleware::hoofs::log::Logger::Instance()->Debug(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define MLOG_INFO(...)                                                         \
    middleware::hoofs::log::Logger::Instance()->Info(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define MLOG_WARN(...)                                                         \
    middleware::hoofs::log::Logger::Instance()->Warn(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define MLOG_ERROR(...)                                                        \
    middleware::hoofs::log::Logger::Instance()->Error(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define MLOG_CRITICAL(...)                                                     \
    middleware::hoofs::log::Logger::Instance()->Critical(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif //__H_MOTOVIS_LOG_H__
