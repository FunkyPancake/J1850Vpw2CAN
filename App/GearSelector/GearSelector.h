//
// Created by PC on 12.07.2023.
//

#ifndef KEWJGWTCU_GEARSELECTOR_H
#define KEWJGWTCU_GEARSELECTOR_H
namespace App::GearSelector
{
    class GearSelectorPosition
    {
    public:
        enum class SelectorPosition
        {
            P = 0b110,
            R = 0b111,
            N = 0b011,
            D = 0b010,
            D4 = 0b001,
            D3 = 0b100,
            D2 = 0b000,
            D1 = 0b101
        };
        SelectorPosition GetCurrentPosition();
    };
}// namespace App::GearSelector

#endif//KEWJGWTCU_GEARSELECTOR_H
