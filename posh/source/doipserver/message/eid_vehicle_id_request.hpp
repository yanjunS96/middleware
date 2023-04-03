#ifndef EID_VEHICLE_ID_REQUEST_H
#define EID_VEHICLE_ID_REQUEST_H

#include <array>
#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Vehicle ID request DoIP message with entity ID filtering
        class EidVehicleIdRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::VehicleIdRequestWithEid};

            std::array<uint8_t, cIdSize> mEid;

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize + cIdSize};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                payload = std::vector<uint8_t>(mEid.cbegin(), mEid.cend());
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],mEid.data(),std::min(
                        (int)(payload.size() - offSize), (int)(cIdSize)));
                offSize += cIdSize;
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                const std::size_t cExpectedSize{
                        static_cast<std::size_t>(cHeaderSize + cIdSize)};

                if (payload.size() == cExpectedSize)
                {
                    std::size_t _offset{cHeaderSize};
                    mEid = Convert::ToByteArray<cIdSize>(payload, _offset);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            EidVehicleIdRequest() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param eid Entity ID filter
            EidVehicleIdRequest(
                    uint8_t protocolVersion, std::array<uint8_t, cIdSize> eid): Message(protocolVersion, cPayloadType),
                                                                                 mEid{eid}
            {
            }

            /// @brief Get EID filter
            /// @return Entity ID
            std::array<uint8_t, cIdSize> GetEid() const{
                return mEid;
            }
        };
//        constexpr PayloadType EidVehicleIdRequest::cPayloadType;
    }
}

#endif