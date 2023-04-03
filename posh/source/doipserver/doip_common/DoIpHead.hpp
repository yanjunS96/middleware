
#ifndef MVP_MODULES_DOIPHEAD_HPP
#define MVP_MODULES_DOIPHEAD_HPP

#include <string.h>
#include "mlog.h"

namespace motovis{
    namespace doip{
#define DBYTE_COMBINE(L,H) (L+(H<<8))  /* 字符偏移合并 */

        constexpr int cServerPort{13400};
        constexpr int cClientPort{13401};

        constexpr std::size_t cHeaderSize{8}; //DoIp 协议头长度: 8
        constexpr std::size_t cIdSize{6}; /* EidSize & GidSize */
        constexpr std::size_t cVinSize{17};

        constexpr std::size_t cBuffSize{0x10000}; /* UdpBuff最大64k */

        using HeaderArray=std::array<uint8_t, cHeaderSize>;
        using BuffPtr=std::shared_ptr<uint8_t>;
        using SerialVecBuff=std::vector<uint8_t>;

        /// @brief Diagnostic message negative acknowledgement type
        enum class DiagNackType : uint8_t
        {
            InvalidSourceAddress = 0x02,    ///!< Invalid message source logical address
            InvalidTargetAddress = 0x03,    ///!< Invalid message target logical address
            TooLargeMessage = 0x04,         ///!< Message with payload length larger than 'DoIPMaxRequest'
            ReceiveBufferOverflow = 0x05,   ///!< Message with a size larger than the receive buffer
            UnreachableTarget = 0x06,       ///!< Unreachable message target due to lack of routing activation
            UnknownNetwork = 0x07,          ///!< Message rejection due to unknown network error
            TransportProtocolError = 0x08   ///!< Message rejection due to transport protocol error
        };

        /// @brief DoIP routing activation response code type
        enum class RoutingActivationResponseType : uint8_t
        {
            InvalidSourceAddress = 0x00,        ///<! Activation denied due to invalid tester logical address
            NoSocketAvailable = 0x01,           ///<! Activation denied due to unavailability of concurrent TCP sockets
            Busy = 0x02,                        ///<! Activation denied due to processing another request
            AlreadyRegisteredTester = 0x03,     ///<! Activation denied due to already tester registeration
            FailedAuthentication = 0x04,        ///<! Activation denied due to authentication missing/failure
            RejectedConfirmation = 0x05,        ///<! Actication denied due to confirmation rejection
            UnsupportedActivationType = 0x06,   ///<! Activation denied due to unsupported requested activation type
            NoSecureSocket = 0x07,              ///<! Activation denied due to TLS connection requirement
            Successful = 0x10,                  ///<! Activation was successful
            Pending = 0x11                      ///<! Activation is pending for confirmation
        };

        /// @brief DoIP node power mode type for a reliable communication with a tester
        enum class PowerModeType : uint8_t
        {
            NotReady = 0x00,    ///!< Not-ready power state
            Ready = 0x01,       ///!< Ready power state
            NotSupported = 0x02 ///!< Unsupported power mode request
        };

        /// @brief DoIP enity type
        enum class NodeType : uint8_t
        {
            DoipGateway = 0x00, ///!< DoIP gateway between the vehicle network and external testers
            DoipNode = 0x01     ///!< DoIP node within the vehicle network
        };

        enum class GenericNackType : uint8_t
        {
            InvalidProtocolVersion = 0x00,  ///!< Invalid DoIP protocol information
            UnsupportedPayloadType = 0x01,  ///!< Unsupported DoIP payload type
            ToolLargeMessage = 0x02,        ///!< Payload length exceeding the 'DoIPMaxRequestBytes'
            ReceiveBufferOverflow = 0x03,   ///!< Payload length exceeding the available buffer size
            InvalidPayloadLength = 0x04,    ///!< Invalid payload length for the specified payload type
        };

        /// @brief DoIP message payload type
        enum class PayloadType : uint16_t
        {
            GenericNegativeAcknowledgement = 0x0000,        ///!< Generic DoIP header negative acknowledgement
            VehicleIdRequest = 0x0001,                      ///!< Vehicle identification request
            VehicleIdRequestWithEid = 0x0002,               ///!< Vehicle identification request with specific EID
            VehicleIdRequestWithVin = 0x0003,               ///!< Vehicle identification request with specific VIN
            VehicleAnnoucementIdResponse = 0x0004,          ///!< Vehicle announcement identification response
            RoutingActivationRequest = 0x0005,              ///!< Routing activation phase request
            RoutingActivationResponse = 0x0006,             ///!< Routing activation phase response
            AliveCheckRequest = 0x0007,                     ///!< Alive check heart-beating request
            AliveCheckResponse = 0x0008,                    ///!< Alive check heart-beating response
            DoipEntityStatusRequest = 0x4001,               ///!< DoIP entity (i.e., node or gateway) status request
            DoipEntityStatusResponse = 0x4002,              ///!< DoIP entity status response
            DiagPowerModeInfoRequest = 0x4003,              ///!< Diagnostic power mode information request
            DiagPowerModeInfoResponse = 0x4004,             ///!< Diagnostic power mode information response
            DiagMessage = 0x8001,                           ///!< Diagnostic message (e.g., UDS)
            DiagMessagePositiveAcknowledgement = 0x8002,    ///!< Diagnostic message positive acknowledgement
            DiagMessageNegativeAcknowledgement = 0x8003     ///!< Diagnostic message negative acknowledgement
        };

        struct DoIpHeader
        {
            uint8_t mProtocolVersion{0x02};
            uint8_t mverseProtocolVersion{0};
            PayloadType mPayloadType;
            uint32_t mpayloadLength{0};

            DoIpHeader& operator<<(const SerialVecBuff & data)
            {
                memset(this, 0, cHeaderSize);
                if (cHeaderSize <= data.size())
                {
                    mProtocolVersion = data.at(0);
                    mverseProtocolVersion = data.at(1);
                    if (mProtocolVersion == (mverseProtocolVersion ^ 0xFF))
                    {
                        mPayloadType = static_cast<PayloadType>(DBYTE_COMBINE(data.at(2), data.at(3)));
                    }
                    mpayloadLength = DBYTE_COMBINE(data.at(4),
                                                   DBYTE_COMBINE(data.at(5),
                                                                 DBYTE_COMBINE(data.at(6), data.at(7))));
                    
                    MLOG_INFO("DoIpHeader: {} {} {} {}"
                              ,mProtocolVersion,mverseProtocolVersion
                              ,(uint16_t)mPayloadType,mpayloadLength
                              );
                }
                else
                {
                    MLOG_ERROR("data is not a DoIpHeader, DoIpHeader len=8,data len:{}", data.size());
                }
                return *this;
            }

            void SerialVecBuff(SerialVecBuff & data) const
            {
                MLOG_INFO("vector size:{}, mpayloadLength:{}", data.size(), mpayloadLength);
                if (data.size() != cHeaderSize + mpayloadLength)
                {
                    data.resize(cHeaderSize + mpayloadLength);
                }
                data.at(0) = mProtocolVersion;
                data.at(1) = mverseProtocolVersion;
                data.at(2) = (static_cast<uint16_t>(mPayloadType) & 0xFF); // L位
                data.at(3) = ((static_cast<uint16_t>(mPayloadType) >> 8) & 0xFF); //H位
                data.at(4) = (mpayloadLength & 0xFF);
                data.at(5) = ((mpayloadLength >> 8) & 0xFF);
                data.at(6) = ((mpayloadLength >> 16) & 0xFF);
                data.at(7) = ((mpayloadLength >> 24) & 0xFF);
            }
        };
    }
}

#endif //MVP_MODULES_DOIPHEAD_HPP
