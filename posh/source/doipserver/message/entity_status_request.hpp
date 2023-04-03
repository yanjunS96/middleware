#ifndef ENTITY_STATUS_REQUEST_H
#define ENTITY_STATUS_REQUEST_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Diagnostic entity status request
        class EntityStatusRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::DoipEntityStatusRequest};

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
            EntityStatusRequest() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            explicit EntityStatusRequest(uint8_t protocolVersion) noexcept: Message(protocolVersion, cPayloadType)
            {
            }
        };
//        constexpr PayloadType EntityStatusRequest::cPayloadType;
    }
}

#endif