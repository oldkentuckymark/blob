#pragma once

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include "level.hpp"

enum class MESHES : uint16_t
{
    SHIP = 0,
    FLAT,
    BACKHIGH,
    BACKLOW,
    BACKMID,
    BOTTOMHIGH,
    BOTTOMLOW,
    BOTTOMMID,
    FRONTHIGH,
    FRONTLOW,
    FRONTMID,
    LEFTHIGH,
    LEFTLOW,
    LEFTMID,
    RIGHTHIGH,
    RIGHTLOW,
    RIGHTMID,
    TOPHIGH,
    TOPLOW,
    TOPMID,
    TUNNELHIGH,
    TUNNELLOW,
    TUNNELMID

};



consteval auto createMesh(MESHES const m) -> std::pair<std::vector<ffm::fixed32>,std::vector<uint16_t>> const
{
    constexpr char shipcsv[] =
    {
        #embed "../data/ship.csv" suffix(, 0)
    };
    constexpr char shipcsv[] =
    {
        #embed "../data/ship.csv" suffix(, 0)
    };
    constexpr char shipcsv[] =
    {
        #embed "../data/ship.csv" suffix(, 0)
    };
    constexpr char shipcsv[] =
    {
        #embed "../data/ship.csv" suffix(, 0)
    };
    constexpr char shipcsv[] =
    {
        #embed "../data/ship.csv" suffix(, 0)
    };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
        };
    constexpr char shipcsv[] =
        {
#embed "../data/ship.csv" suffix(, 0)
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






constexpr static auto BALL_VERTS_SIZE = createMesh(MESHES::BALL).first.size() / 3;
constexpr static auto BALL_VERTS = util::make_array<ffm::fixed32,BALL_VERTS_SIZE * 3>(createMesh(MESHES::BALL).first);

constexpr static auto BALL_COLS_SIZE = createMesh(MESHES::BALL).second.size();
constexpr static auto BALL_COLS = util::make_array<uint16_t,BALL_COLS_SIZE>(createMesh(MESHES::BALL).second);



