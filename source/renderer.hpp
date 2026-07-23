#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "mesh.hpp"
#include "cell.hpp"
#include "player.hpp"
#include "level.hpp"

#include <span>
#include <meta>

class Renderer
{
public:
    Renderer()
    {

    }

    ~Renderer()
    {

    }


private:
    Player * current_player_;
    Level * current_level_;



    //giant global arrays here????

    [[nodiscard]] consteval static auto makeMeshes() -> std::pair<std::vector<Vertex>, std::vector<std::span<Vertex>>>
    {
        return {{},{}};
    }

    constexpr static auto verts_{std::define_static_array(Mesh::makeMeshPiece(Mesh::Piece::TOPLOW))};

};


#endif // RENDERER_HPP
