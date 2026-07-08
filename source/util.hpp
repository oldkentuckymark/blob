#pragma once

#include <cstdint>
#include <array>
#include <vector>

namespace util
{

constexpr auto Convert555to888(uint16_t color) -> std::array<uint8_t, 3>
{
    uint8_t const red = (color & 31) << 3;
    uint8_t const green = ((color >> 5) & 31) << 3;
    uint8_t const blue = ((color >> 10) & 31) << 3;
    return {red,green,blue};
}

constexpr auto Convert888to555(uint8_t const r, uint8_t const g, uint8_t const b) -> uint16_t
{
    return (((r >> 3) & 31) |
            (((g >> 3) & 31) << 5) |
            (((b >> 3) & 31) << 10) );

}

constexpr auto Convert888to555(std::array<uint8_t, 3> const & rgb) -> uint16_t
{
    return (((rgb[0] >> 3) & 31) |
            (((rgb[1] >> 3) & 31) << 5) |
            (((rgb[2] >> 3) & 31) << 10) );

}

constexpr auto brightenColor(uint16_t color, int8_t shades) -> uint16_t
{
    // Extract 5-bit channels (RGB555 layout: bit 15 unused, 14-10 R, 9-5 G, 4-0 B)
    uint8_t r = (color >> 10) & 0x1F;
    uint8_t g = (color >> 5)  & 0x1F;
    uint8_t b = color & 0x1F;

    // Apply shade adjustment
    int16_t nr = r + (shades);
    int16_t ng = g + (shades);
    int16_t nb = b + (shades);

    // Clamp each channel to the valid 5-bit range [0, 31]
    nr = (nr < 0) ? 0 : (nr > 31) ? 31 : nr;
    ng = (ng < 0) ? 0 : (ng > 31) ? 31 : ng;
    nb = (nb < 0) ? 0 : (nb > 31) ? 31 : nb;

    // Reconstruct the RGB555 value (MSB remains 0/unused)
    return static_cast<uint16_t>((nr << 10) | (ng << 5) | nb);
}

constexpr auto CreateEGAPalette() -> std::array<uint16_t, 64>
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

