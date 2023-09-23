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
// Created by PC on 12.07.2023.
//

#include "GearSelector.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

using namespace App;

App::GearSelector::SelectorPosition App::GearSelector::GetCurrentPosition()
{
    auto c2 = 0;//GPIO_PinRead(BOARD_INITPINS_C2_GPIO, BOARD_INITPINS_C1_PIN);
    auto c3 = 0;//GPIO_PinRead(BOARD_INITPINS_C3_GPIO, BOARD_INITPINS_C2_PIN);
    auto c4 = 0;//GPIO_PinRead(BOARD_INITPINS_C4_GPIO, BOARD_INITPINS_C3_PIN);
    auto selectorPosition = static_cast<SelectorPosition>(c4 + (c3 << 1) + (c2 << 2));
    return selectorPosition;
}

App::GearSelector::Gear App::GearSelector::GetGear()
{
    return _gear;
}

App::GearSelector::DriveMode App::GearSelector::GetDriveMode()
{
    return _driveMode;
}

void App::GearSelector::MainFunction()
{
    auto position = GetCurrentPosition();
    ControlParkSelenoid();
    switch (position)
    {
        case SelectorPosition::P:
            _gear = Gear::P;
            _driveMode = DriveMode::Eco;
            break;
        case SelectorPosition::N:
            _gear = Gear::N;
            _driveMode = DriveMode::Eco;
            break;
        case SelectorPosition::D:
            _gear = Gear::D;
            _driveMode = DriveMode::Eco;
            break;
        case SelectorPosition::D1:
            _gear = Gear::D;
            _driveMode = DriveMode::Sport;
            break;
        case SelectorPosition::D2:
            _gear = Gear::D;
            _driveMode = DriveMode::SportPlus;
            break;
        case SelectorPosition::D3:
        case SelectorPosition::D4:
            _gear = Gear::D;
            _driveMode = DriveMode::Comfort;
            break;
        default:
            _gear = Gear::N;
            _driveMode = DriveMode::Eco;
            //TODO: add error notification to diag module
    }
}

void GearSelector::ControlParkSelenoid()
{
    if (GetSpeed() < ParkSelenoidSpeedThreshold && GetBrake())
    {
        _parkSelenoidControlState = true;
        SetOutput(true);
        _parkSelenoidTimer = ParkSelenoidTimerReloadValue;
    } else
    {
        if (_parkSelenoidTimer != 0)
        {
            _parkSelenoidTimer--;
            return;
        }
        _parkSelenoidControlState = false;
        SetOutput(false);
    }

}

GearSelector::GearSelector(const std::function<int16_t()> &getSpeed, const std::function<bool()> &getBrake,
                           const std::function<void(bool)> &setOutput) : GetSpeed(getSpeed), GetBrake(getBrake),
                                                                         SetOutput(setOutput)
{

}
