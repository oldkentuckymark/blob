#pragma once

#include <cstdint>
#include "ffr.hpp"
#include "util.hpp"


class Cell
{
public:

    enum class Collision : uint8_t
    {
        Empty,
        Plane,
        Block,
        Tunnel,
        TunnelPlane,
        TunnelBlock
    };

    enum class Type : uint8_t
    {
        Normal,
        Oxygen,
        Boost,
        Sticky,
        Slippery,
        Kill,
    };

    enum class Height : uint8_t
    {
        Low,
        Med,
        Hi
    };



    Cell() = default;

    Cell(Type const t, Height const h, Collision const c)
    {

    }

    ~Cell() = default;


    constexpr auto packTileAttributes(Type type, Collision collision, Height height) -> uint8_t
    {
        return static_cast<uint8_t>(
            (static_cast<uint8_t>(type)       << 5) |
            (static_cast<uint8_t>(collision)  << 2) |
            static_cast<uint8_t>(height)
            );
    }

    constexpr auto unpackTileAttributes(uint8_t packed, Type& outType, Collision& outCollision, Height& outHeight) -> void
    {
        outType      = static_cast<Type>((packed >> 5) & 0x7);   // 3 bits
        outCollision = static_cast<Collision>((packed >> 2) & 0x7); // 3 bits
        outHeight    = static_cast<Height>(packed & 0x3);         // 2 bits
    }


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
    int16_t length_;
    int16_t oxygen_;
    int16_t gravity_;
    uint16_t front_color_, back_color_;
    uint16_t left_color_, right_color_;
    uint16_t top_color_, bottom_color;



    Cell cells[7][512];
};
