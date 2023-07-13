//
// Created by PC on 12.07.2023.
//

#include "GearSelector.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
App::GearSelector::GearSelectorPosition::SelectorPosition App::GearSelector::GearSelectorPosition::GetCurrentPosition()
{
    auto c2 = GPIO_PinRead(BOARD_INITPINS_C2_GPIO, BOARD_INITPINS_C2_PIN);
    auto c3 = GPIO_PinRead(BOARD_INITPINS_C3_GPIO, BOARD_INITPINS_C3_PIN);
    auto c4 = GPIO_PinRead(BOARD_INITPINS_C4_GPIO, BOARD_INITPINS_C4_PIN);
    auto selectorPosition = static_cast<SelectorPosition>(c4 + (c3 << 1) + (c2 << 2));
    switch (selectorPosition) {
        case SelectorPosition::P:
            GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, 0);
            GPIO_PinWrite(BOARD_INITPINS_D2_GPIO, BOARD_INITPINS_D2_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D3_GPIO, BOARD_INITPINS_D3_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D4_GPIO, BOARD_INITPINS_D4_PIN, 1);
            break;
        case SelectorPosition::R:
            GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D2_GPIO, BOARD_INITPINS_D2_PIN, 0);
            GPIO_PinWrite(BOARD_INITPINS_D3_GPIO, BOARD_INITPINS_D3_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D4_GPIO, BOARD_INITPINS_D4_PIN, 1);
            break;
        case SelectorPosition::N:
            GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D2_GPIO, BOARD_INITPINS_D2_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D3_GPIO, BOARD_INITPINS_D3_PIN, 0);
            GPIO_PinWrite(BOARD_INITPINS_D4_GPIO, BOARD_INITPINS_D4_PIN, 1);
            break;
        case SelectorPosition::D:
            GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D2_GPIO, BOARD_INITPINS_D2_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D3_GPIO, BOARD_INITPINS_D3_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D4_GPIO, BOARD_INITPINS_D4_PIN, 0);
            break;
        default:
            GPIO_PinWrite(BOARD_INITPINS_D1_GPIO, BOARD_INITPINS_D1_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D2_GPIO, BOARD_INITPINS_D2_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D3_GPIO, BOARD_INITPINS_D3_PIN, 1);
            GPIO_PinWrite(BOARD_INITPINS_D4_GPIO, BOARD_INITPINS_D4_PIN, 1);
            break;
    }
    return selectorPosition;
}
