#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <flat_set>
#include <flat_map>
#include <cstdint>
#include <meta>
#include "color.hpp"
#include "util.hpp"
#include "cell.hpp"
#include "mesh.hpp"

struct LevelParseResult
{
    uint16_t length;
    std::array< std::flat_set<std::pair<Color,Color>>, static_cast<size_t>(Cell::Collision::NUM_COLLISIONS) > seenColors;
    std::vector<Cell> cells;
};


template<util::ConstexprString SL>
class Level
{

public:
    constexpr static uint16_t LEVEL_WIDTH{7};
    constexpr static uint16_t LEVEL_MAX_LENGTH{512};


    consteval Level(int16_t const oxygen, int16_t const gravity, ffm::vec3 const sun = {0.0_fx,1.0_fx,0.0_fx}) :
        oxygen_(oxygen), gravity_(gravity), sun_(sun)
    {

        auto getIndex = [](std::flat_set<std::pair<Color,Color>> set, std::pair<Color,Color> cp) -> size_t
        {
            auto it = set.begin();
            for(auto i = 0; i < set.size(); ++i)
            {
                if(*it == cp)
                {
                    return i;
                }
                std::advance(it, 1);
            }
            throw(-1);
        };


        auto result = parseCsvLevel(SL);


        //make color buffers







        //assign each colorbufferindexfor each cell
        for(auto& cell : cells)
        {



        }







    }

    [[nodiscard]] consteval auto getCell(uint16_t w, uint16_t l) const -> Cell { return {}; }


    [[nodiscard]] consteval auto getLength() const -> int16_t { return length_; }

    [[nodiscard]] consteval auto getOxygen() const -> int16_t { return oxygen_; }

    [[nodiscard]] consteval auto getSun() -> ffm::vec3 {return sun_;}

private:
    [[nodiscard]] consteval static auto parseCsvLevel(auto sl) -> LevelParseResult
    {
        constexpr char level0csv[] =
        {
            #embed "../data/levels/level0.txt" suffix(, 0)
        };
        constexpr char level1csv[] =
        {
            #embed "../data/levels/level1.txt" suffix(, 0)
        };

        char const* csvp{level0csv};
        if(sl == "../data/levels/level0.txt")
        {
            csvp = level0csv;
        }
        if(sl == "../data/levels/level1.txt")
        {
            csvp = level1csv;
        }


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

        std::vector<Cell> cells;
        std::array<std::flat_set<std::pair<Color,Color>>, static_cast<size_t>(Cell::Collision::NUM_COLLISIONS)> seenColors;


        char const* p = csvp;
        while(*p != '\0')
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


            seenColors[static_cast<size_t>(collision)].insert({topcolor,sidecolor});

            auto& cell = cells.emplace_back(collision, type, topcolor, sidecolor);

            while(isSeparator(*p))
            {
                ++p;
            }

        }


        return {static_cast<uint16_t>(cells.size()/LEVEL_WIDTH),seenColors,cells};
    }


    constexpr static int16_t length_{static_cast<int16_t>(parseCsvLevel(SL).length)};
    constexpr static std::array< Cell, LEVEL_WIDTH * length_> cells{util::make_array<Cell,LEVEL_WIDTH*length_>(parseCsvLevel(SL).cells)};

    int16_t oxygen_;
    int16_t gravity_;
    ffm::vec3 sun_;



    std::array<uint16_t,LEVEL_WIDTH * length_> cellColorBufferIdxs{};
    std::array<std::array<Color,Mesh::CELL_MESHES[0].size()>,parseCsvLevel(SL).seenColors[0].size()> EmptyBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[1].size()>,parseCsvLevel(SL).seenColors[1].size()> PlaneLowBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[2].size()>,parseCsvLevel(SL).seenColors[2].size()> PlaneMidBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[3].size()>,parseCsvLevel(SL).seenColors[3].size()> PlaneHighBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[4].size()>,parseCsvLevel(SL).seenColors[4].size()> BlockLowBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[5].size()>,parseCsvLevel(SL).seenColors[5].size()> BlockMidBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[6].size()>,parseCsvLevel(SL).seenColors[6].size()> BlockHighBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[0].size()>,parseCsvLevel(SL).seenColors[7].size()> TunnelLowBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[0].size()>,parseCsvLevel(SL).seenColors[8].size()> TunnelMidBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[0].size()>,parseCsvLevel(SL).seenColors[9].size()> TunnelHighBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[10].size()>,parseCsvLevel(SL).seenColors[10].size()> TunnelPlaneLowBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[11].size()>,parseCsvLevel(SL).seenColors[11].size()> TunnelPlaneMidBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[12].size()>,parseCsvLevel(SL).seenColors[12].size()> TunnelPlaneHighBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[13].size()>,parseCsvLevel(SL).seenColors[13].size()> TunnelBlockLowBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[14].size()>,parseCsvLevel(SL).seenColors[14].size()> TunnelBlockMidBuffers{};
    std::array<std::array<Color,Mesh::CELL_MESHES[15].size()>,parseCsvLevel(SL).seenColors[15].size()> TunnelBlockHighBuffers{};


};




constexpr static Level<"../data/levels/level0.txt"> level0(100,500);
constexpr static Level<"../data/levels/level1.txt"> level1(100,500);



#endif // LEVEL_HPP

