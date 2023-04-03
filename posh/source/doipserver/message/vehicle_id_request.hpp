
#ifndef MVP_MODULES_VEHICLE_ID_REQUEST_HPP
#define MVP_MODULES_VEHICLE_ID_REQUEST_HPP

#include "message.hpp"

namespace motovis
{
    namespace doip{
        /// @brief Vehicle ID request DoIP message
        class VehicleIdRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{PayloadType::VehicleIdRequest};

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
            VehicleIdRequest() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            explicit VehicleIdRequest(uint8_t protocolVersion) noexcept
                    : Message(protocolVersion, cPayloadType)
            {
            }
        };
//        constexpr PayloadType VehicleIdRequest::cPayloadType;
    }
}

#endif //MVP_MODULES_VEHICLE_ID_REQUEST_H
