#ifndef MESH_HPP
#define MESH_HPP

#include "ffm.hpp"
#include "csv.hpp"
#include <utility>
#include <cstdint>
#include <initializer_list>
#include "level.hpp"


//giant vertex buffer global, mesh is indices, do index rendering???

class Mesh
{
public:
    enum class Piece : uint8_t
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


    consteval Mesh(std::initializer_list<Piece> pieces)
    {
        for(auto p : pieces)
        {
            auto mp = createMeshPiece(p);
            for(auto& i : mp.first)
            {
                verts.push_back(i);
            }
            for(auto& i : mp.second)
            {
                colors.push_back(i);
            }

        }




    }

    consteval auto size() const -> std::size_t
    {
        return verts.size();
    }

    std::vector<ffm::vec3> verts;
    std::vector<uint16_t> colors;
private:



    [[nodiscard]] consteval static auto createMeshPiece(Piece const m) -> std::pair<std::vector<ffm::vec3>,std::vector<uint16_t>> const
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
        case Piece::SHIP:
            csvp = shipcsv;
            break;
        case Piece::FLAT:
            csvp = flatcsv;
            break;
        case Piece::BACKHIGH:
            csvp = backhighcsv;
            break;
        case Piece::BACKLOW:
            csvp = backlowcsv;
            break;
        case Piece::BACKMID:
            csvp = backmidcsv;
            break;
        case Piece::BOTTOMHIGH:
            csvp = bottomhighcsv;
            break;
        case Piece::BOTTOMLOW:
            csvp = bottomlowcsv;
            break;
        case Piece::BOTTOMMID:
            csvp = bottommidcsv;
            break;
        case Piece::FRONTHIGH:
            csvp = fronthighcsv;
            break;
        case Piece::FRONTLOW:
            csvp = frontlowcsv;
            break;
        case Piece::FRONTMID:
            csvp = frontmidcsv;
            break;
        case Piece::LEFTHIGH:
            csvp = lefthighcsv;
            break;
        case Piece::LEFTLOW:
            csvp = leftlowcsv;
            break;
        case Piece::LEFTMID:
            csvp = leftmidcsv;
            break;
        case Piece::RIGHTHIGH:
            csvp = righthighcsv;
            break;
        case Piece::RIGHTLOW:
            csvp = rightlowcsv;
            break;
        case Piece::RIGHTMID:
            csvp = rightmidcsv;
            break;
        case Piece::TOPHIGH:
            csvp = tophighcsv;
            break;
        case Piece::TOPLOW:
            csvp = toplowcsv;
            break;
        case Piece::TOPMID:
            csvp = topmidcsv;
            break;
        case Piece::TUNNELHIGH:
            csvp = tunnelhighcsv;
            break;
        case Piece::TUNNELLOW:
            csvp = tunnellowcsv;
            break;
        case Piece::TUNNELMID:
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

};

template<int N>
class StaticMesh
{
public:
    consteval StaticMesh(Mesh const & mesh)
    {
        for(auto i = 0ul; i < N; ++i)
        {
            verts[i] = mesh.verts[i];
            colors[i] = mesh.colors[i];
        }
    }


    std::array<ffm::vec3, N> verts;
    std::array<uint16_t, N> colors;


private:


};

constexpr static StaticMesh<Mesh({Mesh::Piece::SHIP}).size()> MESH_SHIP(Mesh({Mesh::Piece::SHIP}));
constexpr static StaticMesh<Mesh({Mesh::Piece::FLAT}).size()> MESH_FLAT(Mesh({Mesh::Piece::FLAT}));
constexpr static StaticMesh<Mesh({Mesh::Piece::FLAT}).size()> MESH_BLOCK(Mesh({Mesh::Piece::FLAT}));




#endif // MESH_HPP
