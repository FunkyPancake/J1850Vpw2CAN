/*
 * Copyright (c) 2023, FunkyPuncake
 *    All rights reserved
 *    This file is part of the KeWjGwTcu. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the GPL license. You should have received
 *    a copy of the license with this file.
 *
 */

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
        _can1->RegisterRxFrame(0x200, [this](auto &&PH1) { Frame200Handler(std::forward<decltype(PH1)>(PH1)); });
        _can1->RegisterRxFrame(0x208, [this](auto &&PH1) { Frame208Handler(std::forward<decltype(PH1)>(PH1)); });
        _can2->RegisterCyclicTxFrame(outBaseId, 10);
        _can2->RegisterCyclicTxFrame(outBaseId + 1, 10);
        _can2->RegisterCyclicTxFrame(outBaseId + 2, 20);
        _can2->RegisterCyclicTxFrame(outBaseId + 3, 20);
    }
    void Gateway::MainFunction()
    {
        Payload payload;
        //pack signals in Syvecs stream
        payload = {.s{_rpm, 0, 0, 0}};
        _can2->UpdateCyclicFrame(outBaseId, payload);

        payload = {.s{_speedRL, _speedRR, _speedFL, _speedFR}};
        _can2->UpdateCyclicFrame(outBaseId + 1, payload);

        payload = {.s{_ppsDriver, _brake, 0x0, 0x0}};
        _can2->UpdateCyclicFrame(outBaseId + 2, payload);

        payload = {.s{static_cast<uint16_t>(_gear), static_cast<uint16_t>(_mode), 0x0, 0x0}};
        _can2->UpdateCyclicFrame(outBaseId + 3, payload);
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
    void Gateway::Frame200Handler(CanFrame frame)
    {
        _brake = frame.payload.b[1] & 0x3;
        _speedFL = frame.payload.s[1];
        _speedFR = frame.payload.s[2];
    }
    void Gateway::Frame208Handler(CanFrame frame)
    {
        _speedRL = frame.payload.s[2];
        _speedRR = frame.payload.s[3];
    }
}// namespace App