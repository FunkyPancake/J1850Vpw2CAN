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

#ifndef KEWJGWTCU_GATEWAY_H
#define KEWJGWTCU_GATEWAY_H

#include "CAN/ICan.h"
#include "FreeRTOS.h"
#include "GearSelector.h"
#include "J1850Vpw.h"
#include "semphr.h"
#include <memory>

namespace App {
    class Gateway {
    private:
        SemaphoreHandle_t _mutex;
        uint16_t speedFR_{}, speedFL_{}, speedRR_{}, speedRL_{}, rpm_{}, ppsDriver_{}, oilTemp_{}, coolantTemp_{};
        uint16_t engineLoad{}, boostPressure{};
        uint16_t torqueAsVe_{};
        uint8_t _brake{};
        GearSelector::Gear _gear{GearSelector::Gear::N};
        GearSelector::DriveMode _mode{GearSelector::DriveMode::Comfort};
        CDD::Vpw _vpw;
        int MutexWaitTicks{100};

        std::shared_ptr<KeCommon::Bsw::Can::ICan> _can1, _can2;

        void OnNewFrame200(const KeCommon::Bsw::Can::CanFrame &frame);

        void OnNewFrame208(const KeCommon::Bsw::Can::CanFrame &frame);

        void OnNewFrame300(const KeCommon::Bsw::Can::CanFrame &frame);

        void OnNewFrame301(const KeCommon::Bsw::Can::CanFrame &frame);

        void UpdateFrame218();

        void UpdateFrame520();

        void UpdateFrame523();

        void UpdateFrame525();

        void UpdateFrame526();

        void UpdateFrame530();

        void UpdateFrame536();

        void UpdateFrame550();

    public:

        Gateway(std::shared_ptr<KeCommon::Bsw::Can::ICan> can1, std::shared_ptr<KeCommon::Bsw::Can::ICan> can2);

        void SetGearAndMode(GearSelector::Gear gear, GearSelector::DriveMode mode);

        void MainFunction();

        bool GetBrakePressed() const;

        int16_t GetAverageSpeed() const;

        uint16_t GetRpm();
    };

}// namespace App

#endif//KEWJGWTCU_GATEWAY_H
