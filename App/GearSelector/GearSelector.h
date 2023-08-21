/*
 * Copyright (c) 2023, FunkyPuncake
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEWJGWTCU_GEARSELECTOR_H
#define KEWJGWTCU_GEARSELECTOR_H

#include <cstdint>

namespace App
{
    class GearSelector
    {

    public:
        enum class Gear
        {
            P = 0x1,
            R = 0x2,
            N = 0x4,
            D = 0x8
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

        void ControlParkSelenoid();

        void SetSpeedAndBrake(int16_t speed, bool brake);

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
        const int16_t ParkSelenoidSpeedThreshold = 5;
        const int16_t ParkSelenoidTimerReloadValue = 10;
        int16_t _parkSelenoidTimer{ParkSelenoidTimerReloadValue};
        int16_t _speed;
        bool _parkSelenoidControlState{false};
        bool _brake;
    };
}// namespace App

#endif//KEWJGWTCU_GEARSELECTOR_H
