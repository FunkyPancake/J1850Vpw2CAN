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
#include <memory>

#define MS2TICKS(x) (configTICK_RATE_HZ / 1000 * (x))

using namespace KeCommon::Bsw::Can;
using namespace KeCommon::Bsw::Diag;
using namespace App;

const UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);
const UBaseType_t cal_task_PRIORITY = (configMAX_PRIORITIES - 4);

[[noreturn]] static void cal_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = MS2TICKS(100);
    auto gateway = reinterpret_cast<Gateway *>(pvParameters);
    xLastWakeTime = xTaskGetTickCount();
    LpSpiRtos sbcSpi{&LPSPI0_handle};
    Tle9461 sbc{&sbcSpi};
    sbc.Init();
    sbc.ConfigWatchdog(Tle9461::WgTimer200ms);
    App::GearSelector gearSelector{};
    for (;;) {
        sbc.RefreshWatchdog();
        gearSelector.MainFunction();
        gateway->SetGearAndMode(gearSelector.GetGear(), gearSelector.GetDriveMode());
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
[[noreturn]] static void cal_task(void *pvParameters)
{
    constexpr TickType_t xFrequency = MS2TICKS(1);
    auto gateway = reinterpret_cast<Gateway *>(pvParameters);
    auto can1 = gateway->GetCan(1);
    auto can2 = gateway->GetCan(2);
    auto canTp = CanTp{*can1};
    auto uds = Uds{&canTp};

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        can1->RxTask();
        can2->RxTask();
        uds.MainFunction();
        gateway->MainFunction();
        canTp.TxMainFunction();
        can1->TxTask();
        can2->TxTask();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void AppTasksInit()
{
    static std::shared_ptr<ICan> can1 = std::make_shared<FlexCan>(CAN0, 16);
    static std::shared_ptr<ICan> can2 = std::make_shared<FlexCan>(CAN1, 16);
    static std::shared_ptr<Gateway> gw = std::make_shared<Gateway>(can1, can2);
    if (xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE + 300, (void *) &gw, app_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }

    if (xTaskCreate(cal_task, "cal_task", configMINIMAL_STACK_SIZE + 512, (void *) &gw, cal_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}