
#ifndef MVP_MODULES_DOIP_PARSE_HPP
#define MVP_MODULES_DOIP_PARSE_HPP

#include <functional>

//#include "doip_common/DoIpHead.hpp"

#include "message/alive_check_request.hpp"
#include "message/alive_check_response.hpp"
#include "message/diag_message.hpp"
#include "message/diag_message_ack.hpp"
#include "message/diag_message_nack.hpp"
#include "message/eid_vehicle_id_request.hpp"
#include "message/entity_status_request.hpp"
#include "message/entity_status_response.hpp"
#include "message/generic_nack.hpp"

#include "message/power_mode_request.hpp"
#include "message/power_mode_response.hpp"
#include "message/routing_activation_request.hpp"
#include "message/routing_activation_response.hpp"
#include "message/vehicle_id_response.hpp"
#include "message/vehicle_id_request.hpp"
#include "message/vin_vehicle_id_request.hpp"


namespace motovis{
    namespace doip
    {
        enum class SOCKET_TYPE:uint8_t
        {
            UDP,
            TCP
        };


        class DoIpParse
        {
            using ParseDoIpFunc=std::function<void(SerialVecBuff &)>;
        public:
            DoIpParse(const DoIpInfoPtr _info):p_info(nullptr != _info? _info: throw std::out_of_range("Invalid DoIpInfoPtr")) {}
            virtual ~DoIpParse() = default;

            bool parseResponse(const SerialVecBuff & recvVal, SerialVecBuff & sendVal, SOCKET_TYPE type) noexcept
            {
                MLOG_INFO("recv_len:{}", recvVal.size());
                DoIpHeader header;
                header << recvVal;
                if ((header.mProtocolVersion != (header.mverseProtocolVersion ^ 0xFF)) ||
                        (p_info->ProtocolVersion != header.mProtocolVersion))
                {
                    MLOG_WARN("mProtocolVersion failed:{} {} {}", header.mProtocolVersion, header.mverseProtocolVersion, p_info->ProtocolVersion);
                    GenericNack(header.mProtocolVersion, GenericNackType::InvalidProtocolVersion).Serialize(sendVal);
                    return false;
                }

                auto messageItr = m_MessageContrl.find(header.mPayloadType);
                if (messageItr != m_MessageContrl.end())
                {
                    GenericNackType nackType;
                    if (std::get<0>(messageItr->second)->TryDeserialize(recvVal,nackType))
                    {
                        //解析DoIp报文,响应回复
                        std::get<1>(messageItr->second)(sendVal);
                        return true;
                    }
                    else
                    {
                        GenericNack(header.mProtocolVersion, nackType).Serialize(sendVal);
                        return false;
                    }
                }
                else
                {
                    GenericNack(header.mProtocolVersion, GenericNackType::UnsupportedPayloadType).Serialize(sendVal);
                    return false;
                }
            }

            void CreateGenericNack(GenericNackType nackCode, uint8_t protocolVersion, SerialVecBuff &response) const noexcept
            {
                GenericNack _genericNack(protocolVersion, nackCode);
                _genericNack.Serialize(response);
            }

        protected:
        private:
            DoIpInfoPtr p_info;

            const std::map<PayloadType, std::tuple<MessPtr, ParseDoIpFunc>> m_MessageContrl{
                    {PayloadType::GenericNegativeAcknowledgement, {std::make_shared<GenericNack>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::VehicleIdRequest, {std::make_shared<VehicleIdRequest>(),[&](SerialVecBuff & buff){
                        VehicleIdResponse(p_info->ProtocolVersion
                                ,p_info->Vin
                                ,p_info->LocalAddress
                                ,p_info->Eid
                                ,p_info->Gid
                                ,p_info->FurtherActionReq).Serialize(buff);
                    }}},
                    {PayloadType::VehicleIdRequestWithEid, {std::make_shared<EidVehicleIdRequest>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::VehicleIdRequestWithVin, {std::make_shared<VinVehicleIdRequest>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::VehicleAnnoucementIdResponse, {std::make_shared<VehicleIdResponse>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::RoutingActivationRequest, {std::make_shared<RoutingActivationRequest>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::RoutingActivationResponse, {std::make_shared<RoutingActivationResponse>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::AliveCheckRequest, {std::make_shared<AliveCheckRequest>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::AliveCheckResponse, {std::make_shared<AliveCheckResponse>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DoipEntityStatusRequest, {std::make_shared<EntityStatusRequest>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DoipEntityStatusResponse, {std::make_shared<EntityStatusResponse>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DiagPowerModeInfoRequest, {std::make_shared<PowerModeRequest>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DiagPowerModeInfoResponse, {std::make_shared<PowerModeResponse>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DiagMessage, {std::make_shared<DiagMessage>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DiagMessagePositiveAcknowledgement, {std::make_shared<DiagMessageAck>(),[&](SerialVecBuff & buff){}}},
                    {PayloadType::DiagMessageNegativeAcknowledgement, {std::make_shared<DiagMessageNack>(),[&](SerialVecBuff & buff){}}},
            }; //message map 表
        };
        using DoIpParsePtr=std::shared_ptr<DoIpParse>;
    }
}

#endif //MVP_MODULES_DOIP_PARSE_HPP
