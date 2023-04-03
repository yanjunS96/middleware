#ifndef VIN_VEHICLE_ID_REQUEST_HPP
#define VIN_VEHICLE_ID_REQUEST_HPP

#include <string>
#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Vehicle ID request DoIP message with vehicle ID number filtering
        class VinVehicleIdRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::VehicleIdRequestWithVin};

            std::string mVin;

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize + cVinSize};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                payload = std::vector<uint8_t>(mVin.cbegin(), mVin.cend());
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],mVin.data(),std::min(
                        payload.size() - offSize, cVinSize));
                offSize += cVinSize;
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override
            {
                const std::size_t cExpectedSize{
                        static_cast<std::size_t>(cHeaderSize + cVinSize)};

                if (payload.size() == cExpectedSize)
                {
                    std::size_t _offset{cHeaderSize};
                    mVin = Convert::ToString<cVinSize>(payload, _offset);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            VinVehicleIdRequest() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param vin Vehicle ID number filter
            /// @throws std::out_of_range Throws if the given VIN is invalid
            VinVehicleIdRequest(uint8_t protocolVersion, const std::string &vin): Message(protocolVersion, cPayloadType),
                                                                                  mVin{vin.size() == cVinSize ? vin : throw std::out_of_range("Invalid vehicle ID number")}
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param vin Vehicle ID number filter
            /// @throws std::out_of_range Throws if the given VIN is invalid
            VinVehicleIdRequest(uint8_t protocolVersion, std::string &&vin): Message(protocolVersion, cPayloadType),
                                                                             mVin{vin.size() == cVinSize ? std::move(vin) : throw std::out_of_range("Invalid vehicle ID number")}
            {
            }

            /// @brief Get VIN filter
            /// @return Vehicle ID number
            std::string GetVin() const{
                return mVin;
            }
        };
//        constexpr PayloadType VinVehicleIdRequest::cPayloadType;
    }
}

#endif