//
// Created by PC on 21.07.2023.
//

#include "Gateway.h"

#include <utility>
using namespace KeCommon::Bsw::Can;
using std::shared_ptr;
namespace App
{
    Gateway::Gateway(shared_ptr<ICan> can1, shared_ptr<ICan> can2)
        : _can1(std::move(can1)), _can2(std::move(can2))
    {
    }
    void Gateway::ToEmuCanStream()
    {
    }
    void Gateway::MainFunction()
    {
    }
    void Gateway::SetGearAndMode(GearSelector::Gear gear, GearSelector::DriveMode mode)
    {
        _gear = gear;
        _mode = mode;
    }
    shared_ptr<ICan> Gateway::GetCan(uint8_t busId)
    {
        switch (busId) {
            case 1:
                return _can1;
            case 2:
                return _can2;
            default:
                return nullptr;
        }
    }
}// namespace App