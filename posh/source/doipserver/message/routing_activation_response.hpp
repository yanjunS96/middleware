#ifndef ROUTING_ACTIVATION_RESPONSE_H
#define ROUTING_ACTIVATION_RESPONSE_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Tester routing activation response to an internal vehicle network
        class RoutingActivationResponse : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::RoutingActivationResponse};
            const uint32_t cIsoReserved{0x00000000};

            uint16_t mTesterLogicalAddress;
            uint16_t mEntityLogicalAddress;
            RoutingActivationResponseType mResponseCode;
            bool mHasOemSpecificData;
            uint32_t mOemSpecificData;

            RoutingActivationResponse(
                    uint8_t protocolVersion,
                    uint16_t testerLogicalAddress,
                    uint16_t entityLogicalAddress,
                    RoutingActivationResponseType responseCode,
                    bool hasOemSpecificData,
                    uint32_t oemSpecificData = 0) noexcept: Message(protocolVersion, cPayloadType),
                                                            mTesterLogicalAddress{testerLogicalAddress},
                                                            mEntityLogicalAddress{entityLogicalAddress},
                                                            mResponseCode{responseCode},
                                                            mHasOemSpecificData{hasOemSpecificData},
                                                            mOemSpecificData{oemSpecificData}
            {
            }

            void SetPayload(const std::vector<uint8_t> &payload)
            {
                std::size_t _offset{cHeaderSize};

                mTesterLogicalAddress = Convert::ToUnsignedInteger<uint16_t>(payload, _offset);
                mEntityLogicalAddress = Convert::ToUnsignedInteger<uint16_t>(payload, _offset);
                mResponseCode =Convert::ToEnum<RoutingActivationResponseType>(payload, _offset);
            }

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize
                                   + sizeof mTesterLogicalAddress+sizeof mEntityLogicalAddress+sizeof mResponseCode+sizeof cIsoReserved};
                if (mHasOemSpecificData) { vecLen += sizeof mOemSpecificData; }
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override{
#if 0
                if (mHasOemSpecificData)
                {
                    Convert::ToByteVector<uint32_t>(mOemSpecificData, payload);
                }

                Convert::ToByteVector<uint32_t>(cIsoReserved, payload);

                auto _responseCodeByte{static_cast<uint8_t>(mResponseCode)};
                payload.insert(payload.begin(), _responseCodeByte);

                Convert::ToByteVector<uint16_t>(mEntityLogicalAddress, payload);

                Convert::ToByteVector<uint16_t>(mTesterLogicalAddress, payload);
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],&mTesterLogicalAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mTesterLogicalAddress)));
                offSize += sizeof mTesterLogicalAddress;

                memmove(&payload[offSize],&mEntityLogicalAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mEntityLogicalAddress)));
                offSize += sizeof mEntityLogicalAddress;

                memmove(&payload[offSize],&mResponseCode,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mResponseCode)));
                offSize += sizeof mResponseCode;

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
                        static_cast<std::size_t>(cHeaderSize + 9)};
                const std::size_t cExpectedSizeMax{
                        static_cast<std::size_t>(cHeaderSize + 13)};
                std::size_t _offset{cExpectedSizeMin};

                if (payload.size() == cExpectedSizeMin)
                {
                    SetPayload(payload);
                    // No OEM-specific data
                    mHasOemSpecificData = false;

                    return true;
                }
                else if (payload.size() == cExpectedSizeMax)
                {
                    SetPayload(payload);
                    // Has OEM-speific data
                    mHasOemSpecificData = true;
                    mOemSpecificData =
                            Convert::ToUnsignedInteger<uint32_t>(payload, _offset);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            RoutingActivationResponse() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor without OEM-specific data
            /// @param protocolVersion DoIP ISO protocol version
            /// @param testerLogicalAddress External tester logical address
            /// @param entityLogicalAddress Internal DoIP entity logical address
            /// @param responseCode Routing activation response code
            RoutingActivationResponse(
                    uint8_t protocolVersion,
                    uint16_t testerLogicalAddress,
                    uint16_t entityLogicalAddress,
                    RoutingActivationResponseType responseCode) noexcept: RoutingActivationResponse(protocolVersion, testerLogicalAddress, entityLogicalAddress, responseCode, false)
            {
            }

            /// @brief Constructor using OEM-specific data
            /// @param protocolVersion DoIP ISO protocol version
            /// @param testerLogicalAddress External tester logical address
            /// @param entityLogicalAddress Internal DoIP entity logical address
            /// @param responseCode Routing activation response code
            /// @param oemSpecificData OEM-specific request data
            RoutingActivationResponse(
                    uint8_t protocolVersion,
                    uint16_t testerLogicalAddress,
                    uint16_t entityLogicalAddress,
                    RoutingActivationResponseType responseCode,
                    uint32_t oemSpecificData) noexcept: RoutingActivationResponse(protocolVersion, testerLogicalAddress, entityLogicalAddress, responseCode, true, oemSpecificData)
            {
            }

            /// @brief Get tester logical address
            /// @return External tester logical address
            uint16_t GetTesterLogicalAddress() const noexcept{
                return mTesterLogicalAddress;
            }

            /// @brief Get entity logical address
            /// @return Internal DoIP entity logical address
            uint16_t GetEntityLogicalAddress() const noexcept{
                return mEntityLogicalAddress;
            }

            /// @brief Get response code
            /// @return Routing activation response code
            RoutingActivationResponseType GetResponseCode() const noexcept{
                return mResponseCode;
            }

            /// @brief Try to get the OEM-specific data
            /// @param[out] oemSpecificData OEM-specific request data
            /// @return True if the data is available, otherwise false
            bool TryGetOemSpecificData(uint32_t &oemSpecificData) const noexcept{
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
//        constexpr PayloadType RoutingActivationResponse::cPayloadType;
    }
}

#endif