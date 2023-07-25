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

const UBaseType_t app_task_PRIORITY = (configMAX_PRIORITIES - 1);
const UBaseType_t cal_task_PRIORITY = (configMAX_PRIORITIES - 4);
typedef struct
{
    std::shared_ptr<ICan> can1;
    std::shared_ptr<ICan> can2;
}TaskParameters;
[[noreturn]] static void cal_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters);

[[noreturn]] static void app_task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = MS2TICKS(100);
    xLastWakeTime = xTaskGetTickCount();
    auto can = ((TaskParameters *) pvParameters)->can1;
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
        can->Send(0x445, payload, 8);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
[[noreturn]] static void cal_task(void *pvParameters)
{
    constexpr TickType_t xFrequency = MS2TICKS(1);

//    auto can1 = ((TaskParameters *) pvParameters)->can1;
//    auto can2 = ((TaskParameters *) pvParameters)->can2;
//    auto canTp = new CanTp{*can1};
//    auto uds = new Uds{canTp};
//    auto gw = new App::Gateway(can1,can2);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
//        can1->RxTask();
//        can2->RxTask();
//        uds->MainFunction();
//        gw->MainFunction();
//        canTp->TxMainFunction();
//        can1->TxTask();
//        can2->TxTask();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
void AppTasksInit()
{
//    static std::shared_ptr<ICan> can1 = std::make_shared<FlexCan>(CAN0, 16);
//    static std::shared_ptr<ICan> can2 = std::make_shared<FlexCan>(CAN1, 16);
//    TaskParameters params = {can1,can2};
//    if (xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE + 300, &params, app_task_PRIORITY, nullptr)
//        != pdPASS) {
//        while (true)
//            ;
//    }

    if (xTaskCreate(cal_task, "cal_task", configMINIMAL_STACK_SIZE + 512, nullptr/*&params*/, cal_task_PRIORITY, nullptr)
        != pdPASS) {
        while (true)
            ;
    }
}