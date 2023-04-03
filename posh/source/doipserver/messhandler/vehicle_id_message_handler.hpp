
#ifndef MVP_MODULES_VEHICLE_ID_MESSAGE_HANDLER_HPP
#define MVP_MODULES_VEHICLE_ID_MESSAGE_HANDLER_HPP

#include "message_handler.hpp"
#include "message/eid_vehicle_id_request.hpp"
#include "message/vehicle_id_response.hpp"

namespace motovis
{
    namespace doip
    {
        class VehicleIdMessageHandler : public MessageHandler
        {
        private:
            static constexpr uint8_t cFurtherAction{0x00};

            VehicleIdResponse mResponse;
            EidVehicleIdRequest mRequest;

        public:
            VehicleIdMessageHandler(
                    uint8_t protocolVersion,
                    std::string &&vin,
                    uint16_t logicalAddress,
                    std::array<uint8_t, cIdSize> eid,
                    std::array<uint8_t, cIdSize> gid)
                    : mResponse{protocolVersion, std::move(vin), logicalAddress, eid, gid, cFurtherAction}
            {
            }

            Message *GetMessage() override
            {
                return static_cast<Message *>(&mRequest);
            }

            bool TryHandle(
                    const Message *request, std::vector<uint8_t> &response) const override
            {
                auto _eidVehicleIdRequest{
                        dynamic_cast<const EidVehicleIdRequest *>(request)};

                // Validate the received EID within the request
                if (_eidVehicleIdRequest->GetEid() == mResponse.GetEid())
                {
                    mResponse.Serialize(response);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };
//        constexpr uint8_t VehicleIdMessageHandler::cFurtherAction;
    }
}

#endif //MVP_MODULES_VEHICLE_ID_MESSAGE_HANDLER_HPP
