#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <cstdint>
#include "util.hpp"
#include "cell.hpp"
#include "mesh.hpp"


//levels load/light meshes, meshes and VB belong to level????


class Level
{

public:
    constexpr static uint16_t LEVEL_WIDTH{7};
    constexpr static uint16_t LEVEL_MAX_LENGTH{512};


    consteval Level(uint16_t number, int16_t const oxygen, int16_t const gravity) :
        oxygen_(oxygen), gravity_(gravity)
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
        switch(number)
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



        auto data = parseCsvLevel(csvp);
        length_ = data.size() / LEVEL_WIDTH;

        Cell * dp = &data[0];
        for(auto l = 0ul; l < length_; ++l)
        {
            for(auto w = 0ul; w < LEVEL_WIDTH; ++w)
            {
                cells[w][l] = *dp;
                ++dp;
            }
        }



        //gen meshes and light colors here????
        //global pos buffer, per level color buffer?

        //keep list of seen meshes, need color buffers for each cell diff color
        std::map<uint16_t> seencolors;


    }


    [[nodiscard]] consteval auto getLength() const -> int16_t { return length_; }

    [[nodiscard]] consteval auto getOxygen() const -> int16_t { return oxygen_; }

private:
    int16_t length_;
    int16_t oxygen_;
    int16_t gravity_;

    //switch these for horizontal rendering in order, back to front?
    Cell cells[LEVEL_WIDTH][LEVEL_MAX_LENGTH];



    [[nodiscard]] consteval static auto parseCsvLevel(const char* csv) -> std::vector<Cell>
    {
        auto isSeparator = [](char c)
        {
            return c == ',' || c == '\r' || c == '\n' || c == '\t' || c == '|';
        };

        auto num = [](char const * p)
        {
            uint16_t c = static_cast<uint16_t>(*p);

            if(c == ' ')
            {
                c = '0';
            }
            if(c >= '0' && c <= 'o')
            {
                c = c - '0';
            }
            else
            {
                throw "ERROR: Invalid data in level file!";
            }

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


constexpr static Level LEVEL0{0,100,500};
constexpr static Level LEVEL1{1,200,1000};

#endif // LEVEL_HPP

