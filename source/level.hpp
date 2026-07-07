#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <cstdint>
#include "util.hpp"
#include "util.hpp"
#include "cell.hpp"



class Level
{

public:

    Level(Cell const * cells, int16_t const oxygen, int16_t const gravity) :
        cells_(cells), oxygen_(oxygen), gravity_(gravity)
    {
        length_ = sizeof(cells) / 7;
    }

    consteval static auto createLevel(uint8_t const lvl) -> std::vector<Cell>
    {
        constexpr char level0csv[] =
        {
            #embed "../data/levels/level.txt" suffix(, 0)
        };
        constexpr char level1csv[] =
        {
            #embed "../data/levels/level.txt" suffix(, 0)
        };


        char const* csvp{level0csv};
        switch(lvl)
        {
        case 0:
            csvp = level0csv;
            break;
        case 1:
            csvp = level1csv;
            break;
        default:
            csvp = level0csv;
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
            return c == ',' || c == '\r' || c == '\n' || c == '\t' || c == '|';
        };

        auto num = [](char const * p)
        {
            uint16_t c = static_cast<uint16_t>(*p);

            if(c == ' ') { c = '0'; }
            if(c >= '0' && c <= 'o') {c = c - '0';}

            return c;
        };

        constexpr auto palette_{util::CreateEGAPalette()};

        std::vector<Cell> tiles;

        char const* p = csv;

        while (*p != '\0')
        {

            Cell::Collision collision = static_cast<Cell::Collision>(num(p));
            ++p;

           Cell::Type type = static_cast<Cell::Type>(num(p));
            ++p;

            uint16_t topcolor = num(p);
            ++p;

            uint16_t sidecolor = num(p);
            ++p;

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


#endif

