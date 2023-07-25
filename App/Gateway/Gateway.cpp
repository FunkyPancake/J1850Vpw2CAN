//
// Created by PC on 21.07.2023.
//

#include "Gateway.h"

#include <utility>

namespace App
{
    Gateway::Gateway(std::shared_ptr<KeCommon::Bsw::Can::ICan> can1, std::shared_ptr<KeCommon::Bsw::Can::ICan> can2)
        : _can1(std::move(can1)), _can2(std::move(can2))
    {
    }
    void Gateway::ToEmuCanStream()
    {
    }
    void Gateway::MainFunction()
    {
    }
}// namespace App