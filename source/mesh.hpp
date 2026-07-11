#ifndef MESH_H
#define MESH_H

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include "level.hpp"

class Mesh
{
public:
    constexpr Mesh()
    {

    }

private:

};

enum class MESH_TYPE : uint16_t
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





consteval auto createMesh(MESH_TYPE const m) -> std::pair<std::vector<ffm::fixed32>,std::vector<uint16_t>> const
{
    constexpr char shipcsv[] =
    {
        #embed "../data/meshes/ship.csv" suffix(, 0)
    };
    constexpr char flatcsv[] =
    {
        #embed "../data/meshes/flat.csv" suffix(, 0)
    };
    constexpr char backhighcsv[] =
    {
        #embed "../data/meshes/backhigh.csv" suffix(, 0)
    };
    constexpr char backlowcsv[] =
    {
        #embed "../data/meshes/backlow.csv" suffix(, 0)
    };
    constexpr char backmidcsv[] =
    {
        #embed "../data/meshes/backmid.csv" suffix(, 0)
    };
    constexpr char bottomhighcsv[] =
    {
        #embed "../data/meshes/bottomhigh.csv" suffix(, 0)
    };
    constexpr char bottomlowcsv[] =
    {
        #embed "../data/meshes/bottomlow.csv" suffix(, 0)
    };
    constexpr char bottommidcsv[] =
    {
        #embed "../data/meshes/bottommid.csv" suffix(, 0)
    };
    constexpr char fronthighcsv[] =
    {
        #embed "../data/meshes/fronthigh.csv" suffix(, 0)
    };
    constexpr char frontlowcsv[] =
    {
        #embed "../data/meshes/frontlow.csv" suffix(, 0)
    };
    constexpr char frontmidcsv[] =
    {
        #embed "../data/meshes/frontmid.csv" suffix(, 0)
    };
    constexpr char lefthighcsv[] =
    {
        #embed "../data/meshes/lefthigh.csv" suffix(, 0)
    };
    constexpr char leftlowcsv[] =
    {
        #embed "../data/meshes/leftlow.csv" suffix(, 0)
    };
    constexpr char leftmidcsv[] =
    {
        #embed "../data/meshes/leftmid.csv" suffix(, 0)
    };
    constexpr char righthighcsv[] =
    {
        #embed "../data/meshes/righthigh.csv" suffix(, 0)
    };
    constexpr char rightlowcsv[] =
    {
        #embed "../data/meshes/rightlow.csv" suffix(, 0)
    };
    constexpr char rightmidcsv[] =
    {
        #embed "../data/meshes/rightmid.csv" suffix(, 0)
    };
    constexpr char tophighcsv[] =
    {
        #embed "../data/meshes/tophigh.csv" suffix(, 0)
    };
    constexpr char toplowcsv[] =
    {
        #embed "../data/meshes/toplow.csv" suffix(, 0)
    };
    constexpr char topmidcsv[] =
    {
        #embed "../data/meshes/topmid.csv" suffix(, 0)
    };
    constexpr char tunnelhighcsv[] =
    {
        #embed "../data/meshes/tunnelhigh.csv" suffix(, 0)
    };
    constexpr char tunnellowcsv[] =
    {
        #embed "../data/meshes/tunnellow.csv" suffix(, 0)
    };
    constexpr char tunnelmidcsv[] =
    {
        #embed "../data/meshes/tunnelmid.csv" suffix(, 0)
    };


    char const* csvp{nullptr};
    switch(m)
    {
    case MESH_TYPE::SHIP:
        csvp = shipcsv;
        break;
    case MESH_TYPE::FLAT:
        csvp = flatcsv;
        break;
    case MESH_TYPE::BACKHIGH:
        csvp = backhighcsv;
        break;
    case MESH_TYPE::BACKLOW:
        csvp = backlowcsv;
        break;
    case MESH_TYPE::BACKMID:
        csvp = backmidcsv;
        break;
    case MESH_TYPE::BOTTOMHIGH:
        csvp = bottomhighcsv;
        break;
    case MESH_TYPE::BOTTOMLOW:
        csvp = bottomlowcsv;
        break;
    case MESH_TYPE::BOTTOMMID:
        csvp = bottommidcsv;
        break;
    case MESH_TYPE::FRONTHIGH:
        csvp = fronthighcsv;
        break;
    case MESH_TYPE::FRONTLOW:
        csvp = frontlowcsv;
        break;
    case MESH_TYPE::FRONTMID:
        csvp = frontmidcsv;
        break;
    case MESH_TYPE::LEFTHIGH:
        csvp = lefthighcsv;
        break;
    case MESH_TYPE::LEFTLOW:
        csvp = leftlowcsv;
        break;
    case MESH_TYPE::LEFTMID:
        csvp = leftmidcsv;
        break;
    case MESH_TYPE::RIGHTHIGH:
        csvp = righthighcsv;
        break;
    case MESH_TYPE::RIGHTLOW:
        csvp = rightlowcsv;
        break;
    case MESH_TYPE::RIGHTMID:
        csvp = rightmidcsv;
        break;
    case MESH_TYPE::TOPHIGH:
        csvp = tophighcsv;
        break;
    case MESH_TYPE::TOPLOW:
        csvp = toplowcsv;
        break;
    case MESH_TYPE::TOPMID:
        csvp = topmidcsv;
        break;
    case MESH_TYPE::TUNNELHIGH:
        csvp = tunnelhighcsv;
        break;
    case MESH_TYPE::TUNNELLOW:
        csvp = tunnellowcsv;
        break;
    case MESH_TYPE::TUNNELMID:
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

consteval auto getMeshSizes() -> std::array<std::size_t, static_cast<std::size_t>(MESH_TYPE::NUM_MESHES)> const
{
    std::array<std::size_t, static_cast<std::size_t>(MESH_TYPE::NUM_MESHES)> r;
    for(std::size_t i = 0; i < static_cast<std::size_t>(MESH_TYPE::NUM_MESHES); ++i)
    {
        r[i] = createMesh(static_cast<MESH_TYPE>(i)).first.size() / 3;
    }
    return r;
}

constexpr static auto MESH_SIZES = getMeshSizes();

constexpr static auto MESH_SHIP = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::SHIP)]*3>(createMesh(MESH_TYPE::SHIP).first);
constexpr static auto MESH_FLAT = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::FLAT)]*3>(createMesh(MESH_TYPE::FLAT).first);
constexpr static auto MESH_BACKHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::BACKHIGH)*3]>(createMesh(MESH_TYPE::BACKHIGH).first);
constexpr static auto MESH_BACKLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::BACKLOW)]*3>(createMesh(MESH_TYPE::BACKLOW).first);
constexpr static auto MESH_BACKMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::BACKMID)]*3>(createMesh(MESH_TYPE::BACKMID).first);
constexpr static auto MESH_BOTTOMHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::BOTTOMHIGH)]*3>(createMesh(MESH_TYPE::BOTTOMHIGH).first);
constexpr static auto MESH_BOTTOMLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::BOTTOMLOW)]*3>(createMesh(MESH_TYPE::BOTTOMLOW).first);
constexpr static auto MESH_BOTTOMMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::BOTTOMMID)]*3>(createMesh(MESH_TYPE::BOTTOMMID).first);
constexpr static auto MESH_FRONTHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::FRONTHIGH)]*3>(createMesh(MESH_TYPE::FRONTHIGH).first);
constexpr static auto MESH_FRONTLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::FRONTLOW)]*3>(createMesh(MESH_TYPE::FRONTLOW).first);
constexpr static auto MESH_FRONTMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::FRONTMID)]*3>(createMesh(MESH_TYPE::FRONTMID).first);
constexpr static auto MESH_LEFTLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::LEFTLOW)]*3>(createMesh(MESH_TYPE::LEFTLOW).first);
constexpr static auto MESH_LEFTHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::LEFTHIGH)]*3>(createMesh(MESH_TYPE::LEFTHIGH).first);
constexpr static auto MESH_LEFTMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::LEFTMID)]*3>(createMesh(MESH_TYPE::LEFTMID).first);
constexpr static auto MESH_RIGHTHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::RIGHTHIGH)]*3>(createMesh(MESH_TYPE::RIGHTHIGH).first);
constexpr static auto MESH_RIGHTLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::RIGHTLOW)]*3>(createMesh(MESH_TYPE::RIGHTLOW).first);
constexpr static auto MESH_RIGHTMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::RIGHTMID)]*3>(createMesh(MESH_TYPE::RIGHTMID).first);
constexpr static auto MESH_TOPHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TOPHIGH)]*3>(createMesh(MESH_TYPE::TOPHIGH).first);
constexpr static auto MESH_TOPLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TOPLOW)]*3>(createMesh(MESH_TYPE::TOPLOW).first);
constexpr static auto MESH_TOPMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TOPMID)]*3>(createMesh(MESH_TYPE::TOPMID).first);
constexpr static auto MESH_TUNNELHIGH = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TUNNELHIGH)]*3>(createMesh(MESH_TYPE::TUNNELHIGH).first);
constexpr static auto MESH_TUNNELLOW = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TUNNELLOW)]*3>(createMesh(MESH_TYPE::TUNNELLOW).first);
constexpr static auto MESH_TUNNELMID = util::make_array<ffm::fixed32, MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TUNNELMID)]*3>(createMesh(MESH_TYPE::TUNNELMID).first);




constexpr static auto BALL_VERTS_SIZE = createMesh(MESH_TYPE::SHIP).first.size() / 3;
constexpr static auto BALL_VERTS = util::make_array<ffm::fixed32,BALL_VERTS_SIZE * 3>(createMesh(MESH_TYPE::SHIP).first);

constexpr static auto BALL_COLS_SIZE = createMesh(MESH_TYPE::SHIP).second.size();
constexpr static auto BALL_COLS = util::make_array<uint16_t,BALL_COLS_SIZE>(createMesh(MESH_TYPE::SHIP).second);



#endif
