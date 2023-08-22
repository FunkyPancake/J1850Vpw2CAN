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
#include "fsl_device_registers.h"


/*Lazy implementation - viable only for single instance, based on FTM3
 * CH3 -> Rx
 * CH2 -> Tx
 * CH0 -> SOF/EOF detection
 */

namespace App::CDD {
#define RX_BUFLEN   12
#define TX_BUFLEN   12
#define US2TICKS(x) ((uint16_t)(x*3))

/*timing thresholds for symbols*/
#define RX_SHORT_MIN US2TICKS(34)
#define RX_SHORT_MAX US2TICKS(96)
#define RX_LONG_MIN  US2TICKS(96)
#define RX_LONG_MAX  US2TICKS(163)
#define RX_SOF_MIN   US2TICKS(163)
#define RX_SOF_MAX   US2TICKS(239)
#define RX_EOF_MIN   US2TICKS(239)
#define SOF_US       200
#define LONG_US      128
#define SHORT_US     64
#define EOF_US       200

#define LONG_IDX  0
#define SHORT_IDX 1
#define SOF_IDX   2
#define EOF_IDX   3


    typedef enum {
        Idle,
        SOF,
        Acvtive,
        Done,
        Error
    } VPW_RxStatus_t;

    class Vpw {
    public:
        Vpw();

        std::vector<uint8_t> GetData();

        void SendData(const std::vector<uint8_t> &data);

    private:
        static constexpr int MaxDataSize = 12;
        static const std::array<uint8_t, 256> _crcTable;
        std::vector<uint8_t> _txBuffer = std::vector<uint8_t>(MaxDataSize + 4);
        std::vector<uint8_t> _rxBuffer = std::vector<uint8_t>(MaxDataSize + 4);
        uint8_t VPW_RxBuf[(RX_BUFLEN + 1) * 8];
        uint8_t VPW_TxBuf[(TX_BUFLEN + 1) * 8];

        void J1850VPW_ByteToBits(uint8_t *byteBuf, uint16_t len);

        uint32_t OnTimerEvent(uint32_t status);

        uint8_t CalcCrc(const std::vector<uint8_t> &data);

        uint8_t CalcCRC(uint8_t *data, uint8_t size);

        bool _txInProgress{false};
        volatile uint32_t *OutputChnCompareValue{&FTM3->CONTROLS[2].CnV};
        volatile uint32_t *EofChnCompareValue{&FTM3->CONTROLS[0].CnV};

        void SetTimerAlarm(volatile uint32_t *counterRegister, uint32_t value);

        uint16_t GetPulseWidth(uint16_t a, uint16_t b);

        void ResetRx(void);

        void FinalizeTx(void);

        int RxInProgress{0};
        int VPW_RxBufPtr{0};
        int VPW_TxBufPtr{0};
        int TxInProgress{0};

        uint8_t J1850_Transmit(uint8_t *byteBuf, uint16_t len);

        uint8_t J1850_Recieve(uint8_t *byteBuf, uint16_t *len);

        uint16_t J1850VPW_BitsToByte(uint8_t *byteBuf);

        uint32_t PrevCntrVal{0};
        const uint32_t CounterMax = 0xfffe;
    };
}// namespace App::J1850Vpw

#endif//J1850_VPW_H
