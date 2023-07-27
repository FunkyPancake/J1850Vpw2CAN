/*
 * Copyright (c) 2023, FunkyPuncake
 *    All rights reserved
 *    This file is part of the KeWjGwTcu. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the GPL license. You should have received
 *    a copy of the license with this file.
 *
 */

#ifndef KEWJGWTCU_GATEWAY_H
#define KEWJGWTCU_GATEWAY_H

#include "CAN/ICan.h"
#include "FreeRTOS.h"
#include "GearSelector.h"
#include "semphr.h"
#include <memory>
namespace App
{
    class Gateway
    {
    private:
        static constexpr int outBaseId{0x600};
        SemaphoreHandle_t _mutex;
        uint16_t _speedFR, _speedFL, _speedRR, _speedRL, _rpm, _ppsDriver;
        uint8_t _brake;

        GearSelector::Gear _gear;
        GearSelector::DriveMode _mode;
        std::shared_ptr<KeCommon::Bsw::Can::ICan> _can1, _can2;
        KeCommon::Bsw::Can::CanFrame canFrame;
        void Frame200Handler(KeCommon::Bsw::Can::CanFrame frame);
        void Frame208Handler(KeCommon::Bsw::Can::CanFrame frame);

    public:
        Gateway(std::shared_ptr<KeCommon::Bsw::Can::ICan> can1, std::shared_ptr<KeCommon::Bsw::Can::ICan> can2);
        void SetGearAndMode(GearSelector::Gear gear, GearSelector::DriveMode mode);
        void MainFunction();
        std::shared_ptr<KeCommon::Bsw::Can::ICan> GetCan(uint8_t busId);
    };

}// namespace App

#endif//KEWJGWTCU_GATEWAY_H
