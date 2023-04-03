#ifndef VEHICLE_ID_RESPONSE_HPP
#define VEHICLE_ID_RESPONSE_HPP

#include <array>
#include <string>
#include "message.hpp"

namespace motovis
{
    namespace doip{
        /// @brief Vehicle announcement ID response message
        class VehicleIdResponse : public Message
        {
        private:
            static constexpr PayloadType cPayloadType{
                    PayloadType::VehicleAnnoucementIdResponse};

            std::string mVin;
            uint16_t mLogicalAddress;
            std::array<uint8_t, cIdSize> mEid;
            std::array<uint8_t, cIdSize> mGid;
            uint8_t mFurtherAction;
            bool mUseVehicleIdSync;
            uint8_t mVinGidStatus;

            void SetPayload(const std::vector<uint8_t> &payload)
            {
                std::size_t _offset{cHeaderSize};

                mVin = Convert::ToString<cVinSize>(payload, _offset);
                mLogicalAddress = Convert::ToUnsignedInteger<uint16_t>(payload, _offset);
                mEid = Convert::ToByteArray<cIdSize>(payload, _offset);
                mGid = Convert::ToByteArray<cIdSize>(payload, _offset);
                mFurtherAction = payload.at(_offset);
            }

        protected:
            void SetPayloadLen(std::vector<uint8_t> &payload) const override
            {
                std::size_t vecLen = cHeaderSize +
                        cVinSize + sizeof mLogicalAddress + cIdSize + cIdSize + sizeof mFurtherAction;
                if (mUseVehicleIdSync)
                {
                    vecLen += sizeof mVinGidStatus;
                }
                if (vecLen != payload.size()) { payload.resize(vecLen); }
            }

            void GetPayload(std::vector<uint8_t> &payload) const override
            {
#if 0
                if (mUseVehicleIdSync)
                {
                    payload.insert(payload.begin(), mVinGidStatus);
                }

                payload.insert(payload.begin(), mFurtherAction);
                payload.insert(payload.begin(), mGid.cbegin(), mGid.cend());
                payload.insert(payload.begin(), mEid.cbegin(), mEid.cend());
                Convert::ToByteVector<uint16_t>(mLogicalAddress, payload);
                payload.insert(payload.begin(), mVin.cbegin(), mVin.cend());
#else
                uint8_t offSize{cHeaderSize};
                memmove(&payload[offSize],mVin.data(),std::min(
                        payload.size() - offSize, cVinSize));
                offSize += cVinSize;

                memmove(&payload[offSize],&mLogicalAddress,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mLogicalAddress)));
                offSize += sizeof mLogicalAddress;

                memmove(&payload[offSize],mEid.data(),std::min(
                        payload.size() - offSize, cIdSize));
                offSize += cIdSize;

                memmove(&payload[offSize],mGid.data(),std::min(
                        payload.size() - offSize, cIdSize));
                offSize += cIdSize;

                memmove(&payload[offSize],&mFurtherAction,std::min(
                        (int)(payload.size() - offSize), (int)(sizeof mFurtherAction)));
                offSize += sizeof mFurtherAction;

                if (mUseVehicleIdSync)
                {
                    memmove(&payload[offSize],&mVinGidStatus,std::min(
                            (int)(payload.size() - offSize), (int)(sizeof mVinGidStatus)));
                    offSize += sizeof mVinGidStatus;
                }
#endif
            }

            virtual bool TrySetPayload(
                    const std::vector<uint8_t> &payload,
                    uint32_t payloadLength) override
            {
                const std::size_t cExpectedSizeMin{
                        static_cast<std::size_t>(cHeaderSize + 32)};
                const std::size_t cExpectedSizeMax{
                        static_cast<std::size_t>(cHeaderSize + 33)};
                const std::size_t cVinGidStatusIndex{cExpectedSizeMin};

                MLOG_INFO("paload.size():{}", payload.size());
                if (payload.size() == cExpectedSizeMin)
                {
                    SetPayload(payload);
                    // No vehicle ID synchronization
                    mUseVehicleIdSync = false;

                    return true;
                }
                else if (payload.size() == cExpectedSizeMax)
                {
                    SetPayload(payload);
                    // Use vehicle ID synchronization
                    mUseVehicleIdSync = true;
                    mVinGidStatus = payload.at(cVinGidStatusIndex);

                    return true;
                }
                else
                {
                    return false;
                }
            }

        public:
            VehicleIdResponse() noexcept: Message(cPayloadType)
            {
            }
            /// @brief Constructor without optional VIN/GID synchronization
            /// @param protocolVersion DoIP ISO protocol version
            /// @param vin Vehicle ID number (VIN)
            /// @param logicalAddress Entity logical address
            /// @param eid Entity ID
            /// @param gid Group ID (GID)
            /// @param furtherAction Further action byte
            /// @throws std::out_of_range Throws if the given VIN is invalid
            VehicleIdResponse(
                    uint8_t protocolVersion,
                    const std::string &vin,
                    uint16_t logicalAddress,
                    std::array<uint8_t, cIdSize> eid,
                    std::array<uint8_t, cIdSize> gid,
                    uint8_t furtherAction): Message(protocolVersion, cPayloadType),
                                            mVin{vin.size() == cVinSize ? vin : /*throw std::out_of_range("Invalid vehicle ID number")*/
                                                 [&]()->std::string{
                                                     std::string tem;
                                                     if (vin.size() > cVinSize)
                                                     {
                                                         tem.assign(vin.begin(), vin.begin() + cVinSize);
                                                     }
                                                     else
                                                     {
                                                         tem = vin;
                                                         for (int i = vin.size(); i < cVinSize; ++i) {
                                                             tem += ' ';
                                                         }
                                                     }
                                                     return tem;
                                                 }()},
                                            mLogicalAddress{logicalAddress},
                                            mEid{eid},
                                            mGid{gid},
                                            mFurtherAction{furtherAction},
                                            mUseVehicleIdSync{false}
            {
            }

            /// @brief Constructor without optional VIN/GID synchronization
            /// @param protocolVersion DoIP ISO protocol version
            /// @param vin Vehicle ID number (VIN)
            /// @param logicalAddress Entity logical address
            /// @param eid Entity ID
            /// @param gid Group ID (GID)
            /// @param furtherAction Further action byte
            /// @throws std::out_of_range Throws if the given VIN is invalid
            VehicleIdResponse(
                    uint8_t protocolVersion,
                    std::string &&vin,
                    uint16_t logicalAddress,
                    std::array<uint8_t, cIdSize> eid,
                    std::array<uint8_t, cIdSize> gid,
                    uint8_t furtherAction): Message(protocolVersion, cPayloadType),
                                            mVin{vin.size() == cVinSize ? std::move(vin) : throw std::out_of_range("Invalid vehicle ID number")},
                                            mLogicalAddress{logicalAddress},
                                            mEid{eid},
                                            mGid{gid},
                                            mFurtherAction{furtherAction},
                                            mUseVehicleIdSync{false}
            {
            }

            /// @brief Constructor using VIN/GID synchronization
            /// @param protocolVersion DoIP ISO protocol version
            /// @param vin Vehicle ID number (VIN)
            /// @param logicalAddress Entity logical address
            /// @param eid Entity ID
            /// @param gid Group ID (GID)
            /// @param furtherAction Further action byte
            /// @param vinGidStatus VIN/GID synchronization status
            /// @throws std::out_of_range Throws if the given VIN is invalid
            VehicleIdResponse(
                    uint8_t protocolVersion,
                    const std::string &vin,
                    uint16_t logicalAddress,
                    std::array<uint8_t, cIdSize> eid,
                    std::array<uint8_t, cIdSize> gid,
                    uint8_t furtherAction,
                    uint8_t vinGidStatus): Message(protocolVersion, cPayloadType),
                                           mVin{vin.size() == cVinSize ? vin : throw std::out_of_range("Invalid vehicle ID number")},
                                           mLogicalAddress{logicalAddress},
                                           mEid{eid},
                                           mGid{gid},
                                           mFurtherAction{furtherAction},
                                           mUseVehicleIdSync{true},
                                           mVinGidStatus{vinGidStatus}
            {
            }

            /// @brief Constructor using VIN/GID synchronization
            /// @param protocolVersion DoIP ISO protocol version
            /// @param vin Vehicle ID number (VIN)
            /// @param logicalAddress Entity logical address
            /// @param eid Entity ID
            /// @param gid Group ID (GID)
            /// @param furtherAction Further action byte
            /// @param vinGidStatus VIN/GID synchronization status
            /// @throws std::out_of_range Throws if the given VIN is invalid
            VehicleIdResponse(
                    uint8_t protocolVersion,
                    std::string &&vin,
                    uint16_t logicalAddress,
                    std::array<uint8_t, cIdSize> eid,
                    std::array<uint8_t, cIdSize> gid,
                    uint8_t furtherAction,
                    uint8_t vinGidStatus): Message(protocolVersion, cPayloadType),
                                           mVin{vin.size() == cVinSize ? std::move(vin) : throw std::out_of_range("Invalid vehicle ID number")},
                                           mLogicalAddress{logicalAddress},
                                           mEid{eid},
                                           mGid{gid},
                                           mFurtherAction{furtherAction},
                                           mUseVehicleIdSync{true},
                                           mVinGidStatus{vinGidStatus}
            {
            }

            /// @brief Get VIN
            /// @return Vehicle ID number
            std::string GetVin() const{
                return mVin;
            }

            /// @brief Get logical address
            /// @return Entity logical address
            uint16_t GetLogicalAddress() const noexcept{
                return mLogicalAddress;
            }

            /// @brief Get EID
            /// @return Entity ID
            std::array<uint8_t, cIdSize> GetEid() const{
                return mEid;
            }

            /// @brief Get GID
            /// @return Group ID
            std::array<uint8_t, cIdSize> GetGid() const{
                return mGid;
            }

            /// @brief Get further action
            /// @return Further action byte
            uint8_t GetFurtherAction() const noexcept{
                return mFurtherAction;
            }

            /// @brief Try to get the VIN/GIN status
            /// @param[out] vinGinStatus VIN/GIN synchronization status
            /// @return True if the VIN/GIN synchronization status is being used, otherwise false
            bool TryGetVinGinStatus(uint8_t &vinGinStatus) const noexcept{
                if (mUseVehicleIdSync)
                {
                    vinGinStatus = mVinGidStatus;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };
//        constexpr PayloadType VehicleIdResponse::cPayloadType;
    }
}

#endif