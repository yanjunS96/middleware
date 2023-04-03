#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string.h>
#include "doip_common/convert.hpp"
#include "doip_common/DoIpHead.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief DoIP generic message
        class Message
        {
        public:
            /// @brief Serialize DoIP message
            /// @param[out] serializedMessage Serialize message byte array
            void Serialize(std::vector<uint8_t> &serializedMessage) const
            {
                SetPayloadLen(serializedMessage); //获取 vector 的内存空间
                uint8_t offSize{0};
                memmove(&serializedMessage[offSize], &mProtocolVersion, std::min(
                        (int)(sizeof mProtocolVersion), (int)(serializedMessage.size() - offSize)));
                offSize += sizeof mProtocolVersion;
                auto _inverseProtocolVersion{static_cast<uint8_t>(~mProtocolVersion)};
                memmove(&serializedMessage[offSize], &_inverseProtocolVersion, std::min(
                        (int)(sizeof _inverseProtocolVersion), (int)(serializedMessage.size() - offSize)));
                offSize += sizeof _inverseProtocolVersion;
                auto _payloadTypeInt{static_cast<uint16_t>(mPayloadType)};
                memmove(&serializedMessage[offSize], &_payloadTypeInt, std::min(
                        (int)(sizeof _payloadTypeInt), (int)(serializedMessage.size() - offSize)));
                offSize += sizeof _payloadTypeInt;
                auto _payloadLength{static_cast<uint32_t>(serializedMessage.size() - cHeaderSize)};
                memmove(&serializedMessage[offSize], &_payloadLength, std::min(
                        (int)(sizeof _payloadLength), (int)(serializedMessage.size() - offSize)));
                offSize += sizeof _payloadLength; //8byte
                GetPayload(serializedMessage);
            }

            /// @brief Try to deserialize DoIP message
            /// @param[in] serializedMessage Serialize message byte array
            /// @param[out] nackCode Negative acknowledgement code if deserialization failed
            /// @return True if the deserialization is successful, otherwise false
            bool TryDeserialize(
                    const std::vector<uint8_t> &serializedMessage,
                    GenericNackType &nackCode)
            {
                // Header length check
                if (serializedMessage.size() < cHeaderSize)
                {
                    MLOG_ERROR("serializedMessage.size() error! {}", serializedMessage.size());
                    nackCode = GenericNackType::InvalidPayloadLength;
                    return false;
                }
#if 0
                std::size_t _offset{0};
                uint8_t _actualProtocolVersion{serializedMessage.at(_offset)}; /* Protocol Version: 1byte */
                ++_offset;

                uint8_t _inverseProtocolVersion{serializedMessage.at(_offset)}; /* ~Protocol Version: 1byte */
                ++_offset;

                // Protocol version check
                auto _expectedProtocolVersion{
                        static_cast<uint8_t>(~_inverseProtocolVersion)};

                if (_actualProtocolVersion != _expectedProtocolVersion)
                {
                    MLOG_ERROR("Protocol version check failed! {} {}", _expectedProtocolVersion, _actualProtocolVersion);
                    nackCode = GenericNackType::InvalidProtocolVersion;
                    return false;
                }

                auto _payloadType{Convert::ToEnum<PayloadType>(serializedMessage, _offset)}; /* PayloadType: 2byte 大小端错了 */

//                MLOG_WARN("version:{} {} {}",_actualProtocolVersion,_inverseProtocolVersion, static_cast<uint16_t>(_payloadType));
                // Payload type check
                if (_payloadType != mPayloadType)
                {
                    MLOG_ERROR("Payload type check failed! {} {}", (uint16_t)_payloadType, (uint16_t)mPayloadType);
                    nackCode = GenericNackType::UnsupportedPayloadType;
                    return false;
                }

                auto _payloadLength{
                        Convert::ToUnsignedInteger<uint32_t>(serializedMessage, _offset)}; /* Payload length: 4byte */

                // Payload length check
                if (_payloadLength > serializedMessage.size() - cHeaderSize)
                {
                    MLOG_ERROR("Payload length check failed! {} {}", _payloadLength, serializedMessage.size() - cHeaderSize);
                    nackCode = GenericNackType::InvalidPayloadLength;
                    return false;
                }
#else
                DoIpHeader header;
                header << serializedMessage;

                uint8_t _actualProtocolVersion{header.mProtocolVersion}; /* Protocol Version: 1byte */
                if (header.mProtocolVersion != (0xFF ^ header.mverseProtocolVersion))
                {
                    MLOG_ERROR("Protocol version check failed! {} {}", header.mProtocolVersion, header.mverseProtocolVersion);
                    nackCode = GenericNackType::InvalidProtocolVersion;
                    return false;
                }

                if (header.mPayloadType != mPayloadType)
                {
                    MLOG_ERROR("Payload type check failed! {} {}", (uint16_t)header.mPayloadType, (uint16_t)mPayloadType);
                    nackCode = GenericNackType::UnsupportedPayloadType;
                    return false;
                }
                auto _payloadLength{header.mpayloadLength}; /* Payload length: 4byte */

                // Payload length check
                if (_payloadLength > serializedMessage.size() - cHeaderSize)
                {
                    MLOG_ERROR("Payload length check failed! {} {}", _payloadLength, serializedMessage.size() - cHeaderSize);
                    nackCode = GenericNackType::InvalidPayloadLength;
                    return false;
                }
#endif
                bool _succeed{TrySetPayload(serializedMessage, _payloadLength)}; /* 派生重载 */
                if (_succeed)
                {
                    mProtocolVersion = _actualProtocolVersion;
                }
                else
                {
                    nackCode = GenericNackType::InvalidPayloadLength;
                }
                return _succeed;
            }

            /// @brief Try to extract the payload type from the given serialized message
            /// @param[in] serializedMessage Serialized message byte array
            /// @param[out] payloadType Extracted payload type
            /// @return True if the payload type is extracted successfully, otherwise false
            static bool TryExtractPayloadType(
                    const std::vector<uint8_t> &serializedMessage,
                    PayloadType &payloadType)
            {
                if (serializedMessage.size() >= cHeaderSize)
                {
                    std::size_t _payloadOffset{2};

                    auto _payloadTypeInt{
                            Convert::ToUnsignedInteger<uint16_t>(
                                    serializedMessage, _payloadOffset)};
                    payloadType = static_cast<PayloadType>(_payloadTypeInt);

                    return true;
                }
                else
                {
                    return false;
                }
            }

            virtual ~Message() noexcept = default;
        protected:
            /// @brief Constructor
            /// @param payloadType DoIP message payload type
            explicit Message(PayloadType payloadType) noexcept
            : mPayloadType{payloadType}
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param payloadType DoIP message payload type
            Message(uint8_t protocolVersion, PayloadType payloadType) noexcept
                    : mProtocolVersion{protocolVersion},
                      mPayloadType{payloadType}
            {
            }

            /// @brief Get message payload
            /// @param[out] payload Payload byte array
            virtual void GetPayload(std::vector<uint8_t> &payload) const = 0; /* DoIp payload 组包 */
            virtual void SetPayloadLen(std::vector<uint8_t> &payload) const = 0; /* DoIp payload 组包 */

            /// @brief Try to set message payload
            /// @param[in] payload Payload byte array
            /// @param[in] payloadLength Payload length from the message header
            /// @return True if the payload is set successfully, otherwise false
            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) = 0; /* 从 DoIp 报文中 取出 DoIp payload */

        protected:
            /// @brief DoIP message header size
        private:
            uint8_t mProtocolVersion;
            const PayloadType mPayloadType;
        };

        using MessPtr=std::shared_ptr<Message>;
    }
}

#endif