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


    // Bit layout (MSB -> LSB):
    // [ Collision:3 | Type:3 | Height:2 ]

    constexpr static uint8_t packTileAttributes(Collision collision, Type type, Height height)
    {
        return static_cast<uint8_t>(
            (static_cast<uint8_t>(collision) << 5) |
            (static_cast<uint8_t>(type)      << 2) |
            static_cast<uint8_t>(height)
            );
    }

    constexpr static void unpackTileAttributes(uint8_t packed, Collision& outCollision, Type& outType, Height& outHeight)
    {
        outCollision = static_cast<Collision>((packed >> 5) & 0x7); // 3 bits
        outType      = static_cast<Type>((packed >> 2) & 0x7);      // 3 bits
        outHeight    = static_cast<Height>(packed & 0x3);            // 2 bits
    }


    Collision collision;
    Type type;
    Height height;


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


    consteval std::vector<uint8_t> ParseCsvTiles(const char* csv)
    {
        auto isSeparator = [](char c)
        {
            return c == ',' || c == '\r' || c == '\n' || c == ' ' || c == '\t';
        };

        std::vector<uint8_t> packedTiles;

        const char* p = csv;
        while (isSeparator(*p)) ++p;
        while (*p != '\0')
        {
            Cell::Collision collision = static_cast<Cell::Collision>(*p++ - '0');
            Cell::Type      type      = static_cast<Cell::Type>(*p++ - '0');
            Cell::Height    height    = static_cast<Cell::Height>(*p++ - '0');

            packedTiles.push_back(Cell::packTileAttributes(collision, type, height));

            while (isSeparator(*p))
                ++p;
        }

        return packedTiles;
    }



};
