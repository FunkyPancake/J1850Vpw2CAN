//
// Created by PC on 14.09.2023.
//

#include "StarterControl.h"

namespace App {

    void StarterControl::MainFunction()
    {
        GearSelector::Gear gear;
        switch (_state)
        {
            case State::Idle:

                gear = GetGear();
                if (GetStartRequest() && GetRpm() == 0 &&
                    (gear == GearSelector::Gear::P || gear == GearSelector::Gear::N))
                {
                    _counter = 0;
                    _state = State::Actuate;
                    SetOutput(true);
                }
                break;
            case State::Actuate:
                if (!GetStartRequest() || _counter > EnableTimeout || GetRpm() > RpmThreshold)
                {
                    _state = State::Cooldown;
                    _counter = 0;
                    SetOutput(false);
                } else
                {
                    _counter++;
                }
                break;

            case State::Cooldown:
                if (_counter > CooldownTimeout)
                {
                    _state = State::Idle;
                }
                break;

        }
    }

    StarterControl::StarterControl(const std::function<uint16_t()> &getRpm,
                                   const std::function<bool()> &getStartRequest,
                                   const std::function<GearSelector::Gear()> &getGear,
                                   const std::function<void(bool)> &setOutput) : GetStartRequest(getStartRequest),
                                                                                 GetRpm(getRpm),
                                                                                 GetGear(getGear),
                                                                                 SetOutput(setOutput)
    {
        SetOutput(false);
    }
}