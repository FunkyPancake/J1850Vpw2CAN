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

#ifndef J1850_VPW_H
#define J1850_VPW_H

#include <cstdint>
#include <array>


/*Lazy implementation - viable only for single instance, based on FTM3
 * CH3 -> Rx
 * CH2 -> Tx
 * CH0 -> SOF/EOF detection
 */

namespace App::CDD {
    class Vpw {
    public:
        Vpw();

        std::vector<uint8_t> GetData();

        void SendData(const std::vector<uint8_t> &data);

    private:
        static constexpr int MaxDataSize = 12;
        static const std::array<uint8_t, 256> crcTable_;
        std::vector<uint8_t> _txBuffer = std::vector<uint8_t>(MaxDataSize + 4);
        std::vector<uint8_t> _rxBuffer = std::vector<uint8_t>(MaxDataSize + 4);

        uint32_t OnTimerEvent(uint32_t status);

        uint8_t CalcCrc(const std::vector<uint8_t> &data);
    };
}// namespace App::J1850Vpw

#endif//J1850_VPW_H
