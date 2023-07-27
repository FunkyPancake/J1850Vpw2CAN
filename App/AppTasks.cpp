/*
 * Copyright (c) 2023, FunkyPuncake
 *    All rights reserved
 *    This file is part of the KeWjGwTcu. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the GPL license. You should have received
 *    a copy of the license with this file.
 *
 */

#include "AppTasks.h"
#include "../KeCommon/Bsw/Diag/CanTp.h"
#include "../KeCommon/Bsw/Diag/Uds.h"
#include "FlexCan.h"
#include "FreeRTOS.h"
#include "Gateway.h"
#include "GearSelector/GearSelector.h"
#include "LpSpi.h"
#include "LpUart.h"
#include "Tle9461.h"
#include "peripherals.h"
#include "pin_mux.h"
#include <memory>

#define MS2TICKS(x) (configTICK_RATE_HZ / 1000 * (x))

using namespace KeCommon::Bsw::Can;
using namespace KeCommon::Bsw::Diag;
using namespace App;

constexpr UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);
constexpr UBaseType_t cal_task_PRIORITY = (configMAX_PRIORITIES - 2);

[[noreturn]] static void comTask(void *pvParameters);

[[noreturn]] static void appTask(void *pvParameters);

[[noreturn]] static void appTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = MS2TICKS(100);
    auto gateway = reinterpret_cast<Gateway *>(pvParameters);
    xLastWakeTime = xTaskGetTickCount();

    App::GearSelector gearSelector{};
    auto led = 1;
    for (;;) {
        led ^= 1;
        GPIO_PinWrite(BOARD_INITPINS_D2_GPIO, BOARD_INITPINS_D2_PIN, led);
        gearSelector.MainFunction();
        gateway->SetGearAndMode(gearSelector.GetGear(), gearSelector.GetDriveMode());

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
[[noreturn]] static void comTask(void *pvParameters)
{
    constexpr TickType_t xFrequency = MS2TICKS(5);

    LpSpiRtos sbcSpi{&LPSPI0_handle};
    Tle9461 sbc{&sbcSpi};

    auto gateway = reinterpret_cast<Gateway *>(pvParameters);
    auto can1 = gateway->GetCan(1);
    auto can2 = gateway->GetCan(2);
    auto canTp = CanTp{*can1};
    auto uds = Uds{&canTp};
    auto led = 1;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    sbc.Init();
    sbc.ConfigWatchdog(Tle9461::WgTimer200ms);
    for (;;) {
        led ^= 1;
        GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, led);
        can1->RxTask();
        can2->RxTask();
        uds.MainFunction();
        gateway->MainFunction();
        canTp.TxMainFunction();
        can1->TxTask();
        can2->TxTask();
        sbc.RefreshWatchdog();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void AppTasksInit()
{
    static std::shared_ptr<ICan> can1 = std::make_shared<FlexCan>(CAN0, 16);
    static std::shared_ptr<ICan> can2 = std::make_shared<FlexCan>(CAN1, 16);
    static std::shared_ptr<Gateway> gw = std::make_shared<Gateway>(can1, can2);
    if (xTaskCreate(appTask, "appTask", configMINIMAL_STACK_SIZE + 256, (void *) &gw, app_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }

    if (xTaskCreate(comTask, "comTask", configMINIMAL_STACK_SIZE + 256, (void *) &gw, cal_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}