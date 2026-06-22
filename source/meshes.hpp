#pragma once

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include "level.hpp"

enum class MESH : uint16_t
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
    TUNNELMID,

    NUM_MESHES

};



consteval auto createMesh(MESH const m) -> std::pair<std::vector<ffm::fixed32>,std::vector<uint16_t>> const
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
    case MESH::SHIP:
        csvp = shipcsv;
        break;
    case MESH::FLAT:
        csvp = flatcsv;
        break;
    case MESH::BACKHIGH:
        csvp = backhighcsv;
        break;
    case MESH::BACKLOW:
        csvp = backlowcsv;
        break;
    case MESH::BACKMID:
        csvp = backmidcsv;
        break;
    case MESH::BOTTOMHIGH:
        csvp = bottomhighcsv;
        break;
    case MESH::BOTTOMLOW:
        csvp = bottomlowcsv;
        break;
    case MESH::BOTTOMMID:
        csvp = bottommidcsv;
        break;
    case MESH::FRONTHIGH:
        csvp = fronthighcsv;
        break;
    case MESH::FRONTLOW:
        csvp = frontlowcsv;
        break;
    case MESH::FRONTMID:
        csvp = frontmidcsv;
        break;
    case MESH::LEFTHIGH:
        csvp = lefthighcsv;
        break;
    case MESH::LEFTLOW:
        csvp = leftlowcsv;
        break;
    case MESH::LEFTMID:
        csvp = leftmidcsv;
        break;
    case MESH::RIGHTHIGH:
        csvp = righthighcsv;
        break;
    case MESH::RIGHTLOW:
        csvp = rightlowcsv;
        break;
    case MESH::RIGHTMID:
        csvp = rightmidcsv;
        break;
    case MESH::TOPHIGH:
        csvp = tophighcsv;
        break;
    case MESH::TOPLOW:
        csvp = toplowcsv;
        break;
    case MESH::TOPMID:
        csvp = topmidcsv;
        break;
    case MESH::TUNNELHIGH:
        csvp = tunnelhighcsv;
        break;
    case MESH::TUNNELLOW:
        csvp = tunnellowcsv;
        break;
    case MESH::TUNNELMID:
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

consteval auto getMeshSizes() -> std::array<std::size_t, static_cast<std::size_t>(MESH::NUM_MESHES)> const
{
    std::array<std::size_t, static_cast<std::size_t>(MESH::NUM_MESHES)> r;
    for(std::size_t i = 0; i < static_cast<std::size_t>(MESH::NUM_MESHES); ++i)
    {
        r[i] = createMesh(static_cast<MESH>(i)).first.size() / 3;
    }
    return r;
}

constexpr static auto MESH_SIZES = getMeshSizes();

constexpr static auto MESH_SHIP = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::SHIP)]*3>(createMesh(MESH::SHIP).first);
constexpr static auto MESH_FLAT = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::FLAT)]*3>(createMesh(MESH::FLAT).first);
constexpr static auto MESH_BACKHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::BACKHIGH)*3]>(createMesh(MESH::BACKHIGH).first);
constexpr static auto MESH_BACKLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::BACKLOW)]*3>(createMesh(MESH::BACKLOW).first);
constexpr static auto MESH_BACKMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::BACKMID)]*3>(createMesh(MESH::BACKMID).first);
constexpr static auto MESH_BOTTOMHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::BOTTOMHIGH)]*3>(createMesh(MESH::BOTTOMHIGH).first);
constexpr static auto MESH_BOTTOMLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::BOTTOMLOW)]*3>(createMesh(MESH::BOTTOMLOW).first);
constexpr static auto MESH_BOTTOMMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::BOTTOMMID)]*3>(createMesh(MESH::BOTTOMMID).first);
constexpr static auto MESH_FRONTHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::FRONTHIGH)]*3>(createMesh(MESH::FRONTHIGH).first);
constexpr static auto MESH_FRONTLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::FRONTLOW)]*3>(createMesh(MESH::FRONTLOW).first);
constexpr static auto MESH_FRONTMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::FRONTMID)]*3>(createMesh(MESH::FRONTMID).first);
constexpr static auto MESH_LEFTLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::LEFTLOW)]*3>(createMesh(MESH::LEFTLOW).first);
constexpr static auto MESH_LEFTHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::LEFTHIGH)]*3>(createMesh(MESH::LEFTHIGH).first);
constexpr static auto MESH_LEFTMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::LEFTMID)]*3>(createMesh(MESH::LEFTMID).first);
constexpr static auto MESH_RIGHTHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::RIGHTHIGH)]*3>(createMesh(MESH::RIGHTHIGH).first);
constexpr static auto MESH_RIGHTLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::RIGHTLOW)]*3>(createMesh(MESH::RIGHTLOW).first);
constexpr static auto MESH_RIGHTMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::RIGHTMID)]*3>(createMesh(MESH::RIGHTMID).first);
constexpr static auto MESH_TOPHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::TOPHIGH)]*3>(createMesh(MESH::TOPHIGH).first);
constexpr static auto MESH_TOPLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::TOPLOW)]*3>(createMesh(MESH::TOPLOW).first);
constexpr static auto MESH_TOPMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::TOPMID)]*3>(createMesh(MESH::TOPMID).first);
constexpr static auto MESH_TUNNELHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::TUNNELHIGH)]*3>(createMesh(MESH::TUNNELHIGH).first);
constexpr static auto MESH_TUNNELLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::TUNNELLOW)]*3>(createMesh(MESH::TUNNELLOW).first);
constexpr static auto MESH_TUNNELMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH::TUNNELMID)]*3>(createMesh(MESH::TUNNELMID).first);







constexpr static auto BALL_VERTS_SIZE = createMesh(MESH::SHIP).first.size() / 3;
constexpr static auto BALL_VERTS = util::make_array<ffm::fixed32,BALL_VERTS_SIZE * 3>(createMesh(MESH::SHIP).first);

constexpr static auto BALL_COLS_SIZE = createMesh(MESH::SHIP).second.size();
constexpr static auto BALL_COLS = util::make_array<uint16_t,BALL_COLS_SIZE>(createMesh(MESH::SHIP).second);



