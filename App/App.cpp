#include "App.h"
#include "CanTp.h"
#include "FlexCan.h"
#include "FreeRTOS.h"
#include "LpSpi.h"
#include "LpUart.h"
#include "Tle9461.h"
#include "peripherals.h"
#include <memory>

using namespace KeCommon::Bsw::Can;
using namespace KeCommon::Bsw::Cal;

const UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);
const UBaseType_t cal_task_PRIORITY = (configMAX_PRIORITIES - 2);
[[noreturn]] static void cal_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters)
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 40;
    xLastWakeTime = xTaskGetTickCount();
    auto can = (FlexCan *) pvParameters;
    LpSpiRtos sbcSpi{&LPSPI0_handle};
    Tle9461 sbc{&sbcSpi};
    sbc.Init();
    sbc.ConfigWatchdog(Tle9461::WgTimer200ms);
    for (;;) {
        sbc.RefreshWatchdog();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
[[noreturn]] static void cal_task(void *pvParameters)
{
    constexpr uint32_t TxId = 0x7fffff15;
    constexpr uint32_t RxId = 0x7fffff16;
    constexpr TickType_t xFrequency = 10;

    auto can = (FlexCan *) pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    CanTp canTp{reinterpret_cast<ICan &>(*can), TxId, RxId};
    for (;;) {
        canTp.TxTask();
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

    if (xTaskCreate(cal_task, "cal_task", configMINIMAL_STACK_SIZE, &can, cal_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}