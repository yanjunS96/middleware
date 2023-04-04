//
// Created by songyanjun on 2023/3/27.
//

#ifndef MVP_MODULES_DOIP_CONFIG_HPP
#define MVP_MODULES_DOIP_CONFIG_HPP

#include <array>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include "yaml_parse.hpp"
#include "doip_common/DoIpHead.hpp"

namespace motovis{
    namespace doip{
        struct DoIpInfo
        {
            int         ServerPort{cServerPort};
            int         ClientPort{cClientPort};
            uint8_t     ProtocolVersion{0x02};
            uint8_t     DoIpAnnounceNum{0x03};
            uint16_t    LocalAddress{0x0017};
            int    doipMaxRequestBytes{64};

            uint8_t     FurtherActionReq{0x00};
            std::array<uint8_t,cIdSize> Gid;
            std::array<uint8_t,cIdSize> Eid;
            std::string                 Vin{"00000000000000000"};

            uint8_t     doIPInitialVehicleAnnouncementTime{0x00}; //ms
            uint8_t     doIPVehicleAnnouncementInterval{0x01}; //ms

            DoIpInfo()
            :Eid([]()->std::array<uint8_t, cIdSize>{
                std::array<uint8_t, cIdSize> eid{0};
                {
                    struct ifreq ifr;

                    int fd = socket(AF_INET, SOCK_DGRAM, 0);
                    ifr.ifr_addr.sa_family = AF_INET;
//                    strncpy((char*)ifr.ifr_name, "eth0", IFNAMSIZ-1);
                    strncpy((char*)ifr.ifr_name, "eth4", IFNAMSIZ-1);
                    ioctl(fd, SIOCGIFHWADDR, &ifr);
                    close(fd);

                    unsigned char* mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
                    for(int i = 0; i < std::min(cIdSize, (std::size_t)strlen((char *)mac)); i++)
                    {
                        eid[i] = mac[i];
                    }
                }
                return eid;
            }())
            {
//                MLOG_INFO("info value!");
            }

            void SetEid(const uint64_t & id)
            {

                Eid[0] = (id & 0xFF);
                Eid[1] = ((id>>8) & 0xFF);
                Eid[2] = ((id>>16) & 0xFF);
                Eid[3] = ((id>>24) & 0xFF);
                Eid[4] = ((id>>32) & 0xFF);
                Eid[5] = ((id>>40) & 0xFF);
                MLOG_INFO("get Eid :{}, {} {} {} {} {} {}",id,Eid[0],Eid[1],Eid[2],Eid[3],Eid[4],Eid[5]);
            }

            void SetGid(const uint64_t & id)
            {
                Gid[0] = (id & 0xFF);
                Gid[1] = ((id>>8) & 0xFF);
                Gid[2] = ((id>>16) & 0xFF);
                Gid[3] = ((id>>24) & 0xFF);
                Gid[4] = ((id>>32) & 0xFF);
                Gid[5] = ((id>>40) & 0xFF);
                MLOG_INFO("get Gid :{}, {} {} {} {} {} {}",id,Gid[0],Gid[1],Gid[2],Gid[3],Gid[4],Gid[5]);
            }
        };

        using DoIpInfoPtr=std::shared_ptr<DoIpInfo>;

        class DoIpConfig: public spi::YamlParse
        {
        public:
            DoIpConfig(DoIpInfoPtr _info):YamlParse(), m_info(nullptr != _info? _info: throw std::out_of_range("Invalid DoIpInfoPtr"))
            {}
            virtual ~DoIpConfig()
            {
                MLOG_INFO("DoIp parse finish!");
            }

            DoIpInfoPtr parseYaml(const std::string& file = "./SpiConfig.yaml")
            {
                DoIpConfig::readYaml(file, "spi");
                return m_info;
            }
        protected:
            virtual bool parseConfig(const YAML::Node& config_) override
            {
                if (config_["doip"].IsDefined())
                {
                    auto yamlDoIp = config_["doip"];
                    if (yamlDoIp["ServerPort"].IsDefined()) { m_info->ServerPort = yamlDoIp["ServerPort"].as<int>(); }
                    if (yamlDoIp["ClientPort"].IsDefined()) { m_info->ClientPort = yamlDoIp["ClientPort"].as<int>(); }
                    if (yamlDoIp["ProtocolVersion"].IsDefined()) { m_info->ProtocolVersion = yamlDoIp["ProtocolVersion"].as<uint8_t>(); }
                    if (yamlDoIp["LocalAddress"].IsDefined()) { m_info->LocalAddress = [&]()->uint16_t {
                            auto address = yamlDoIp["LocalAddress"].as<std::string>();
                            std::string::size_type index = address.find("0x");
                            uint16_t ret_num = 0x0017;
                            if (index != std::string::npos)
                            {
                                ret_num = 0;
                                for (int i = index; i < address.size(); ++i) {
                                    if (('0'<=address[i]) && ('9'>= address[i]))
                                    {
                                        ret_num = ret_num<<8;
                                        ret_num += (address[i] - '0');
                                    }
                                    else
                                    {
                                        return 0x0017;
                                    }
                                }
                            }
                            return ret_num;
                        }(); }
                    if (yamlDoIp["DoIpAnnounceNum"].IsDefined()) { m_info->DoIpAnnounceNum = yamlDoIp["DoIpAnnounceNum"].as<uint8_t>(); }
                    if (yamlDoIp["FurtherActionReq"].IsDefined()) { m_info->FurtherActionReq = yamlDoIp["FurtherActionReq"].as<uint8_t>(); }
                    if (yamlDoIp["Gid"].IsDefined()) {
                        auto Gid = yamlDoIp["Gid"].as<uint64_t>();
                        m_info->SetGid(Gid); }
                    if (yamlDoIp["Eid"].IsDefined()) {
                        auto Eid = yamlDoIp["Eid"].as<uint64_t>();
                        m_info->SetEid(Eid); }
                    if (yamlDoIp["Vin"].IsDefined())
                    {
                        auto vin = yamlDoIp["Vin"].as<std::string>();
                        if (cVinSize == vin.size())
                        {
                            m_info->Vin = vin;
                        }
                        else
                        {
                            //使用默认值
                            MLOG_ERROR("vin_size: {} is not 17,use default vin",vin.size());
                        }
                    }
                    if (yamlDoIp["doipMaxRequestBytes"].IsDefined()) { m_info->doipMaxRequestBytes = yamlDoIp["doipMaxRequestBytes"].as<int>(); }
                    MLOG_INFO("DoIpYamlConfig,ServerPort:{},ClientPort:{},ProtocolVersion:{},LocalAddress:{},"
                              "DoIpAnnounceNum:{},FurtherActionReq:{},{},Vin:{}"
                            ,m_info->ServerPort ,m_info->ClientPort, m_info->ProtocolVersion,m_info->LocalAddress
                            ,m_info->DoIpAnnounceNum,m_info->FurtherActionReq,m_info->doipMaxRequestBytes
                            ,m_info->Vin);
                }
                return true;
            }
        private:
            DoIpInfoPtr m_info;
        };
    }
}

#endif //MVP_MODULES_DOIP_CONFIG_HPP
