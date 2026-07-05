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
        PlaneLow,
        PlaneMid,
        PlaneHigh,
        BlockLow,
        BlockMid,
        BlockHigh,
        TunnelLow,
        TunnelMid,
        TunnelHigh,
        TunnelPlaneLow,
        TunnelPlaneMid,
        TunnelPlaneHigh,
        TunnelBlockLow,
        TunnelBlockMid,
        TunnelBlockHigh
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



    constexpr Cell() = default;

    constexpr Cell(Collision const c, Type const t, uint16_t const tc, uint16_t const sc) :
        collision(c), type(t), topColor(tc), sideColor(sc)
    {

    }

    ~Cell() = default;

    Collision collision{Collision::Empty};
    Type type{Type::Normal};
    uint16_t topColor{0};
    uint16_t sideColor{0};


private:



};


class Level
{

public:

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


    auto getLength() const -> int16_t { return length_; }

    auto getOxygen() const -> int16_t { return oxygen_; }

private:
    int16_t length_;
    int16_t oxygen_;
    int16_t gravity_;


    Cell const * cells_{nullptr};


    consteval static auto parseCsvTiles(const char* csv) -> std::vector<Cell>
    {
        auto isSeparator = [](char c)
        {
            return c == ',' || c == '\r' || c == '\n' || c == ' ' || c == '\t';
        };

        constexpr auto palette_{util::CreateEGAPalette()};

        std::vector<Cell> tiles;

        const char* p = csv;
        while (isSeparator(*p)) ++p;
        while (*p != '\0')
        {
            Cell::Collision collision = static_cast<Cell::Collision>(*p++ - '0');
            Cell::Type type = static_cast<Cell::Type>(*p++ - '0');

            uint16_t topcolor = *p++ - '0';
            uint16_t sidecolor = *p++ - '0';

            topcolor = palette_[topcolor];
            sidecolor = palette_[sidecolor];

            tiles.emplace_back(collision, type, topcolor, sidecolor);

            while (isSeparator(*p))
            {
                ++p;
            }
        }

        return tiles;
    }




};






constexpr static auto LEVEL0 = util::make_array< Cell, Level::createLevel(0).size() >(Level::createLevel(0));
constexpr static auto LEVEL1 = util::make_array< Cell, Level::createLevel(1).size() >(Level::createLevel(1));




