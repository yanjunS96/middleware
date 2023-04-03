#ifndef ENTITY_STATUS_RESPONSE_H
#define ENTITY_STATUS_RESPONSE_H

#include "message.hpp"

namespace motovis
{
    namespace doip
    {
        /// @brief Diagnostic entity status response
        class EntityStatusResponse : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::DoipEntityStatusResponse};

            NodeType mEntityType;
            uint8_t mMaxOpenSockets;
            uint8_t mCurrentlyOpenSocket;
            bool mHasMaxDataSize;
            uint32_t mMaxDataSize;

            EntityStatusResponse(
                    uint8_t protocolVersion,
                    NodeType entityType,
                    uint8_t maxOpenSockets,
                    uint8_t currentlyOpenSocket,
                    bool hasMaxDataSize,
                    uint32_t maxDataSize = 0) noexcept: Message(protocolVersion, cPayloadType),
                                                        mEntityType{entityType},
                                                        mMaxOpenSockets{maxOpenSockets},
                                                        mCurrentlyOpenSocket{currentlyOpenSocket},
                                                        mHasMaxDataSize{hasMaxDataSize},
                                                        mMaxDataSize{maxDataSize}
            {
            }

            void SetPayload(const std::vector<uint8_t> &payload){
                std::size_t _offset{cHeaderSize};

                mEntityType = Convert::ToEnum<NodeType>(payload, _offset);
                mMaxOpenSockets = Convert::ToUnsignedInteger<uint8_t>(payload, _offset);
                mCurrentlyOpenSocket = Convert::ToUnsignedInteger<uint8_t>(payload, _offset);
            }

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen{cHeaderSize
                                   + sizeof mEntityType + sizeof mMaxOpenSockets + sizeof mCurrentlyOpenSocket};
                if (mHasMaxDataSize) { vecLen += sizeof mMaxDataSize; }
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }
            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                if (mHasMaxDataSize)
                {
                    Convert::ToByteVector<uint32_t>(mMaxDataSize, payload);
                }

                payload.insert(payload.begin(), mCurrentlyOpenSocket);

                payload.insert(payload.begin(), mMaxOpenSockets);

                auto _entityTypeByte{static_cast<uint8_t>(mEntityType)};
                payload.insert(payload.begin(), _entityTypeByte);
#else
                uint8_t offSize{cHeaderSize};
                auto _entityTypeByte{static_cast<uint8_t>(mEntityType)};
                memmove(&payload[offSize],&_entityTypeByte,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof _entityTypeByte)));
                offSize += sizeof _entityTypeByte;

                memmove(&payload[offSize],&mMaxOpenSockets,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mMaxOpenSockets)));
                offSize += sizeof mMaxOpenSockets;

                memmove(&payload[offSize],&mCurrentlyOpenSocket,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mCurrentlyOpenSocket)));
                offSize += sizeof mCurrentlyOpenSocket;

                if (mHasMaxDataSize)
                {
                    memmove(&payload[offSize],&mMaxDataSize,std::min(
                            (int)(payload.size() - offSize), (int)(sizeof mMaxDataSize)));
                    offSize += sizeof mMaxDataSize;
                }
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override{
                const std::size_t cExpectedSizeMin{
                        static_cast<std::size_t>(cHeaderSize + 3)};
                const std::size_t cExpectedSizeMax{
                        static_cast<std::size_t>(cHeaderSize + 7)};
                std::size_t _offset{cExpectedSizeMin};

                if (payload.size() == cExpectedSizeMin)
                {
                    SetPayload(payload);
                    // No maximum data size field
                    mHasMaxDataSize = false;

                    return true;
                }
                else if (payload.size() == cExpectedSizeMax)
                {
                    SetPayload(payload);
                    // Has maximum data size field
                    mHasMaxDataSize = true;
                    mMaxDataSize =
                            Convert::ToUnsignedInteger<uint32_t>(payload, _offset);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            EntityStatusResponse() noexcept: Message(cPayloadType)
            {
            }

            /// @brief Constructor without maximum data size field
            /// @param protocolVersion DoIP ISO protocol version
            /// @param entityType DoIP entity type
            /// @param maxOpenSockets Maximum number of concurrent open TCP sockets at the entity
            /// @param currentlyOpenSocket Current number of open TCP socket at the entity
            EntityStatusResponse(
                    uint8_t protocolVersion,
                    NodeType entityType,
                    uint8_t maxOpenSockets,
                    uint8_t currentlyOpenSocket) noexcept: EntityStatusResponse(protocolVersion, entityType, maxOpenSockets, currentlyOpenSocket, false)
            {
            }

            /// @brief Constructor using maximum data size field
            /// @param protocolVersion DoIP ISO protocol version
            /// @param entityType DoIP entity type
            /// @param maxOpenSockets Maximum number of concurrent open TCP sockets at the entity
            /// @param currentlyOpenSocket Current number of open TCP socket at the entity
            /// @param maxDataSize Maximum processable request data size
            EntityStatusResponse(
                    uint8_t protocolVersion,
                    NodeType entityType,
                    uint8_t maxOpenSockets,
                    uint8_t currentlyOpenSocket,
                    uint32_t maxDataSize) noexcept: EntityStatusResponse(protocolVersion, entityType, maxOpenSockets, currentlyOpenSocket, true, maxDataSize)
            {
            }

            /// @brief Get entity type
            /// @return DoIP entity type
            NodeType GetEntityType() const noexcept{
                return mEntityType;
            }

            /// @brief Get maximum open sockets number
            /// @return Maximum number of concurrent open TCP sockets at the entity
            uint8_t GetMaxOpenSockets() const noexcept{
                return mMaxOpenSockets;
            }

            /// @brief Get currently open socket number
            /// @return Current number of open TCP socket at the entity
            uint8_t GetCurrentlyOpenSocket() const noexcept{
                return mCurrentlyOpenSocket;
            }

            /// @brief Try to get the maximum data size
            /// @param[out] maxDataSize Maximum processable request data size
            /// @return True if the maximum size is available, otherwise false
            bool TryGetMaxDataSize(uint32_t &maxDataSize) const noexcept{
                if (mHasMaxDataSize)
                {
                    maxDataSize = mMaxDataSize;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };
//        constexpr PayloadType EntityStatusResponse::cPayloadType;
    }
}

#endif