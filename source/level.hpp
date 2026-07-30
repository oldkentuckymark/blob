#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <set>
#include <flat_set>
#include <flat_map>
#include <cstdint>
#include <meta>
#include "util.hpp"
#include "cell.hpp"
#include "mesh.hpp"

struct LevelParseResult
{
    uint16_t length;
    std::array<std::flat_set<std::pair<uint16_t,uint16_t>>,16> seenColors;
    std::vector<Cell> cells;
};

template<util::ConstexprString SL>
class Level
{

public:
    constexpr static uint16_t LEVEL_WIDTH{7};
    constexpr static uint16_t LEVEL_MAX_LENGTH{512};


    consteval Level(int16_t const oxygen, int16_t const gravity) :
        oxygen_(oxygen), gravity_(gravity)
    {


        auto result = parseCsvLevel(SL);
        auto data = result.cells;

        //keep list of seen blocks, need color buffers for each cell diff color





        //generate mesh color buffers

        //fisrt positions, shared for all colors

        //now all color combos seen in level



    }


    [[nodiscard]] consteval auto getLength() const -> int16_t { return length_; }

    [[nodiscard]] consteval auto getOxygen() const -> int16_t { return oxygen_; }

private:
    constexpr static int16_t length_{parseCsvLevel(SL).length};
    int16_t oxygen_;
    int16_t gravity_;

    //switch these for horizontal rendering in order, back to front?
    Cell cells[LEVEL_WIDTH][length_];



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
        std::array<std::flat_set<std::pair<uint16_t,uint16_t>>,16> seen;

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

            cells.emplace_back(collision, type, topcolor, sidecolor);

            while(isSeparator(*p))
            {
                ++p;
            }

        }

        Cell * dp = cells.data();
        for(auto l = 0ul; l < cells.size()/LEVEL_WIDTH; ++l)
        {
            for(auto w = 0ul; w < LEVEL_WIDTH; ++w)
            {
                Cell const& c{*dp};
                seen[static_cast<size_t>(c.collision)].insert({c.topColor,c.sideColor});
                ++dp;
            }
        }

        return {cells.size()/LEVEL_WIDTH,seen,cells};
    }



};

constexpr static std::span<Vertex const> MESHES[] =
{
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TOPLOW})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TOPMID})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TOPHIGH})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::LEFTLOW,Mesh::Piece::RIGHTLOW,Mesh::Piece::FRONTLOW,Mesh::Piece::TOPLOW})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::LEFTMID,Mesh::Piece::RIGHTMID,Mesh::Piece::FRONTMID,Mesh::Piece::TOPMID})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::LEFTHIGH,Mesh::Piece::RIGHTHIGH,Mesh::Piece::FRONTHIGH,Mesh::Piece::TOPHIGH})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TUNNELLOW})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TUNNELMID})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TUNNELHIGH})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TOPLOW,Mesh::Piece::TUNNELLOW})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TOPMID,Mesh::Piece::TUNNELMID})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::TOPHIGH,Mesh::Piece::TUNNELHIGH})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::LEFTLOW,Mesh::Piece::RIGHTLOW,Mesh::Piece::FRONTLOW,Mesh::Piece::TOPLOW,Mesh::Piece::TUNNELLOW})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::LEFTMID,Mesh::Piece::RIGHTMID,Mesh::Piece::FRONTMID,Mesh::Piece::TOPMID,Mesh::Piece::TUNNELMID})),
    std::define_static_array(Mesh::makeMesh({Mesh::Piece::LEFTHIGH,Mesh::Piece::RIGHTHIGH,Mesh::Piece::FRONTHIGH,Mesh::Piece::TOPHIGH,Mesh::Piece::TUNNELHIGH})),
};


#endif // LEVEL_HPP

