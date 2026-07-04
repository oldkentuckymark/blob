#pragma once

#include <cstdint>
#include "ffr.hpp"
#include "util.hpp"
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
        TunnelBlock,
        Ramp
    };

    enum class Type : uint8_t
    {
        Normal,
        Oxygen,
        Boost,
        Sticky,
        Slippery,
        Kill,
        End
    };

    enum class Height : uint8_t
    {
        Low,
        Med,
        Hi
    };



    constexpr Cell() = default;

    constexpr Cell(Collision const c, Type const t, Height const h) :
        collision(c), type(t), height(h)
    {

    }

    ~Cell() = default;

    Collision collision{Collision::Empty};
    Type type{Type::Normal};
    Height height{Height::Low};


private:



};


class Level
{

public:
    Level()
    {

    }




    auto getLength() const -> int16_t { return length_; }

    auto getOxygen() const -> int16_t { return oxygen_; }

private:
    int16_t length_;
    int16_t oxygen_;
    int16_t gravity_;
    uint16_t front_color_, back_color_;
    uint16_t left_color_, right_color_;
    uint16_t top_color_, bottom_color;

    util::ColorPalette palette_;



    Cell const * cells{nullptr};




};


consteval static auto parseCsvTiles(const char* csv) -> std::vector<Cell>
{
    auto isSeparator = [](char c)
    {
        return c == ',' || c == '\r' || c == '\n' || c == ' ' || c == '\t';
    };

    std::vector<Cell> tiles;

    const char* p = csv;
    while (isSeparator(*p)) ++p;
    while (*p != '\0')
    {
        Cell::Collision collision = static_cast<Cell::Collision>(*p++ - '0');
        Cell::Type      type      = static_cast<Cell::Type>(*p++ - '0');
        Cell::Height    height    = static_cast<Cell::Height>(*p++ - '0');

        tiles.emplace_back(collision, type, height);

        while (isSeparator(*p))
        {
            ++p;
        }
    }

    return tiles;
}

consteval static auto createLevel(uint8_t const lvl) -> std::vector<Cell>
{
    constexpr char level0csv[] =
    {
        #embed "../data/level.txt" suffix(, 0)
    };
    constexpr char level1csv[] =
    {
        #embed "../data/level.txt" suffix(, 0)
    };


    char const* csvp{nullptr};
    switch(lvl)
    {
    case 0:
        csvp = level0csv;
        break;
    case 1:
        csvp = level1csv;
        break;
    }



    auto data = parseCsvTiles(csvp);

    return data;


}

constexpr static auto LEVEL0 = util::make_array<Cell,createLevel(0).size()>(createLevel(0));
constexpr static auto LEVEL1 = util::make_array<Cell,createLevel(0).size()>(createLevel(1));




