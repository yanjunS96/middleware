//
// Created by songyanjun on 2023/3/27.
//

#ifndef MVP_MODULES_DOIP_CONTROL_H
#define MVP_MODULES_DOIP_CONTROL_H

#include "doip_config.hpp"
#include "./server/DoIPServer.h"

#include "messhandler/message_handler.hpp"

//#include "./server/doip_parse.hpp"

namespace motovis{
    namespace doip
    {
        class DoipController
        {
        public:
            DoipController(const DoIpInfoPtr) noexcept;
            virtual ~DoipController() noexcept;

        protected:
        private:
            ServerPtr pServer;
            DoIpParsePtr pDoIpParse;
        };
    }
}

#endif //MVP_MODULES_DOIP_CONTROL_H
