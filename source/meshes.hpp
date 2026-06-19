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
    constexpr char flatcsv[] =
    {
        #embed "../data/flat.csv" suffix(, 0)
    };
    constexpr char backhighcsv[] =
    {
        #embed "../data/backhigh.csv" suffix(, 0)
    };
    constexpr char backlowcsv[] =
    {
        #embed "../data/backlow.csv" suffix(, 0)
    };
    constexpr char backmidcsv[] =
    {
        #embed "../data/backmid.csv" suffix(, 0)
    };
    constexpr char bottomhighcsv[] =
    {
        #embed "../data/bottomhigh.csv" suffix(, 0)
    };
    constexpr char bottomlowcsv[] =
    {
        #embed "../data/bottomlow.csv" suffix(, 0)
    };
    constexpr char bottommidcsv[] =
    {
        #embed "../data/bottommid.csv" suffix(, 0)
    };
    constexpr char fronthighcsv[] =
    {
        #embed "../data/fronthigh.csv" suffix(, 0)
    };
    constexpr char frontlowcsv[] =
    {
        #embed "../data/frontlow.csv" suffix(, 0)
    };
    constexpr char frontmidcsv[] =
    {
        #embed "../data/frontmid.csv" suffix(, 0)
    };
    constexpr char lefthighcsv[] =
    {
        #embed "../data/lefthigh.csv" suffix(, 0)
    };
    constexpr char leftlowcsv[] =
    {
        #embed "../data/leftlow.csv" suffix(, 0)
    };
    constexpr char leftmidcsv[] =
    {
        #embed "../data/leftmid.csv" suffix(, 0)
    };
    constexpr char righthighcsv[] =
    {
        #embed "../data/righthigh.csv" suffix(, 0)
    };
    constexpr char rightlowcsv[] =
    {
        #embed "../data/rightlow.csv" suffix(, 0)
    };
    constexpr char rightmidcsv[] =
    {
        #embed "../data/rightmid.csv" suffix(, 0)
    };
    constexpr char tophighcsv[] =
    {
        #embed "../data/tophigh.csv" suffix(, 0)
    };
    constexpr char toplowcsv[] =
    {
        #embed "../data/toplow.csv" suffix(, 0)
    };
    constexpr char topmidcsv[] =
    {
        #embed "../data/topmid.csv" suffix(, 0)
    };
    constexpr char tunnelhighcsv[] =
    {
        #embed "../data/tunnelhigh.csv" suffix(, 0)
    };
    constexpr char tunnellowcsv[] =
    {
        #embed "../data/tunnellow.csv" suffix(, 0)
    };
    constexpr char tunnelmidcsv[] =
    {
        #embed "../data/tunnelmid.csv" suffix(, 0)
    };


    char const* csvp{nullptr};
    switch(m)
    {
    case MESHES::SHIP:
        csvp = shipcsv;
        break;
    case MESHES::FLAT:
        csvp = flatcsv;
        break;
    case MESHES::BACKHIGH:
        csvp = backhighcsv;
        break;
    case MESHES::BACKLOW:
        csvp = backlowcsv;
        break;
    case MESHES::BACKMID:
        csvp = backmidcsv;
        break;
    case MESHES::BOTTOMHIGH:
        csvp = bottomhighcsv;
        break;
    case MESHES::BOTTOMLOW:
        csvp = bottomlowcsv;
        break;
    case MESHES::BOTTOMMID:
        csvp = bottommidcsv;
        break;
    case MESHES::FRONTHIGH:
        csvp = fronthighcsv;
        break;
    case MESHES::FRONTLOW:
        csvp = frontlowcsv;
        break;
    case MESHES::FRONTMID:
        csvp = frontmidcsv;
        break;
    case MESHES::LEFTHIGH:
        csvp = lefthighcsv;
        break;
    case MESHES::LEFTLOW:
        csvp = leftlowcsv;
        break;
    case MESHES::LEFTMID:
        csvp = leftmidcsv;
        break;
    case MESHES::RIGHTHIGH:
        csvp = righthighcsv;
        break;
    case MESHES::RIGHTLOW:
        csvp = rightlowcsv;
        break;
    case MESHES::RIGHTMID:
        csvp = rightmidcsv;
        break;
    case MESHES::TOPHIGH:
        csvp = tophighcsv;
        break;
    case MESHES::TOPLOW:
        csvp = toplowcsv;
        break;
    case MESHES::TOPMID:
        csvp = topmidcsv;
        break;
    case MESHES::TUNNELHIGH:
        csvp = tunnelhighcsv;
        break;
    case MESHES::TUNNELLOW:
        csvp = tunnellowcsv;
        break;
    case MESHES::TUNNELMID:
        csvp = tunnelmidcsv;
        break;
    }








    std::vector<double> dv =  parse_csv(csvp);

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






constexpr static auto BALL_VERTS_SIZE = createMesh(MESHES::TUNNELLOW).first.size() / 3;
constexpr static auto BALL_VERTS = util::make_array<ffm::fixed32,BALL_VERTS_SIZE * 3>(createMesh(MESHES::TUNNELLOW).first);

constexpr static auto BALL_COLS_SIZE = createMesh(MESHES::TUNNELLOW).second.size();
constexpr static auto BALL_COLS = util::make_array<uint16_t,BALL_COLS_SIZE>(createMesh(MESHES::TUNNELLOW).second);



