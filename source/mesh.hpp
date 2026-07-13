#ifndef MESH_HPP
#define MESH_HPP

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include <cstdint>
#include <tuple>
#include <initializer_list>
#include "util.hpp"


//levels own meshes and creat them and light them

class Mesh
{
public:
    enum class Piece : uint8_t
    {
        SHIP = 0,
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

        NUM_PIECES
    };



    uint16_t start;
    uint16_t count;

private:




};

[[nodiscard]] consteval static auto createMeshPiece(Mesh::Piece const m) -> std::pair<std::vector<ffm::vec3>,std::vector<uint16_t>>
{

    constexpr char shipcsv[] =
        {
#embed "../data/meshes/ship.csv" suffix(, 0)
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
    case Mesh::Piece::SHIP:
        csvp = shipcsv;
        break;
    case Mesh::Piece::BACKHIGH:
        csvp = backhighcsv;
        break;
    case Mesh::Piece::BACKLOW:
        csvp = backlowcsv;
        break;
    case Mesh::Piece::BACKMID:
        csvp = backmidcsv;
        break;
    case Mesh::Piece::BOTTOMHIGH:
        csvp = bottomhighcsv;
        break;
    case Mesh::Piece::BOTTOMLOW:
        csvp = bottomlowcsv;
        break;
    case Mesh::Piece::BOTTOMMID:
        csvp = bottommidcsv;
        break;
    case Mesh::Piece::FRONTHIGH:
        csvp = fronthighcsv;
        break;
    case Mesh::Piece::FRONTLOW:
        csvp = frontlowcsv;
        break;
    case Mesh::Piece::FRONTMID:
        csvp = frontmidcsv;
        break;
    case Mesh::Piece::LEFTHIGH:
        csvp = lefthighcsv;
        break;
    case Mesh::Piece::LEFTLOW:
        csvp = leftlowcsv;
        break;
    case Mesh::Piece::LEFTMID:
        csvp = leftmidcsv;
        break;
    case Mesh::Piece::RIGHTHIGH:
        csvp = righthighcsv;
        break;
    case Mesh::Piece::RIGHTLOW:
        csvp = rightlowcsv;
        break;
    case Mesh::Piece::RIGHTMID:
        csvp = rightmidcsv;
        break;
    case Mesh::Piece::TOPHIGH:
        csvp = tophighcsv;
        break;
    case Mesh::Piece::TOPLOW:
        csvp = toplowcsv;
        break;
    case Mesh::Piece::TOPMID:
        csvp = topmidcsv;
        break;
    case Mesh::Piece::TUNNELHIGH:
        csvp = tunnelhighcsv;
        break;
    case Mesh::Piece::TUNNELLOW:
        csvp = tunnellowcsv;
        break;
    case Mesh::Piece::TUNNELMID:
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

        auto const r = static_cast<uint8_t>(dv[i+3] * 255.0);
        auto const g = static_cast<uint8_t>(dv[i+4] * 255.0);
        auto const b = static_cast<uint8_t>(dv[i+5] * 255.0);
        cvec.push_back((((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10) ));

    }


    return {fvec,cvec};
}

[[nodiscard]] consteval static auto assembleMeshPieces(std::initializer_list<Mesh::Piece> const & pieces) -> std::pair<std::vector<ffm::vec3>, std::vector<uint16_t>>
{
    std::vector<ffm::vec3> verts;
    std::vector<uint16_t> colors;

    for(auto i = 0; i < pieces.size(); ++i)
    {
        auto mp = createMeshPiece(static_cast<Mesh::Piece>(i));
        verts.insert(verts.end(), mp.first.begin(), mp.first.end());
        colors.insert(colors.begin(), mp.second.begin(), mp.second.end());
    }


    return {verts,colors};
}

[[nodiscard]] consteval static auto applyLightingColors(std::vector<ffm::vec3> const & verts, std::vector<uint16_t> const & colors,
                                                        ffm::vec3 const & lightdirection, ffm::vec3 lightColor) -> std::vector<uint16_t>
{
    std::vector<uint16_t> r;
    for(auto i = 0ul; i < verts.size(); i = i + 3)
    {
        auto colv3 = util::Convert555tovec3(colors[i]);

        auto norm = util::triangleNormal(verts[i+0],verts[i+1],verts[i+2]);
        auto newcolor = util::calculateLight(norm,colv3,lightdirection,lightColor);
        r.push_back(newcolor); r.push_back(newcolor); r.push_back(newcolor);
    }
    return r;
}

// separate meshes for left and right side blocks?
[[nodiscard]] consteval static auto createMeshes() -> std::tuple<std::vector<ffm::vec3>, std::vector<uint16_t>, std::vector<Mesh>>
{
    std::vector<ffm::vec3> verts;
    std::vector<uint16_t> colors;
    std::vector<Mesh> meshes;

    std::vector<std::pair<std::vector<ffm::vec3>, std::vector<uint16_t>>> assembled;

    Mesh m(0,0);

    m.start = m.count;
    assembled.push_back( assembleMeshPieces({Mesh::Piece::TOPLOW}) );
    m.count = assembled[0].first.size();
    meshes.push_back( m );

    m.start = m.count;
    assembled.push_back( assembleMeshPieces({Mesh::Piece::TOPMID}) );
    m.count = assembled[1].first.size();
    meshes.push_back( m );

    m.start = m.count;
    assembled.push_back( assembleMeshPieces({Mesh::Piece::TOPHIGH}) );
    m.count = assembled[1].first.size();
    meshes.push_back( m );





    return {verts,colors,meshes};
}




#endif // MESH_HPP
