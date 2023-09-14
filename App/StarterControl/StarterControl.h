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

//
// Created by PC on 14.09.2023.
//

#ifndef STARTER_H
#define STARTER_H

#include "GearSelector.h"
#include "functional"

namespace App {
    class StarterControl
    {
    public:
        StarterControl(const std::function<uint16_t()> &getRpm, const std::function<bool()> &getStartRequest,
                       const std::function<GearSelector::Gear()> &getGear, const std::function<void(bool)> &setOutput);

        void MainFunction();

    private:
        constexpr static uint16_t EnableTimeout{30000};
        constexpr static uint16_t CooldownTimeout{5000};
        constexpr static uint16_t RpmThreshold{500};
        enum class State
        {
            Idle,
            Actuate,
            Cooldown
        };
        State _state{State::Idle};
        std::function<bool()> GetStartRequest;
        std::function<uint16_t()> GetRpm;
        std::function<GearSelector::Gear()> GetGear;
        std::function<void(bool)> SetOutput;
        uint16_t _counter{0};

    };
}

#endif //STARTER_H
