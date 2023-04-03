#ifndef ALIVE_CHECK_RESPONSE_H
#define ALIVE_CHECK_RESPONSE_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief DoIP tester alive check response
        class AliveCheckResponse : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{PayloadType::AliveCheckResponse};

            uint16_t mSourceAddress;

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize + sizeof mSourceAddress};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                Convert::ToByteVector<uint16_t>(mSourceAddress, payload);
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],&mSourceAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mSourceAddress)));
                offSize += sizeof mSourceAddress;
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override
            {
                const std::size_t cExpectedSize{cHeaderSize + 2};
                std::size_t _offset{cHeaderSize};

                if (payload.size() == cExpectedSize)
                {
                    mSourceAddress =
                            Convert::ToUnsignedInteger<uint16_t>(payload, _offset);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            AliveCheckResponse() noexcept: Message(cPayloadType){}

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress Tester logical address
            explicit AliveCheckResponse(
                    uint8_t protocolVersion, uint16_t sourceAddress) noexcept: Message(protocolVersion, cPayloadType),
                                                                               mSourceAddress{sourceAddress}
            {}

            /// @brief Get source address
            /// @return Tester logical address
            uint16_t GetSourceAddress() const noexcept
            {
                return mSourceAddress;
            }
        };
//        constexpr PayloadType AliveCheckResponse::cPayloadType;
    }
}

#endif