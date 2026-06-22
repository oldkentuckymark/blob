#pragma once

#include <cstdint>
#include "ffr.hpp"
#include "util.hpp


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
        Block = 0,
        TunnelNoFloor,
        TunnelFloor,

    };

    Cell(Type const t, Height const h, Collision const c)
    {

    }

    ~Cell() = default;

    Type type;
    Height height;
    Collision collision;


private:

};


class Level
{

public:
    Level()
    {

    }


private:
    int16_t oxygen_;
    int16_t gravity_;
    uint16_t front_color_, back_color_;
    uint16_t left_color_, right_color_;
    uint16_t top_color_, bottom_color;



    //Cell cells[7][1024];
};
