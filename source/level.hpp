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
        TunelFloorCeil
    };

    Type type;
    Height height;
    Collision collision;


private:

};


class Level
{



public:



private:
    std::array<Cell, 5> cells;
};
