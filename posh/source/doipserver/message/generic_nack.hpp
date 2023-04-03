
#ifndef MVP_MODULES_GENERIC_NACK_HPP
#define MVP_MODULES_GENERIC_NACK_HPP

#include "message.hpp"

namespace motovis
{
    namespace doip{
        /// @brief Generic negative acknowledgement DoIP message
        class GenericNack : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::GenericNegativeAcknowledgement};

            GenericNackType mNackCode;

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize
                                   + sizeof mNackCode};
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                auto _nackCodeByte{static_cast<uint8_t>(mNackCode)};
                payload.insert(payload.begin(), _nackCodeByte);
#else
                uint8_t offSize{cHeaderSize};
                auto _nackCodeByte{static_cast<uint8_t>(mNackCode)};
                memmove(&payload[offSize],&_nackCodeByte,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof _nackCodeByte)));
                offSize += sizeof _nackCodeByte;
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                const std::size_t cExpectedSize{cHeaderSize + sizeof(GenericNackType)};
                std::size_t _offset{cHeaderSize};

                if (payload.size() == cExpectedSize)
                {
                    mNackCode = Convert::ToEnum<GenericNackType>(payload, _offset);
                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            GenericNack() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor
            /// @param protocolVersion DoIP ISO protocol version
            /// @param nackCode Negative acknowledgement code
            GenericNack(uint8_t protocolVersion, GenericNackType nackCode) noexcept: Message(protocolVersion, cPayloadType),
                                                                                     mNackCode{nackCode}
            {
            }

            /// @brief Get NACK code
            /// @return Negative acknoweledgement code
            GenericNackType GetNackCode() const noexcept
            {
                return mNackCode;
            }
        };
//        constexpr PayloadType GenericNack::cPayloadType;
    }
}

#endif //MVP_MODULES_GENERIC_NACK_H
