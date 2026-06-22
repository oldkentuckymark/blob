#pragma once

#include <cstdint>
#include <array>
#include <vector>

namespace util
{

constexpr auto Convert888to555(uint8_t const r, uint8_t const g, uint8_t const b) -> uint32_t
{
    return (((r >> 3) & 31) |
            (((g >> 3) & 31) << 5) |
            (((b >> 3) & 31) << 10) );

}

constexpr auto Convert555to888(uint32_t color) -> std::array<uint8_t, 4>
{
    uint8_t const red = (color & 31) << 3;
    uint8_t const green = ((color >> 5) & 31) << 3;
    uint8_t const blue = ((color >> 10) & 31) << 3;
    uint8_t const alpha = 255;
    return {red,green,blue,alpha};
}


enum class ColorPalette : uint16_t
{
    Black = Convert888to555(0x00,0x00,0x00),

    //coorespond to power up Type
    Blue = Convert888to555(0x00,0x00,0xAA),
    Green = Convert888to555(0x00,0xAA,0xAA),
    Teal = Convert888to555(0x00,0xAA,0xAA),
    Red = Convert888to555(0xAA,0x00,0x00),
    Pink = Convert888to555(0xAA,0x00,0xAA),


    Brown = Convert888to555(0xAA,0x55,0x00),
    Grey = Convert888to555(0xAA,0xAA,0xAA),
    GreyDark = Convert888to555(0x55,0x55,0x55),
    BlueLight = Convert888to555(0x55,0x55,0xFF),
    GreenLight = Convert888to555(0x55,0xFF,0xFF),
    TealLight = Convert888to555(0x55,0xFF,0xFF),
    RedLight = Convert888to555(0xFF,0x55,0x55),
    PinkLight = Convert888to555(0xFF,0x55,0xFF),
    Yellow = Convert888to555(0xFF,0xFF,0x55),
    White = Convert888to555(0xFF,0xFF,0xFF)

};

template<class T, std::size_t N>
consteval auto make_array(std::vector<T> vec) -> std::array<T, N>
{
    std::array<T, N> arr{};
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = vec[i];
    }
    return arr;
}


}

