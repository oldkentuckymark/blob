#pragma once

#include <cstdint>
#include <array>
#include <vector>

namespace util
{

constexpr auto Convert888to555(uint8_t const r, uint8_t const g, uint8_t const b) -> uint16_t
{
    return (((r >> 3) & 31) |
            (((g >> 3) & 31) << 5) |
            (((b >> 3) & 31) << 10) );

}

constexpr auto Convert555to888(uint16_t color) -> std::array<uint8_t, 4>
{
    uint8_t const red = (color & 31) << 3;
    uint8_t const green = ((color >> 5) & 31) << 3;
    uint8_t const blue = ((color >> 10) & 31) << 3;
    uint8_t const alpha = 255;
    return {red,green,blue,alpha};
}


constexpr static auto CreateEGAPalette() -> std::array<uint16_t, 64>
{
    std::array<uint16_t, 64> palette{};

    for (std::size_t i = 0; i < 64; ++i)
    {
        uint8_t const red   = ((i & 0x20) ? 0xAA : 0x00) | ((i & 0x04) ? 0x55 : 0x00);
        uint8_t const green = ((i & 0x10) ? 0xAA : 0x00) | ((i & 0x02) ? 0x55 : 0x00);
        uint8_t const blue  = ((i & 0x08) ? 0xAA : 0x00) | ((i & 0x01) ? 0x55 : 0x00);

        palette[i] = Convert888to555(red, green, blue);
    }

    return palette;
}




template<class T, std::size_t N>
consteval auto make_array(std::vector<T> const & vec) -> std::array<T, N>
{
    std::array<T, N> arr{};
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = vec[i];
    }
    return arr;
}


}

