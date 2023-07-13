#include "App.h"
#include "../KeCommon/Bsw/Diag/CanTp.h"
#include "../KeCommon/Bsw/Diag/Uds.h"
#include "FlexCan.h"
#include "FreeRTOS.h"
#include "LpSpi.h"
#include "LpUart.h"
#include "Tle9461.h"
#include "peripherals.h"

using namespace KeCommon::Bsw::Can;
using namespace KeCommon::Bsw::Diag;
#define MS2TICKS(x) (configTICK_RATE_HZ / 1000 * x)
const UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);
const UBaseType_t cal_task_PRIORITY = (configMAX_PRIORITIES - 4);
[[noreturn]] static void cal_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters)
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = MS2TICKS(10);
    xLastWakeTime = xTaskGetTickCount();
    auto can = (FlexCan *) pvParameters;
    LpSpiRtos sbcSpi{&LPSPI0_handle};
    Tle9461 sbc{&sbcSpi};
    sbc.Init();
    Payload payload{};
    sbc.ConfigWatchdog(Tle9461::WgTimer200ms);
    for (;;) {
        sbc.RefreshWatchdog();

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

    CanTp canTp{reinterpret_cast<ICan &>(*can), TxId, RxId};
    Uds uds{&canTp};
    for (;;) {
        can->RxTask();
        uds.MainFunction();
        canTp.TxMainFunction();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void App()
{
    static auto can = FlexCan{CAN0, 16};
    if (xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE + 256, &can, app_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }

    if (xTaskCreate(cal_task, "cal_task", configMINIMAL_STACK_SIZE + 100, &can, cal_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}