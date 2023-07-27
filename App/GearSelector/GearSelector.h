/*
 * Copyright (c) 2023, FunkyPuncake
 *    All rights reserved
 *    This file is part of the KeWjGwTcu. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the GPL license. You should have received
 *    a copy of the license with this file.
 *
 */

#ifndef KEWJGWTCU_GEARSELECTOR_H
#define KEWJGWTCU_GEARSELECTOR_H
namespace App
{
    class GearSelector
    {

    public:
        enum class Gear
        {
            P,
            R,
            N,
            D
        };
        enum class DriveMode
        {
            Eco,
            Comfort,
            Sport,
            SportPlus
        };
        Gear GetGear();
        DriveMode GetDriveMode();
        void MainFunction();

    private:
        enum class SelectorPosition
        {
            P = 0b110,
            R = 0b111,
            N = 0b011,
            D = 0b010,
            D4 = 0b001,
            D3 = 0b100,
            D2 = 0b000,
            D1 = 0b101
        };
        SelectorPosition GetCurrentPosition();
        Gear _gear;
        DriveMode _driveMode;
    };
}// namespace App

#endif//KEWJGWTCU_GEARSELECTOR_H
