#ifndef __H_MIDDLEWARE_LOGGER_IMPL_H__
#define __H_MIDDLEWARE_LOGGER_IMPL_H__
#include <iostream>
#include <string>
#include "macros.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
//#include "yaml-cpp/yaml.h"
#include "cpptoml/cpptoml.h"
namespace middleware
{
    namespace hoofs{
        namespace log
        {
            class Logger final
            {
            public:
                ~Logger() = default;
                template <typename... Args>
                void Debug(const char *file, const int line, const char *func, Args &&... args);
                template <typename... Args>
                void Info(const char *file, const int line, const char *func, Args &&... args);
                template <typename... Args>
                void Warn(const char *file, const int line, const char *func, Args &&... args);
                template <typename... Args>
                void Error(const char *file, const int line, const char *func, Args &&... args);
                template <typename... Args>
                void Critical(const char *file, const int line, const char *func, Args &&... args);
                void Init(const std::string& config_file, const std::string name = "default");
            private:
                template <typename... Args>
                void Log(const char *file, const int line, const char *func,
                         const spdlog::level::level_enum level, Args &&... args);
                size_t ParseFileSize(const std::string &file_size);
                void SetLogLevel(const std::shared_ptr<spdlog::sinks::sink> &sink,
                                 const std::string &log_level);
//            void LoadConfig(const YAML::Node& config, const std::string name);
                void LoadConfig(const std::shared_ptr<cpptoml::table>& config, const std::string name);
            private:
                std::shared_ptr<spdlog::logger> logger_{nullptr};
            DECLARE_SINGLETON(Logger)
            };
            template <typename... Args>
            void Logger::Log(const char *file, const int line, const char *func,
                             const spdlog::level::level_enum level, Args &&... args)
            {
                try {
                    logger_->log(spdlog::source_loc{file, line, func}, level,
                                 std::forward<Args>(args)...);
                }
                catch (spdlog::spdlog_ex & ex)
                {
                    SPDLOG_WARN("Log spdlog::spdlog_ex error! {}", ex.what());
                }
            }
            template <typename... Args>
            void Logger::Debug(const char *file, const int line, const char *func, Args &&... args)
            {
                try {
                    logger_->log(spdlog::source_loc{file, line, func}, spdlog::level::debug,
                                 std::forward<Args>(args)...);
//            logger_->flush_on(spdlog::level::debug);
                }
                catch (spdlog::spdlog_ex & ex)
                {
                    SPDLOG_WARN("debug spdlog::spdlog_ex error! {}", ex.what());
                }
            }
            template <typename... Args>
            void Logger::Info(const char *file, const int line, const char *func, Args &&... args)
            {
                try {
                    logger_->log(spdlog::source_loc{file, line, func}, spdlog::level::info,
                                 std::forward<Args>(args)...);
//            logger_->flush_on(spdlog::level::info);
                }
                catch (spdlog::spdlog_ex & ex)
                {
                    SPDLOG_WARN("info spdlog::spdlog_ex error! {}", ex.what());
                }
            }
            template <typename... Args>
            void Logger::Warn(const char *file, const int line, const char *func, Args &&... args)
            {
                try {
                    logger_->log(spdlog::source_loc{file, line, func}, spdlog::level::warn,
                                 std::forward<Args>(args)...);
//            logger_->flush_on(spdlog::level::warn);
                }
                catch (spdlog::spdlog_ex & ex)
                {
                    SPDLOG_WARN("warn spdlog::spdlog_ex error! {}", ex.what());
                }
            }
            template <typename... Args>
            void Logger::Error(const char *file, const int line, const char *func, Args &&... args)
            {
                try {
                    logger_->log(spdlog::source_loc{file, line, func}, spdlog::level::err,
                                 std::forward<Args>(args)...);
//            logger_->flush_on(spdlog::level::err);
                }
                catch (spdlog::spdlog_ex & ex)
                {
                    SPDLOG_WARN("err spdlog::spdlog_ex error! {}", ex.what());
                }
            }
            template <typename... Args>
            void Logger::Critical(const char *file, const int line, const char *func, Args &&... args)
            {
                try {
                    logger_->log(spdlog::source_loc{file, line, func}, spdlog::level::critical,
                                 std::forward<Args>(args)...);
//            logger_->flush_on(spdlog::level::critical);
                }
                catch (spdlog::spdlog_ex & ex)
                {
                    SPDLOG_WARN("critical spdlog::spdlog_ex error! {}", ex.what());
                }
            }
        } // namespace log
    }
}
#endif //__H_MOTOVIS_LOGGER_IMPL_H__
