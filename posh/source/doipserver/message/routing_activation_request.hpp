

#ifndef MVP_MODULES_ROUTING_ACTIVATION_REQUEST_H
#define MVP_MODULES_ROUTING_ACTIVATION_REQUEST_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Tester routing activation request to an internal vehicle network
        class RoutingActivationRequest : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::RoutingActivationRequest};
            const uint32_t cIsoReserved{0x00000000};

            uint16_t mSourceAddress;
            uint8_t mActivationType;
            bool mHasOemSpecificData;
            uint32_t mOemSpecificData;

        private:
            RoutingActivationRequest(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint8_t activationType,
                    bool hasOemSpecificData,
                    uint32_t oemSpecificData = 0) noexcept: Message(protocolVersion, cPayloadType),
                                                            mSourceAddress{sourceAddress},
                                                            mActivationType{activationType},
                                                            mHasOemSpecificData{hasOemSpecificData},
                                                            mOemSpecificData{oemSpecificData}
            {
            }

            bool TrySetCompulsoryPayload(const std::vector<uint8_t> &payload)
            {
                std::size_t _offset{cHeaderSize};

                auto _sourceAddress{
                        Convert::ToUnsignedInteger<uint16_t>(payload, _offset)};
                auto _activationType{
                        Convert::ToUnsignedInteger<uint8_t>(payload, _offset)};

                // Reserved bytes validation
                auto _actualReservedInt{
                        Convert::ToUnsignedInteger<uint32_t>(payload, _offset)};

                if (_actualReservedInt == cIsoReserved)
                {
                    mSourceAddress = _sourceAddress;
                    mActivationType = _activationType;

                    return true;
                }
                else
                {
                    return false;
                }
            }


        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize
                                   + sizeof mSourceAddress+sizeof mActivationType+sizeof cIsoReserved};
                if (mHasOemSpecificData) { vecLen += sizeof mOemSpecificData; }
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                if (mHasOemSpecificData)
                {
                    Convert::ToByteVector<uint32_t>(mOemSpecificData, payload);
                }

                Convert::ToByteVector<uint32_t>(cIsoReserved, payload);
                payload.insert(payload.begin(), mActivationType);
                Convert::ToByteVector<uint16_t>(mSourceAddress, payload);
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],&mSourceAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mSourceAddress)));
                offSize += sizeof mSourceAddress;

                memmove(&payload[offSize],&mActivationType,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mActivationType)));
                offSize += sizeof mActivationType;

                memmove(&payload[offSize],&cIsoReserved,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof cIsoReserved)));
                offSize += sizeof cIsoReserved;
                if (mHasOemSpecificData)
                {
                    memmove(&payload[offSize],&mOemSpecificData,std::min(
                            (int)(payload.size() - offSize), (int)(sizeof mOemSpecificData)));
                    offSize += sizeof mOemSpecificData;
                }
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override
            {
                const std::size_t cExpectedSizeMin{
                        static_cast<std::size_t>(cHeaderSize + 7)};
                const std::size_t cExpectedSizeMax{
                        static_cast<std::size_t>(cHeaderSize + 11)};
                std::size_t _offset{cExpectedSizeMin};

                if (payload.size() == cExpectedSizeMin)
                {
                    bool _succeed{TrySetCompulsoryPayload(payload)};
                    if (_succeed)
                    {
                        // No OEM-specific data
                        mHasOemSpecificData = false;
                    }

                    return _succeed;
                }
                else if (payload.size() == cExpectedSizeMax)
                {
                    bool _succeed{TrySetCompulsoryPayload(payload)};
                    if (_succeed)
                    {
                        // Has OEM-speific data
                        mHasOemSpecificData = true;
                        mOemSpecificData =
                                Convert::ToUnsignedInteger<uint32_t>(payload, _offset);
                    }

                    return _succeed;
                }
                else
                {
                    return false;
                }
            }

        public:
            RoutingActivationRequest() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor without OEM-specific data
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress External tester logical address
            /// @param activationType Routing activation type
            RoutingActivationRequest(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint8_t activationType) noexcept: RoutingActivationRequest(protocolVersion, sourceAddress, activationType, false)
            {
            }

            /// @brief Constructor using OEM-specific data
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress External tester logical address
            /// @param activationType Routing activation type
            /// @param oemSpecificData OEM-specific request data
            RoutingActivationRequest(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint8_t activationType,
                    uint32_t oemSpecificData) noexcept: RoutingActivationRequest(protocolVersion, sourceAddress, activationType, true, oemSpecificData)
            {
            }

            /// @brief Get request source address
            /// @return External tester logical address
            uint16_t GetSourceAddress() const noexcept
            {
                return mSourceAddress;
            }

            /// @brief Get activation type
            /// @return Routing activation type
            uint8_t GetActivationType() const noexcept
            {
                return mActivationType;
            }

            /// @brief Try to get the OEM-specific data
            /// @param[out] oemSpecificData OEM-specific request data
            /// @return True if the data is available, otherwise false
            bool TryGetOemSpecificData(uint32_t &oemSpecificData) const noexcept
            {
                if (mHasOemSpecificData)
                {
                    oemSpecificData = mOemSpecificData;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };
//        constexpr PayloadType RoutingActivationRequest::cPayloadType;
    }
}


#endif //MVP_MODULES_ROUTING_ACTIVATION_REQUEST_H
