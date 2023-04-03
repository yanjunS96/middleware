#ifndef POWER_MODE_RESPONSE_H
#define POWER_MODE_RESPONSE_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Diagnostic node power mode information response
        class PowerModeResponse : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::DiagPowerModeInfoResponse};

            PowerModeType mPowerMode;

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize
                                   + sizeof mPowerMode};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                auto _powerModeByte{static_cast<uint8_t>(mPowerMode)};
                payload.insert(payload.begin(), _powerModeByte);
#else
                uint8_t offSize{cHeaderSize};
                auto _powerModeByte{static_cast<uint8_t>(mPowerMode)};
                memmove(&payload[offSize],&_powerModeByte,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof _powerModeByte)));
                offSize += sizeof _powerModeByte;
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                const std::size_t cExpectedSize{cHeaderSize + 1};
                std::size_t _offset{cHeaderSize};

                if (payload.size() == cExpectedSize)
                {
                    mPowerMode = Convert::ToEnum<PowerModeType>(payload, _offset);
                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            PowerModeResponse() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param powerMode DoIP node current power mode
            explicit PowerModeResponse(
                    uint8_t protocolVersion, PowerModeType powerMode) noexcept: Message(protocolVersion, cPayloadType),
                                                                                mPowerMode{powerMode}
            {
            }

            /// @brief Get power mode
            /// @return DoIP node current power mode
            PowerModeType GetPowerMode() const noexcept{
                return mPowerMode;
            }
        };
//        constexpr PayloadType PowerModeResponse::cPayloadType;
    }
}

#endif