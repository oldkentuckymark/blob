#ifndef MESH_HPP
#define MESH_HPP

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include <cstdint>
#include "level.hpp"

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


class Mesh
{
public:
    consteval Mesh()
    {

    }

    ffm::vec3 * verts;
    uint16_t numVerts;
    uint16_t * colors;
    uint16_t numColors;
};


//loadverts()??? better name??
[[nodiscard]] consteval auto createMesh(MESH_TYPE const m) -> std::pair<std::vector<ffm::vec3>,std::vector<uint16_t>> const
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



    char const* csvp{shipcsv};
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


    std::vector<double> dv = parse_csv(csvp);

    std::vector<ffm::vec3> fvec;
    std::vector<uint16_t> cvec;


    for(std::size_t i = 0; i < dv.size(); i = i + 7)
    {
        ffm::fixed32 const x =  static_cast<ffm::fixed32>(dv[i+0]);
        ffm::fixed32 const y =  static_cast<ffm::fixed32>(dv[i+1]);
        ffm::fixed32 const z =  static_cast<ffm::fixed32>(dv[i+2]);
        fvec.push_back({x,y,z});

        uint8_t const r = static_cast<uint8_t>(dv[i+3] * 255.0);
        uint8_t const g = static_cast<uint8_t>(dv[i+4] * 255.0);
        uint8_t const b = static_cast<uint8_t>(dv[i+5] * 255.0);
        cvec.push_back((((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10) ));

    }


    return {fvec,cvec};
}


constexpr static auto TUNNEL_LOW = util::make_array<ffm::vec3,createMesh(MESH_TYPE::TUNNELLOW).first.size()>(createMesh(MESH_TYPE::TUNNELLOW).first);





#endif // MESH_HPP
