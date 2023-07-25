//
// Created by PC on 21.07.2023.
//

#ifndef KEWJGWTCU_GATEWAY_H
#define KEWJGWTCU_GATEWAY_H

#include "CAN/ICan.h"
#include <memory>

namespace App
{
    //route signals from Powrtrain can to CANTCU

    /*3 frames:
     * 0x60F -> short[0]-> VE -> x10 czyli 70.0% -> 700
     *          byte[3] -> brake SWitch
     * 0x600 -> if (can_id == _EMUbase) {
//0-1 RPM in 16Bit unsigned
emu_data.RPM = (data[1] << 8) + data[0];
//2 TPS in /2 %
emu_data.TPS = data[2] * 0.5;
//3 IAT 8bit signed -40-127°C
emu_data.IAT = int8_t(data[3]);
//4-5 MAP 16Bit 0-600kpa
emu_data.MAP = (data[5] << 8) + data[4];
//6-7 INJPW 0-50 0.016129ms
emu_data.pulseWidth = ((data[7] << 8) + data[6]) * 0.016129;
     if (can_id == _EMUbase + 2) {
//0-1 VSPD in 16Bit unsigned 1 kmh/h / bit
emu_data.vssSpeed = (data[1] << 8) + data[0];
//2 BARO 50-130 kPa
emu_data.Baro = data[2];
//3 OILT 0-160°C
emu_data.oilTemperature = data[3];
//4 OILP BAR 0.0625 bar/bit
emu_data.oilPressure = data[4] * 0.0625;
//5 FUELP BAR 0.0625 bar/bit
emu_data.fuelPressure = data[5] * 0.0625;
//6-7 CLT 16bit Signed -40-250 1 C/bit
emu_data.CLT = int16_t(((data[7] << 8) + data[6]));
}
}
    */
    class Gateway
    {
    private:
        std::shared_ptr<KeCommon::Bsw::Can::ICan> _can1, _can2;
        void ToEmuCanStream();
    public:
        Gateway(std::shared_ptr<KeCommon::Bsw::Can::ICan> can1, std::shared_ptr<KeCommon::Bsw::Can::ICan> can2);
        void MainFunction();
    };

}// namespace App

#endif//KEWJGWTCU_GATEWAY_H
