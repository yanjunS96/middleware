//
// Created by songyanjun on 2023/3/28.
//

#include "doip_control.h"

using namespace motovis::doip;

DoipController::DoipController(const DoIpInfoPtr _info) noexcept
:pDoIpParse(new DoIpParse(_info))
, pServer(new DoIPServer(_info))
{
    //开启监听服务
    pServer->StartServer(pDoIpParse);
}

DoipController::~DoipController() noexcept
{
    MLOG_CRITICAL("~DoipController");
}

