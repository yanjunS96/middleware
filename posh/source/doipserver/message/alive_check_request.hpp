#ifndef ALIVE_CHECK_REQUEST_H
#define ALIVE_CHECK_REQUEST_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief DoIP tester alive check request
        class AliveCheckRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{PayloadType::AliveCheckRequest};

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
                // The message contain no payload.
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override
            {
                // The message contain no payload.
                return payload.size() == cHeaderSize;
            }

        public:
            AliveCheckRequest() noexcept: Message(cPayloadType) {}

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            explicit AliveCheckRequest(uint8_t protocolVersion) noexcept: Message(protocolVersion, cPayloadType) {}
        };
//        constexpr PayloadType AliveCheckRequest::cPayloadType;
    }
}

#endif