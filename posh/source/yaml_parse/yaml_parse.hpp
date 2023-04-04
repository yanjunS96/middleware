//
// Created by songyanjun on 2023/3/2.
//

#ifndef MVP_MODULES_YAML_PARSE_HPP
#define MVP_MODULES_YAML_PARSE_HPP

#include <iostream>
#include <regex>
#include <linux/spi/spidev.h>
#include "mlog.h"
#include "yaml-cpp/yaml.h"

namespace motovis{
    namespace spi{

        struct YamlParse{
        public:
            YamlParse() noexcept
            {}
            virtual ~YamlParse() noexcept = default;

        protected:
            void readYaml(const std::string& _file, const std::string& name)
            {
                try
                {
                    YAML::Node _config = YAML::LoadFile(_file);
                    for (auto config : _config)
                    {
                        if (!config["module_name"].IsDefined())
                        {
                            MLOG_ERROR("YAML have not module_name node!");
                        }
                        else
                        {
                            std::string module_name = config["module_name"].as<std::string>();
                            if(module_name == name && config["config"].IsDefined())
                            {
                                parseConfig(config["config"]);
                            }
                        }
                    }
                }
                catch (YAML::BadFile &e)
                {
                    MLOG_ERROR("config file load error! use default config!{}",e.what());
                }
                catch (YAML::ParserException &e) {
                    MLOG_ERROR("YAML::ParserException failed,{}!", e.what());
                }
                catch (YAML::InvalidNode &e)
                {
                    MLOG_ERROR("YAML::InvalidNode failed,{}!", e.what());
                }
                catch (YAML::TypedBadConversion<unsigned long> &e)
                {
                    MLOG_ERROR("YAML::TypedBadConversion failed,{}!", e.what());
                }
                catch (YAML::TypedBadConversion<std::string> &e)
                {
                    MLOG_ERROR("YAML::TypedBadConversion failed,{}!", e.what());
                }
                catch (YAML::TypedBadConversion<int> &e)
                {
                    MLOG_ERROR("YAML::TypedBadConversion failed,{}!", e.what());
                }
            }
            virtual bool parseConfig(const YAML::Node&) = 0;

            size_t ParseFileSize(const std::string &file_size)
            {
                std::regex rx("\\s*(\\d+|\\d+[.]|\\d?[.]\\d+)\\s*((ms|MS|us|US|s|S)[]?)\\s*");
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

        public:
//            std::string versionFile;
//            SpiInfo     m_info;
//            YAML::Node m_config;
        };
    }
}

#endif //MVP_MODULES_YAML_PARSE_HPP
