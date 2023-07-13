#include "../KeCommon/Bsw/Diag/CanTp.h"
#include "../KeCommon/Bsw/Diag/Uds.h"
#include "AppTasks.h"
#include "FlexCan.h"
#include "FreeRTOS.h"
#include "GearSelector/GearSelector.h"
#include "LpSpi.h"
#include "LpUart.h"
#include "Tle9461.h"
#include "peripherals.h"

#define MS2TICKS(x) (configTICK_RATE_HZ / 1000 * (x))

using namespace KeCommon::Bsw::Can;
using namespace KeCommon::Bsw::Diag;

const UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);
const UBaseType_t cal_task_PRIORITY = (configMAX_PRIORITIES - 4);


[[noreturn]] static void cal_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters)
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = MS2TICKS(100);
    xLastWakeTime = xTaskGetTickCount();
    auto can = (FlexCan *) pvParameters;
    LpSpiRtos sbcSpi{&LPSPI0_handle};
    Tle9461 sbc{&sbcSpi};
    sbc.Init();
    Payload payload{};
    sbc.ConfigWatchdog(Tle9461::WgTimer200ms);
    App::GearSelector::GearSelectorPosition gearSelector{};
    for (;;) {
        sbc.RefreshWatchdog();
        auto selectedPosition = gearSelector.GetCurrentPosition();
        payload.b[0] = static_cast<uint8_t>(selectedPosition);
        can->Send(0x123445, payload, 8);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
[[noreturn]] static void cal_task(void *pvParameters)
{
    constexpr uint32_t TxId = 0x80007ff5;
    constexpr uint32_t RxId = 0x80007ff6;
    constexpr TickType_t xFrequency = MS2TICKS(1);

    auto can = (FlexCan *) pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    auto canTp = new CanTp{reinterpret_cast<ICan &>(*can), RxId, TxId};
    auto uds = new Uds{canTp};
    for (;;) {
        can->RxTask();
        uds->MainFunction();
        canTp->TxMainFunction();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void AppTasksInit()
{
    static auto can = FlexCan{CAN0, 16};
    if (xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE + 256, &can, app_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }

    if (xTaskCreate(cal_task, "cal_task", configMINIMAL_STACK_SIZE + 128, &can, cal_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}