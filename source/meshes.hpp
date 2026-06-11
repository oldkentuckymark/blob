#pragma once

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include "level.hpp"

enum class MESHES : unsigned int
{
    BLOB_BLUE = 0,
    BLOB_RED,
    BALL
};



consteval auto createMesh(MESHES const m) -> std::pair<std::vector<ffm::fixed32>,std::vector<uint16_t>> const
{
    constexpr char blobbluecsv[] =
    {
        #embed "../data/m.csv" suffix(, 0)
    };

    constexpr char blobredcsv[] =
    {
        #embed "../data/m.csv" suffix(, 0)
    };

    constexpr char ballcsv[] =
    {
        #embed "../data/m.csv" suffix(, 0)
    };

    std::vector<double> dv =  parse_csv(blobbluecsv);

    std::vector<ffm::fixed32> fvec;
    std::vector<uint16_t> cvec;


    for(std::size_t i = 0; i < dv.size(); i = i + 7)
    {
        fvec.push_back(ffm::fixed32{dv[i+0]});
        fvec.push_back(ffm::fixed32{dv[i+1]});
        fvec.push_back(ffm::fixed32{dv[i+2]});

        uint8_t const r = static_cast<uint8_t>(dv[i+3] * 255.0);
        uint8_t const g = static_cast<uint8_t>(dv[i+4] * 255.0);
        uint8_t const b = static_cast<uint8_t>(dv[i+5] * 255.0);
        cvec.push_back((((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10) ));

    }


    return {fvec,cvec};
}


consteval auto createCellVerts(Cell const & cell) -> std::vector<ffm::fixed32>
{
    ffm::vec3 flatpos[4] =
    {
        {0.0_fx,0.0_fx,0.0_fx},
        {0.0_fx,0.0_fx,0.0_fx},
        {0.0_fx,0.0_fx,0.0_fx},
        {0.0_fx,0.0_fx,0.0_fx}
    };

    return {};

}




constexpr auto BALL_VERTS_SIZE = createMesh(MESHES::BALL).first.size() / 3;
constexpr static auto BALL_VERTS = util::make_array<ffm::fixed32,BALL_VERTS_SIZE * 3>(createMesh(MESHES::BALL).first);

constexpr auto BALL_COLS_SIZE = createMesh(MESHES::BALL).second.size();
constexpr static auto BALL_COLS = util::make_array<uint16_t,BALL_COLS_SIZE>(createMesh(MESHES::BALL).second);



