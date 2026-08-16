#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "mesh.hpp"
#include "cell.hpp"
#include "player.hpp"
#include "level.hpp"
#include "ffr.hpp"

#include <span>
#include <meta>


template <class Context>
class Renderer
{
public:
    Renderer()
    {
        ctx.setViewPort(240,160);
        ctx.setFaceCulling(0);
        ctx.getVertexFunction().camPos = {-2.0_fx,1.3_fx,-1.3_fx};
    }

    ~Renderer()
    {

    }

    auto setDrawDistance(uint16_t const blocks) -> void
    {
        draw_distance_ = blocks;
    }

    auto setPlayer(Player* p) -> void
    {
        current_player_ = p;
    }

    auto setPlayerMesh(std::span<Vertex const> m) -> void
    {
        current_ship_mesh_ = m;
    }

    auto setLevel(ILevel const * level) -> void
    {
        current_level_ = level;
    }

    auto draw() -> void
    {
        ctx.clear();
        ctx.getVertexFunction().camPos.z = ctx.getVertexFunction().camPos.z + 0.0001_fx;

        for(int16_t z = current_level_->getLength() - 1; z >=0; --z)
        {
            for(int16_t x = 0; x < ILevel::LEVEL_WIDTH; ++x)
            {

                ctx.getVertexFunction().modelPos = {ffm::fixed32(x-3) - 0.5_fx,0.0_fx,ffm::fixed32(z)};
                ctx.setColorPointer(0, current_level_->getCellColorBufferPtr(x,z));
                ctx.setVertexPointer(3,sizeof(Vertex), Mesh::CELL_MESHES[ static_cast<size_t>(current_level_->getCell(x,z).collision) ].data());
                ctx.drawArray(ffr::DrawType::Triangles,0,Mesh::CELL_MESHES[ static_cast<size_t>(current_level_->getCell(x,z).collision)].size());

            }
        }



        ctx.getVertexFunction().modelPos = current_player_->position;
        ctx.setColorPointer(sizeof(Vertex), &Mesh::SHIP_MESH.data()->color);
        ctx.setVertexPointer(3,sizeof(Vertex),Mesh::SHIP_MESH.data());
        //ctx.drawArray(ffr::DrawType::Triangles,0,Mesh::SHIP_MESH.size());





        ctx.present();
    }

private:
    Context ctx;

    uint16_t draw_distance_{0};

    ILevel const * current_level_{nullptr};

    Player* current_player_{nullptr};
    std::span<Vertex const> current_ship_mesh_{};

};


#endif // RENDERER_HPP
