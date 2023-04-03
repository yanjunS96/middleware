//
// Created by songyanjun on 2023/3/2.
//

#ifndef MVP_MODULES_SPI_CONFIG_HPP
#define MVP_MODULES_SPI_CONFIG_HPP
#include "yaml_parse.hpp"
#include "spi_port.h"
namespace motovis{
    namespace spi{
#define DEV_SPI1 "/dev/spidev1.0"
#define DEV_SPI2 "/dev/spidev2.0"
#define NODE_SPI1 "spi_1"
#define NODE_SPI2 "spi_2"

        enum class SpiMold
        {
            SPI1 = 0,
            SPI2
        };
        struct SpiInfo{
            std::string m_versionFile{"./motovis_version"};
            uint8_t 	m_cSpiMode{SPI_MODE_0};
            uint8_t 	m_cBits{8};
            uint32_t    m_nSpeed{1000000};
            uint16_t 	m_nDelay{0};
            int32_t     m_fd{-1};
            uint32_t    m_capacity{512};
            uint8_t     m_synctime{20};
        };

        class SpiConfig : public YamlParse
        {
        public:
            explicit SpiConfig(const SpiMold spi_mode)
                    :YamlParse(),
                    m_spiMold([spi_mode]()->std::string{
                        if(SpiMold::SPI1 == spi_mode){return NODE_SPI1;}
                        else if(SpiMold::SPI2 == spi_mode){return NODE_SPI2;}
                        else {return NODE_SPI1;}
                    }())
            {}
            virtual ~SpiConfig(){}

            SpiInfo parseYaml(const std::string& file = "./SpiConfig.yaml")
            {
                SpiConfig::readYaml(file, "spi");
                return m_spiInfo;
            }

        protected:
            virtual bool parseConfig(const YAML::Node& config_) override
            {
                if (!config_["verfile"].IsNull())
                {
                    m_spiInfo.m_versionFile = config_["verfile"].as<std::string>();
                }

                if (!config_[m_spiMold].IsNull())
                {
                    auto spiconfig = config_[m_spiMold];
                    if (!spiconfig["speed"].IsNull()) { m_spiInfo.m_nSpeed = spiconfig["speed"].as<uint32_t>(); }
                    if (!spiconfig["bits"].IsNull()) { m_spiInfo.m_cBits = spiconfig["bits"].as<uint32_t>(); }
                    if (!spiconfig["delay"].IsNull()) { m_spiInfo.m_nDelay = spiconfig["delay"].as<uint32_t>(); }
                    if (!spiconfig["mode"].IsNull()) {
                        m_spiInfo.m_cSpiMode = m_spiMode.find(spiconfig["mode"].as<std::string>())->second;
                    }
                    if (!spiconfig["dev"].IsNull()) {
                        auto dev = spiconfig["dev"].as<std::string>();
                        //spi初始化
                        m_spiInfo.m_fd = SpiMessInit(dev.data(),
                                                     m_spiInfo.m_cSpiMode,
                                                     m_spiInfo.m_cBits,
                                                     m_spiInfo.m_nSpeed);
                    }
                    if (!spiconfig["translen"].IsNull()) { m_spiInfo.m_capacity = spiconfig["translen"].as<uint32_t>(); }
                    MLOG_INFO("parseYamlConfig,{} speed:{},bits:{},delay:{}, mode:{}, m_fd:{}"
                        ,m_spiInfo.m_capacity ,m_spiInfo.m_nSpeed, m_spiInfo.m_cBits,m_spiInfo.m_nDelay
                        ,m_spiInfo.m_cSpiMode,m_spiInfo.m_fd);
                }

                return true;
            }
            const std::map<std::string, uint8_t> m_spiMode{
                    {"SPI_MODE_0", SPI_MODE_0},
                    {"SPI_MODE_1", SPI_MODE_1},
                    {"SPI_MODE_2", SPI_MODE_2},
                    {"SPI_MODE_3", SPI_MODE_3}
            };
        private:
            std::string m_spiMold;
            SpiInfo m_spiInfo;
        };
    }
}
#endif //MVP_MODULES_SPI_CONFIG_HPP

