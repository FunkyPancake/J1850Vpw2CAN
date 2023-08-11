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
#include "fsl_gpio.h"
#include "peripherals.h"
#include "pin_mux.h"
#include <memory>

#define MS2TICKS(x) (configTICK_RATE_HZ / 1000 * (x))

using namespace KeCommon::Bsw::Can;
using namespace KeCommon::Bsw::Diag;
using namespace App;

constexpr UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);

//For MVP use single task only
[[noreturn]] static void appTask(void *pvParameters);

[[noreturn]] static void appTask(void *pvParameters)
{
    constexpr TickType_t xFrequency = MS2TICKS(10);
    auto can1 = std::make_shared<FlexCan>(CAN0, 16);
    auto can2 = std::make_shared<FlexCan>(CAN1, 16);
    auto gateway = Gateway(can1, can2);
    LpSpiRtos sbcSpi{&LPSPI0_handle};
    Tle9461 sbc{&sbcSpi};

    auto canTp = CanTp{*can1};
    auto uds = Uds{&canTp};
    auto led = 1;
    App::GearSelector gearSelector{};

    TickType_t xLastWakeTime = xTaskGetTickCount();
    sbc.Init();
    sbc.ConfigWatchdog(Tle9461::WgTimer200ms);

    for (;;) {
        led ^= 1;
        GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, led);
        can1->RxTask();
        can2->RxTask();
        uds.MainFunction();

        gearSelector.MainFunction();
        gateway.SetGearAndMode(gearSelector.GetGear(), gearSelector.GetDriveMode());

        gateway.MainFunction();

        canTp.TxMainFunction();
        can1->TxTask();
        can2->TxTask();
        sbc.RefreshWatchdog();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void AppTasksInit()
{

    if (xTaskCreate(appTask, "appTask", configMINIMAL_STACK_SIZE + 512, nullptr, app_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}