#include <functional>
#include <cstring>
#include "J1850Vpw.h"
#include "fsl_ftm.h"
#include "FTM.h"
#include "pin_mux.h"
#include "fsl_gpio.h"


using namespace App::CDD;

constexpr std::array<uint8_t, 256> Vpw::_crcTable = {0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53, 0xE8, 0xF5, 0xD2,
                                                     0xCF, 0x9C, 0x81, 0xA6, 0xBB, 0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4,
                                                     0x83, 0x9E, 0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76, 0x87,
                                                     0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4, 0x6F, 0x72, 0x55, 0x48,
                                                     0x1B, 0x06, 0x21, 0x3C, 0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04,
                                                     0x19, 0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1, 0x13, 0x0E,
                                                     0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40, 0xFB, 0xE6, 0xC1, 0xDC, 0x8F,
                                                     0x92, 0xB5, 0xA8, 0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
                                                     0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65, 0x94, 0x89, 0xAE,
                                                     0xB3, 0xE0, 0xFD, 0xDA, 0xC7, 0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15,
                                                     0x32, 0x2F, 0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A, 0xB1,
                                                     0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2, 0x26, 0x3B, 0x1C, 0x01,
                                                     0x52, 0x4F, 0x68, 0x75, 0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80,
                                                     0x9D, 0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8, 0x03, 0x1E,
                                                     0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50, 0xA1, 0xBC, 0x9B, 0x86, 0xD5,
                                                     0xC8, 0xEF, 0xF2, 0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
                                                     0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F, 0x84, 0x99, 0xBE,
                                                     0xA3, 0xF0, 0xED, 0xCA, 0xD7, 0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C,
                                                     0x7B, 0x66, 0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E, 0xF8,
                                                     0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB, 0x10, 0x0D, 0x2A, 0x37,
                                                     0x64, 0x79, 0x5E, 0x43, 0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC,
                                                     0xE1, 0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09, 0x7F, 0x62,
                                                     0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C, 0x97, 0x8A, 0xAD, 0xB0, 0xE3,
                                                     0xFE, 0xD9, 0xC4};


Vpw::Vpw()
{
    auto f = [this](auto &&PH1) { return OnTimerEvent(std::forward<decltype(PH1)>(PH1)); };
    KeCommon::Bsw::Timers::FTM::getInstance().RegisterTimerCallback(FTM3, f);
    //make sure channels are effectively disabled by setting compare value above counter max
    FTM3->COMBINE = 0;
//    FTM3->FILTER |= FTM_FILTER_CH3FVAL(3);
    *OutputChnCompareValue = CompareDisabled;
    *EofChnCompareValue = CompareDisabled;
    FTM3->MODE = FTM_MODE_INIT_MASK;
    _txStatus = Status::Idle;
    ResetRx();
    FTM_StartTimer(FTM3, kFTM_FixedClock);
}

inline void Vpw::SetTimerAlarm(volatile uint32_t *counterRegister, uint32_t value)
{
    if (value > CounterMax)
    {
        value -= CounterMax;
    }
    *counterRegister = value;
}

inline uint16_t Vpw::GetPulseWidth(uint16_t a, uint16_t b)
{
    if (b < a)
    {
        return b + CounterMax - a;
    }
    return b - a;
}

inline void Vpw::ResetRx()
{
    _rxStatus = Status::Idle;
    _rxMessageBuffer.Len = 0;
    *EofChnCompareValue = CompareDisabled;
}


uint32_t Vpw::OnTimerEvent(uint32_t status)
{
    GPIO_PinWrite(BOARD_INITPINS_LED2_GPIO, BOARD_INITPINS_LED2_PIN, 0);
    uint32_t eventsToClear = 0;
    uint8_t bit = 0;
    uint32_t width;
    uint32_t curVal = FTM3->CNT;

    if (status & kFTM_Chnl3Flag)
    {
        eventsToClear |= kFTM_Chnl3Flag;


//        if (_rxStatus == Status::Idle && (FTM3->CONTROLS[3].CnSC & FTM_CnSC_CHIS_MASK) == FTM_CnSC_CHIS_MASK)
        if (_rxStatus == Status::Idle)
        {
            _rxStatus = Status::Active;
        } else
        {
            int32_t sym = -1;
            width = GetPulseWidth(PrevCntrVal, curVal);
            if (width > RX_SOF_MIN && width <= RX_SOF_MAX)
            {
                _rxMessageBuffer.Len = 0;
                _rxBuffByte = 0;
                _rxBitInBytePos = 7;
                _rxLastBit = 1;
                _rxLastSym = 1;
                bit = 1;
            } else if (width > RX_LONG_MIN && width <= RX_LONG_MAX)
            {
                sym = LONG_IDX;
            } else if (width > RX_SHORT_MIN && width <= RX_SHORT_MAX)
            {
                sym = SHORT_IDX;
            }
            if (sym != -1)
            {
                bit = (sym == _rxLastSym) ? !_rxLastBit : _rxLastBit;
                _rxLastBit = bit;
                _rxLastSym = sym;
                _rxBuffByte |= bit << _rxBitInBytePos;
                _rxBitInBytePos--;

                if (_rxBitInBytePos < 0)
                {
                    _rxMessageBuffer.Data[_rxMessageBuffer.Len] = _rxBuffByte;
                    _rxMessageBuffer.Len++;
                    _rxBuffByte = 0;
                    _rxBitInBytePos = 7;
                }
            }
        }
        switch (_txStatus)
        {
            case Status::Sof:
            {
                _txBitInBytePos = 7;
                _txLastSym = 1;
                _txLastBit = 1;
                _txBuffByte = 0;
                _txStatus = Status::Active;
                SetTimerAlarm(OutputChnCompareValue, curVal + US2TICKS(SOF_US));
            }
                break;
            case Status::Active:
            {
                uint8_t txBit = (_txMessageBuffer.Data[_txBuffByte] >> _txBitInBytePos) & 1;
                uint8_t txSym = txBit == _txLastBit ? !_txLastSym : _txLastSym;
//                if ( txBit!= bit)
//                {
//                    _txStatus = Status::Failed;
//                    *OutputChnCompareValue = CompareDisabled;
//                } else
                {
                    _txLastBit = txBit;
                    _txLastSym = txSym;
                    auto pulseLen = txSym ? US2TICKS(SHORT_US) : US2TICKS(LONG_US);

                    if (_txBitInBytePos == 0 && _txBuffByte == _txMessageBuffer.Len)
                    {
                        _txStatus = Status::Eof;
                    } else
                    {
                        _txBitInBytePos--;
                        if (_txBitInBytePos < 0)
                        {
                            _txBuffByte++;
                            _txBitInBytePos = 7;
                        }
                    }

                    SetTimerAlarm(OutputChnCompareValue, curVal + pulseLen);
                }
            }
                break;
            case Status::Eof:
            {
                *OutputChnCompareValue = CompareDisabled;
                _txStatus = Status::Done;
            }
                break;
            default:
                break;
        }

        SetTimerAlarm(EofChnCompareValue, curVal + RX_EOF_MIN);
        PrevCntrVal = curVal;
    }
    if (status & kFTM_Chnl0Flag)
    {
        eventsToClear |= kFTM_Chnl0Flag;
        if (_txStatus != Status::Done)
        {
            _rxFifo.push(_rxMessageBuffer);
        }
        *OutputChnCompareValue = CompareDisabled;
        ResetRx();

        switch (_txStatus)
        {
            case Status::Idle:
            case Status::Done:
                if (_txFifo.empty())
                {
                    _txStatus = Status::Idle;
                } else
                {
                    _txMessageBuffer = _txFifo.front();
                    _txFifo.pop();
                    StartTx();
                }
                break;
            case Status::Failed:
                StartTx();
                break;
            default:
                _txStatus = Status::Idle;
                break;
        }
    }
    GPIO_PinWrite(BOARD_INITPINS_LED2_GPIO, BOARD_INITPINS_LED2_PIN, 1);

    return eventsToClear;
}

uint8_t Vpw::CalcCrc(const std::vector<uint8_t> &data)
{
    uint8_t crc = 0xff;
    for (auto b: data)
    {
        /* XOR-in next input byte */
        uint8_t byte = (b ^ crc);
        /* get current CRC value = remainder */
        crc = (_crcTable[byte]);
    }
    crc ^= 0xff;
    return crc;
}

std::vector<uint8_t> Vpw::GetData()
{
    return {};
}

void Vpw::SendData(const std::vector<uint8_t> &data)
{

    auto crc = CalcCrc(data);
    Message msg = {.Len = static_cast<uint8_t>(data.size())};
    std::memcpy(msg.Data.data(), data.data(), data.size());
    msg.Data[data.size()] = crc;

    if (_rxStatus == Status::Idle && _txStatus == Status::Idle)
    {
        _txMessageBuffer = msg;
        StartTx();
    } else
    {
        _txFifo.push(msg);
    }
}

void Vpw::StartTx()
{
    FTM3->MODE = FTM_MODE_INIT_MASK;
    _txStatus = Status::Sof;
    auto curVal = FTM3->CNT;
    SetTimerAlarm(OutputChnCompareValue, curVal + 10);
}

void Vpw::VpwMainFunction()
{

}

void Vpw::SendPeriodic(Vpw::Message &message, int period)
{

}
