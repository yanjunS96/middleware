#include <regex>
#include <errno.h>
#include "mlog.h"
extern char *__progname;
namespace middleware
{
    namespace hoofs{
        namespace log
        {
#define MAX_LOGGER_SIZE (2<<10)
            /*spdlog::logger *Logger::logger_ = nullptr;
            YAML::Node Logger::config_;*/
            Logger::Logger()
            :logger_([]()->std::shared_ptr<spdlog::logger>{
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                //设置打印等级, 默认debug等级以上
                console_sink->set_level(spdlog::level::debug);
                //设置打印格式
                console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%P:%t] [%s:%#] [%!] %v");
                return std::make_shared<spdlog::logger>(spdlog::logger("multi_sink", console_sink));
            }())
            {}

            void Logger::Init(const std::string& config_file, const std::string name) //需要写文件才调用
            {
                try
                {
//                auto config_ = YAML::LoadFile(config_file); //加载yaml文件
                    auto config_ = cpptoml::parse_file(config_file.c_str());
                    LoadConfig(config_, name); //根据yaml文件匹配获取对应参数
//                    if(nullptr == logger_)
//                    {
//                        SPDLOG_WARN("config file  wrong format!");
//                        LoadDefaultConfig();
//                    }
                }
//            catch (YAML::BadFile &e)
                catch (const std::exception& parserException)
                {
                    //文件加载失败--可能没有对应的文件, 使用默认参数
                    SPDLOG_WARN("config file load error! using default config, {}", parserException.what());
//                    LoadDefaultConfig();
                }
            }
            //根据文件匹配参数
#if 0
            void Logger::LoadConfig(const YAML::Node& config, const std::string name)
        {
            for(auto & tem : config) {
                //循环取文件中的 "module_name" 参数
                auto module_name = tem["module_name"].as<std::string>();
                if(module_name == name) //根据name参数进行匹配文件中的"log"参数信息
                {
                    //申请 打印终端的sinks, 用的是 ansicolor_stdout_sink_mt, _mt为线程安全的, zu2项目多进程多线程, 需要使用线程安全
                    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                    // 根据配置文件中的 "console_level" 参数, 设置打印等级
                    SetLogLevel(console_sink, tem["log"]["console_level"].as<std::string>());
                    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%P:%t] [%s:%#] [%!] %v"); //设置打印格式

                    try {
                        spdlog::init_thread_pool(MAX_LOGGER_SIZE, 1);
                        //取出配置文件中的 对应参数 -> 文件路径, 名称, 文件最大size, 文件个数
                        auto root_path = tem["log"]["root_path"].as<std::string>();
                        auto file_size = tem["log"]["max_file_size"].as<std::string>();
                        auto max_size = ParseFileSize(file_size);
                        auto file_num = tem["log"]["max_file_num"].as<int>();
                        //根据配置文件中的 目录+名称+文件大小+文件数量, 申请文件 sink
                        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                                root_path + "/" + module_name + "_log.txt", max_size, file_num);
                        // 根据配置文件中的 "file_level" 参数,设置写入文件等级
                        SetLogLevel(file_sink, tem["log"]["file_level"].as<std::string>());
                        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%P:%t] [%s:%#] [%!] %v"); //设置写入格式
                        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink}; //打印sink和文件sink合并到vector
                        //为 logger_ 指针赋值
                        logger_.reset(std::make_shared<spdlog::async_logger>(
                                "multi_sink",
                                sinks.begin(),
                                sinks.end(),
                                spdlog::thread_pool(),
                                spdlog::async_overflow_policy::block).get());
                    }
                    catch (spdlog::spdlog_ex & ex)
                    {
                        SPDLOG_WARN("rotating_file_sink_mt spdlog::spdlog_ex error! {}", ex.what());
                        logger_.reset(std::make_shared<spdlog::logger>(spdlog::logger("multi_sink", console_sink)).get());
                    }
                }
            }
        }
#else
            void Logger::LoadConfig(const std::shared_ptr<cpptoml::table>& config, const std::string name)
            {
                if(nullptr == config)
                {
                    SPDLOG_WARN("config is nullptr!");
                    return;
                }
                auto config_infos = config->get_table_array("config_info");
                if(config_infos)
                {
                    for(auto config_info: *config_infos)
                    {
                        auto module_name = config_info->get_as<std::string>("module_name");
                        if(module_name && name == *module_name)
                        {
                            auto console_level = config_info->get_as<std::string>("console_level");
                            auto file_level = config_info->get_as<std::string>("file_level");
                            auto root_path = config_info->get_as<std::string>("root_path");
                            auto max_file_size = config_info->get_as<std::string>("max_file_size");
                            auto max_file_num = config_info->get_as<int>("max_file_num");
                            //申请 打印终端的sinks, 用的是 ansicolor_stdout_sink_mt, _mt为线程安全的, zu2项目多进程多线程, 需要使用线程安全
                            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                            // 根据配置文件中的 "console_level" 参数, 设置打印等级
                            if(console_level){SetLogLevel(console_sink, *console_level);}
                            else{SetLogLevel(console_sink, "debug");}
                            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%P:%t] [%s:%#] [%!] %v"); //设置打印格式

                            try {
                                spdlog::init_thread_pool(MAX_LOGGER_SIZE, 1);
                                //取出配置文件中的 对应参数 -> 文件路径, 名称, 文件最大size, 文件个数
                                std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink{nullptr};
                                if(root_path && max_file_size && file_level)
                                {
                                    auto max_size = ParseFileSize(*max_file_size);
//                                    SPDLOG_WARN("max_size:{}", max_size);
                                    //根据配置文件中的 目录+名称+文件大小+文件数量, 申请文件 sink
                                    file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                                            *root_path + "/" + *module_name + "_log.txt", max_size, *max_file_num);
                                }
                                if(nullptr != file_sink && file_level)
                                {
                                    SetLogLevel(file_sink, *file_level);
                                    // 根据配置文件中的 "file_level" 参数,设置写入文件等级
                                    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%P:%t] [%s:%#] [%!] %v"); //设置写入格式

                                    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink}; //打印sink和文件sink合并到vector
                                    //为 logger_ 指针赋值
                                    logger_.reset(
                                            new spdlog::async_logger(
                                                    "multi_sink",
                                                    sinks.begin(),
                                                    sinks.end(),
                                                    spdlog::thread_pool(),
                                                    spdlog::async_overflow_policy::block
                                                    )
                                            );
                                }
                            }
                            catch (spdlog::spdlog_ex & ex)
                            {
                                SPDLOG_WARN("rotating_file_sink_mt spdlog::spdlog_ex error! {}", ex.what());
                            }
                        }
                    }
                }
            }
#endif

            void Logger::SetLogLevel(const std::shared_ptr<spdlog::sinks::sink> &sink,
                                     const std::string &log_level)
            {
                if (log_level == "debug")
                {
                    sink->set_level(spdlog::level::debug);
                }
                else if (log_level == "info")
                {
                    sink->set_level(spdlog::level::info);
                }
                else if (log_level == "warn")
                {
                    sink->set_level(spdlog::level::warn);
                }
                else if (log_level == "error")
                {
                    sink->set_level(spdlog::level::err);
                }
                else if (log_level == "critical")
                {
                    sink->set_level(spdlog::level::critical);
                }
                else if (log_level == "off")
                {
                    sink->set_level(spdlog::level::off);
                }
                else
                {
                    sink->set_level(spdlog::level::off);
                }
            }
            size_t Logger::ParseFileSize(const std::string &file_size)
            {
                std::regex rx("\\s*(\\d+|\\d+[.]|\\d?[.]\\d+)\\s*((k|K|M|m|G|g)[Bb]?)\\s*");
                std::smatch m;
                if (!std::regex_match(file_size, m, rx))
                {
                    throw std::runtime_error("Unable to parse '" + file_size +
                                             "' as size.");
                }
                double d = std::stod(m[1].str());
                size_t mult = 1;
                if (m[3] == "")
                {
                    mult = 1;
                }
                else if (m[3] == "k" || m[3] == "K")
                {
                    mult = 1LL << 10;
                }
                else if (m[3] == "M" || m[3] == "m")
                {
                    mult = 1LL << 20;
                }
                else if (m[3] == "G" || m[3] == "G")
                {
                    mult = 1LL << 30;
                }
                else
                {
                    throw std::logic_error("Unhandled prefix '" + m[2].str() + "'.");
                }
                return d * mult;
            }

            void Logger::LogLevel(const log::LogLevel loglevel)
            {
                spdlog::level::level_enum log_level{spdlog::level::warn};
                switch (loglevel) {
                    case log::LogLevel::OFF:
                        log_level = spdlog::level::off;
                        break;
                    case log::LogLevel::CRITICAL:
                        log_level = spdlog::level::critical;
                        break;
                    case log::LogLevel::ERROR:
                        log_level = spdlog::level::err;
                        break;
                    case log::LogLevel::WARN:
                        log_level = spdlog::level::warn;
                        break;
                    case log::LogLevel::INFO:
                        log_level = spdlog::level::info;
                        break;
                    case log::LogLevel::DEBUG:
                        log_level = spdlog::level::debug;
                        break;
                    default:
                        break;
                }

                logger_->set_level(log_level);

                return;
            }
        } // namespace log
    }
} // namespace middleware

void LoggerInit(const std::string& config_file, const std::string appname)
{
    middleware::hoofs::log::Logger::Instance()->Init(config_file, appname);
}
