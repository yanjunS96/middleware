//
// Created by syj on 2023/2/23.
//

#ifndef MIDDLEWARE_CMD_LINE_PARSE_HPP
#define MIDDLEWARE_CMD_LINE_PARSE_HPP
#include "logger/mlog.h"

namespace middleware{
    namespace posh{
        struct CmdLineArgs_t{
            hoofs::log::LogLevel logLevel{hoofs::log::LogLevel::WARN};
            std::string configFilePath;
        };

        class CmdLineParser{
        public:
            CmdLineParser() noexcept = default;
            virtual ~CmdLineParser() noexcept = default;
            CmdLineParser(const CmdLineParser&) = delete;
            CmdLineParser(CmdLineParser&&) = delete;
            CmdLineParser& operator=(const CmdLineParser&) = delete;
            CmdLineParser& operator=(CmdLineParser&) = delete;

        protected:
            hoofs::log::LogLevel m_logerLevel{hoofs::log::LogLevel::WARN};
        };
    }
}

#endif //MIDDLEWARE_CMD_LINE_PARSE_HPP
