#pragma once

#include <cstdint>
#include "ffr.hpp"

class Level
{

static constexpr uint16_t k = 0b000'00'000'00000000;//3 bit for collision type(flat,tunnel,invtunnel), 2 bits for height, 3 bits for road type, 8 bits for color

consteval auto cell(uint8_t collision, uint8_t height, uint8_t type, uint8_t topcol, uint8_t bottomcol) -> uint8_t
{

}

public:
    enum class ColorPallate : uint16_t
    {
        Black = ffr::Convert888to555(0x00,0x00,0x00),
        Blue = ffr::Convert888to555(0x00,0x00,0xAA),
        Green = ffr::Convert888to555(0x00,0xAA,0xAA),
        Teal = ffr::Convert888to555(0x00,0xAA,0xAA),
        Red = ffr::Convert888to555(0xAA,0x00,0x00),
        Pink = ffr::Convert888to555(0xAA,0x00,0xAA),
        Brown = ffr::Convert888to555(0xAA,0x55,0x00),
        Grey = ffr::Convert888to555(0xAA,0xAA,0xAA),
        GreyDark = ffr::Convert888to555(0x55,0x55,0x55),
        BlueLight = ffr::Convert888to555(0x55,0x55,0xFF),
        GreenLight = ffr::Convert888to555(0x55,0xFF,0xFF),
        TealLight = ffr::Convert888to555(0x55,0xFF,0xFF),
        RedLight = ffr::Convert888to555(0xFF,0x55,0x55),
        PinkLight = ffr::Convert888to555(0xFF,0x55,0xFF),
        Yellow = ffr::Convert888to555(0xFF,0xFF,0x55),
        White = ffr::Convert888to555(0xFF,0xFF,0xFF)

    };

    enum class Type : uint8_t
    {

        Normal = 0,
        Supplies,
        Boost,
        Sticky,
        Slippery,
        Burning,
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


private:
};
