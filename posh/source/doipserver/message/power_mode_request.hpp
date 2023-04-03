#ifndef POWER_MODE_REQUEST_H
#define POWER_MODE_REQUEST_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Diagnostic node power mode information request
        class PowerModeRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::DiagPowerModeInfoRequest};

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override{
                // The message contain no payload.
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                // The message contain no payload.
                return payload.size() == cHeaderSize;
            }

        public:
            PowerModeRequest() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            explicit PowerModeRequest(uint8_t protocolVersion) noexcept: Message(protocolVersion, cPayloadType)
            {
            }
        };
//        constexpr PowerModeRequest::PayloadType cPayloadType;
    }

}

#endif