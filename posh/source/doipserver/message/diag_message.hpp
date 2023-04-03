
#ifndef MVP_MODULES_DIAG_MESSAGE_H
#define MVP_MODULES_DIAG_MESSAGE_H


#include "message.hpp"


namespace motovis
{
    namespace doip
    {
        /// @brief Diagnostic message (e.g., an UDS message)
        class DiagMessage : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{PayloadType::DiagMessage};

            /* DoIp payload message */
            uint16_t mSourceAddress;
            uint16_t mTargetAddress;
            std::vector<uint8_t> mUserData;

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize
                                + sizeof mSourceAddress + sizeof mTargetAddress + mUserData.size()};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
                /* mSourceAddress + mTargetAddress + mUserData */
#if  0
                payload.insert(payload.begin(), mUserData.cbegin(), mUserData.cend());
                Convert::ToByteVector<uint16_t>(mTargetAddress, payload);
                Convert::ToByteVector<uint16_t>(mSourceAddress, payload);
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],&mSourceAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mSourceAddress)));
                offSize += sizeof mSourceAddress;
                memmove(&payload[offSize],&mTargetAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mTargetAddress)));
                offSize += sizeof mTargetAddress;
                memmove(&payload[offSize],&mUserData[0],std::min(
                        (int)(payload.size() - offSize), (int)(mUserData.size())));
                offSize += mUserData.size();
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                if (payload.size() > cHeaderSize)
                {
                    const auto cPayloadSize{static_cast<std::size_t>(payloadLength)};
                    const std::size_t cAddressesSize{sizeof(uint16_t) + sizeof(uint16_t)};

                    std::size_t _offset{cHeaderSize};

                    mSourceAddress =
                            Convert::ToUnsignedInteger<uint16_t>(payload, _offset);

                    mTargetAddress =
                            Convert::ToUnsignedInteger<uint16_t>(payload, _offset);

                    const auto cBeginItr{payload.cbegin() + _offset};
                    // [Header: 8 bytes]<cBeginItr>[AddressesSize: 4 bytes][UserData: var bytes]
                    const auto cEndItr{cBeginItr + (cPayloadSize - cAddressesSize)};
                    mUserData = std::vector<uint8_t>(cBeginItr, cEndItr);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            DiagMessage(): Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress Sender logical address
            /// @param targetAddress Receiver logical address
            /// @param userData Actual diagnostic payload data (e.g., UDS message payload)
            DiagMessage(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint16_t targetAddress,
                    const std::vector<uint8_t> &userData): Message(protocolVersion, cPayloadType),
                                                           mSourceAddress{sourceAddress},
                                                           mTargetAddress{targetAddress},
                                                           mUserData{userData}
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress Sender logical address
            /// @param targetAddress Receiver logical address
            /// @param userData Actual diagnostic payload data (e.g., UDS message payload)
            DiagMessage(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint16_t targetAddress,
                    std::vector<uint8_t> &&userData): Message(protocolVersion, cPayloadType),
                                                      mSourceAddress{sourceAddress},
                                                      mTargetAddress{targetAddress},
                                                      mUserData{std::move(userData)}
            {
            }

            /// @brief Get message source address
            /// @return Sender logical address
            uint16_t GetSourceAddress() const noexcept{
                return mSourceAddress;
            }

            /// @brief Get message target address
            /// @return Receiver logical address
            uint16_t GetTargetAddress() const noexcept{
                return mTargetAddress;
            }

            /// @brief Get user data
            /// @param[out] userData Actual diagnostic payload data (e.g., UDS message payload)
            void GetUserData(std::vector<uint8_t> &userData) const{
                userData = mUserData;
            }
        };
//        constexpr PayloadType DiagMessage::cPayloadType;
    }
}


#endif //MVP_MODULES_DIAG_MESSAGE_H
