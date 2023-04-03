#ifndef DIAG_MESSAGE_ACK_HPP
#define DIAG_MESSAGE_ACK_HPP

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Diagnostic message positive acknowledgement
        class DiagMessageAck : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::DiagMessagePositiveAcknowledgement};
            const uint8_t cAckCode{0x00};

            uint16_t mSourceAddress;
            uint16_t mTargetAddress;
            bool mHasPreviousMessage;
            std::vector<uint8_t> mPreviousMessage;

            bool TrySetCompulsoryPayload(const std::vector<uint8_t> &payload){
                std::size_t _offset{cHeaderSize};

                auto _sourceAddress{
                        Convert::ToUnsignedInteger<uint16_t>(payload, _offset)};
                auto _targetAddress{
                        Convert::ToUnsignedInteger<uint16_t>(payload, _offset)};

                // Reserved bytes validation
                uint8_t _actualAckCode{payload.at(_offset)};

                if (_actualAckCode == cAckCode)
                {
                    mSourceAddress = _sourceAddress;
                    mTargetAddress = _targetAddress;

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
                                   + sizeof mSourceAddress + sizeof mTargetAddress + sizeof cAckCode};
                if (mHasPreviousMessage) { vecLen += mPreviousMessage.size(); }
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                if (mHasPreviousMessage)
                {
                    payload.insert(
                            payload.begin(), mPreviousMessage.cbegin(), mPreviousMessage.cend());
                }

                Convert::ToByteVector<uint8_t>(cAckCode, payload);
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
                memmove(&payload[offSize],&cAckCode,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof cAckCode)));
                offSize += sizeof cAckCode;
                if (mHasPreviousMessage)
                {
                    memmove(&payload[offSize],mPreviousMessage.data(),std::min(
                            (int)(payload.size() - offSize), (int)(mPreviousMessage.size())));
                    offSize += mPreviousMessage.size();
                }
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                const std::size_t cCompulsoryPayloadSize{5};
                const std::size_t cExpectedSizeMin{
                        static_cast<std::size_t>(cHeaderSize + cCompulsoryPayloadSize)};

                if (payload.size() == cExpectedSizeMin)
                {
                    bool _succeed{TrySetCompulsoryPayload(payload)};
                    if (_succeed)
                    {
                        // No previous message payload
                        mHasPreviousMessage = false;
                    }

                    return _succeed;
                }
                else if (payload.size() > cExpectedSizeMin)
                {
                    bool _succeed{TrySetCompulsoryPayload(payload)};
                    if (_succeed)
                    {
                        const auto cPayloadSize{static_cast<std::size_t>(payloadLength)};
                        // Has revious message payload
                        mHasPreviousMessage = true;
                        const auto cBeginItr{payload.cbegin() + cExpectedSizeMin};
                        // [Header: 8 bytes]<cBeginItr>[CompulsoryPayload: 5 bytes][PreviousMessage: var bytes]
                        const auto cEndItr{
                                cBeginItr + (cPayloadSize - cCompulsoryPayloadSize)};
                        mPreviousMessage = std::vector<uint8_t>(cBeginItr, cEndItr);
                    }

                    return _succeed;
                }
                else
                {
                    return false;
                }
            }

        public:
            DiagMessageAck(): Message(cPayloadType)
            {
            }

            /// @brief Constructor without previous message
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress Sender logical address
            /// @param targetAddress Receiver logical address
            DiagMessageAck(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint16_t targetAddress): Message(protocolVersion, cPayloadType),
                                             mSourceAddress{sourceAddress},
                                             mTargetAddress{targetAddress},
                                             mHasPreviousMessage{false}
            {
            }

            /// @brief Constructor including previous message
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress Sender logical address
            /// @param targetAddress Receiver logical address
            /// @param previousMessage Previous diagnostic message payload
            DiagMessageAck(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint16_t targetAddress,
                    const std::vector<uint8_t> &previousMessage): Message(protocolVersion, cPayloadType),
                                                                  mSourceAddress{sourceAddress},
                                                                  mTargetAddress{targetAddress},
                                                                  mHasPreviousMessage{true},
                                                                  mPreviousMessage{previousMessage}
            {
            }

            /// @brief Constructor including previous message
            /// @param protocolVersion DoIP ISO protocol version
            /// @param sourceAddress Sender logical address
            /// @param targetAddress Receiver logical address
            /// @param previousMessage Previous diagnostic message payload
            DiagMessageAck(
                    uint8_t protocolVersion,
                    uint16_t sourceAddress,
                    uint16_t targetAddress,
                    std::vector<uint8_t> &&previousMessage): Message(protocolVersion, cPayloadType),
                                                             mSourceAddress{sourceAddress},
                                                             mTargetAddress{targetAddress},
                                                             mHasPreviousMessage{true},
                                                             mPreviousMessage{std::move(previousMessage)}
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

            /// @brief Get previous diagnostic message
            /// @param[out] previousMessage Previous diagnostic message payload
            /// @return True if the message is available, otherwise false
            bool TryGetPreviousMessage(std::vector<uint8_t> &previousMessage){
                if (mHasPreviousMessage)
                {
                    previousMessage = mPreviousMessage;
                }
                return mHasPreviousMessage;
            }
        };
//        constexpr PayloadType DiagMessageAck::cPayloadType;
    }
}

#endif