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
// Created by PC on 21.07.2023.
//

#include "Gateway.h"

#include <utility>

using namespace KeCommon::Bsw::Can;
using std::shared_ptr;
namespace App {
    /**
     * @brief
     * @param [in] can1 -> Engine and ABS can bus
     * @param [in] can2 -> CanTcu can bus
     * */
    Gateway::Gateway(shared_ptr <ICan> can1, shared_ptr <ICan> can2)
            : _vpw(CDD::Vpw()), _can1(std::move(can1)), _can2(std::move(can2)) {
        _mutex = xSemaphoreCreateMutex();
        _can1->RegisterRxFrame(0x200, [this](auto &&PH1) { OnNewFrame200(std::forward<decltype(PH1)>(PH1)); });
        _can1->RegisterRxFrame(0x208, [this](auto &&PH1) { OnNewFrame208(std::forward<decltype(PH1)>(PH1)); });
        _can1->RegisterRxFrame(0x300, [this](auto &&PH1) { OnNewFrame300(std::forward<decltype(PH1)>(PH1)); });
        _can1->RegisterRxFrame(0x301, [this](auto &&PH1) { OnNewFrame301(std::forward<decltype(PH1)>(PH1)); });

        _can1->RegisterCyclicTxFrame(0x218, 8, 20);

        _can2->RegisterCyclicTxFrame(0x520, 8, 20);
        _can2->RegisterCyclicTxFrame(0x523, 8, 20);
        _can2->RegisterCyclicTxFrame(0x525, 8, 20);
        _can2->RegisterCyclicTxFrame(0x526, 8, 20);
        _can2->RegisterCyclicTxFrame(0x530, 8, 20);
        _can2->RegisterCyclicTxFrame(0x536, 8, 20);
        _can2->RegisterCyclicTxFrame(0x530, 8, 20);
        _can2->RegisterCyclicTxFrame(0x550, 8, 20);

    }

    void Gateway::MainFunction() {
        UpdateFrame218();

        UpdateFrame520();
        UpdateFrame523();
        UpdateFrame525();
        UpdateFrame526();
        UpdateFrame530();
        UpdateFrame536();
        static int i = 0;
        if (i > 20) {
            _vpw.SendData({0x8D, 0x93, 0x01, 0x01, 0x80});
            i = 0;
        } else {
            i++;
        }
        UpdateFrameBase550();
    }

    void Gateway::SetGearAndMode(GearSelector::Gear gear, GearSelector::DriveMode mode) {
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        _gear = gear;
        _mode = mode;
//            xSemaphoreGive(_mutex);
//        }

    }

    void Gateway::OnNewFrame200(const KeCommon::Bsw::Can::CanFrame &frame) {
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        _brake = frame.payload.b[1] & 0x3;
        speedFL_ = ICan::SwapBytes<uint16_t>(frame.payload.s[1]);
        speedFR_ = ICan::SwapBytes<uint16_t>(frame.payload.s[2]);
//            xSemaphoreGive(_mutex);
//        }
    }

    void Gateway::OnNewFrame208(const KeCommon::Bsw::Can::CanFrame &frame) {
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        speedRL_ = ICan::SwapBytes<uint16_t>(frame.payload.s[2]);
        speedRR_ = ICan::SwapBytes<uint16_t>(frame.payload.s[3]);
//            xSemaphoreGive(_mutex);
//        }
    }

    void Gateway::OnNewFrame300(const KeCommon::Bsw::Can::CanFrame &frame) {
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        rpm_ = ((frame.payload.b[0] << 8) + frame.payload.b[1]) * 2;
        ppsDriver_ = ((frame.payload.b[2] << 8) + frame.payload.b[3]) * 1000 / 4095;
        torqueAsVe_ = ((frame.payload.b[4] << 8) + frame.payload.b[5]) * 1000 / 4095;
        boostPressure = ((frame.payload.b[6] << 8) + frame.payload.b[7]) * 10 / 8;
//            xSemaphoreGive(_mutex);
//        }
    }

    void Gateway::OnNewFrame301(const KeCommon::Bsw::Can::CanFrame &frame) {
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        oilTemp_ = ((frame.payload.b[2] << 8) + frame.payload.b[3]) * 10 / 16;
        coolantTemp_ = ((frame.payload.b[6] << 8) + frame.payload.b[7]) * 10 / 16;
//            xSemaphoreGive(_mutex);
//        }
    }

    void Gateway::UpdateFrame218() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
//            xSemaphoreGive(_mutex);
//        }
        _can1->UpdateCyclicFrame(0x218, payload);
    }

    void Gateway::UpdateFrame520() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.s[0] = rpm_;
        payload.s[1] = ppsDriver_;
        payload.s[2] = boostPressure;
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x520, payload);
    }

    void Gateway::UpdateFrame523() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.s[0] = (speedFL_ + speedFR_) / 2;
        payload.s[1] = (speedRL_ + speedRR_) / 2;
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x523, payload);
    }

    void Gateway::UpdateFrame525() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.s[0] = torqueAsVe_;
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x525, payload);
    }

    void Gateway::UpdateFrame526() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.b[1] = _brake;
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x526, payload);
    }

    void Gateway::UpdateFrame530() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.s[3] = coolantTemp_;
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x530, payload);
    }

    void Gateway::UpdateFrame536() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.s[3] = oilTemp_;
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x536, payload);
    }

    void Gateway::UpdateFrameBase550() {
        Payload payload{};
//        if (xSemaphoreTake(_mutex, (TickType_t) MutexWaitTicks) == pdTRUE) {
        payload.b[0] = static_cast<uint16_t>(_gear);
        payload.b[1] = static_cast<uint16_t>(_mode);
//            xSemaphoreGive(_mutex);
//        }
        _can2->UpdateCyclicFrame(0x550, payload);
    }

    bool Gateway::GetBrakePressed() const {
        return _brake == 1;
    }

    int16_t Gateway::GetAverageSpeed() const {
        return static_cast<int16_t>((speedRL_ + speedRR_) / 2);
    }

}// namespace App