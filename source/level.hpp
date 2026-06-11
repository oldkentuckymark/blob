#pragma once

#include <cstdint>
#include "ffr.hpp"


class Cell
{
public:

    enum class Type : uint8_t
    {

        Normal = 0,
        Oxygen,
        Boost,
        Sticky,
        Slippery,
        Kill,
    };

    enum class Height : uint8_t
    {
        Low = 0,
        Med,
        Hi
    };

    enum class Collision : uint8_t
    {
        Flat = 0,
        TunnelNoFloor,
        TunnelFloor,
        TunelFloorCeil,
        Ramp
    };

    uint16_t bottom_color, side_color, topColor;//Type overrides colors
    Type type;
    Height height;
    Collision collision;


private:

};


class Level
{


static constexpr uint16_t k = 0b000'00'000'00000000;//3 bit for collision type(flat,tunnel,invtunnel), 2 bits for height, 3 bits for road type, 8 bits for color

consteval auto cell(uint8_t collision, uint8_t height, uint8_t type, uint8_t topcol, uint8_t bottomcol) -> uint8_t
{
    return 0;
}

public:



private:
};
