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
        ctx.setViewPort(160,128);
        ctx.setNearZ(0.5_fx);
        ctx.getVertexFunction().camPos = {0._fx,1.3_fx,5.3_fx};
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
        ctx.setFaceCulling(ffr::FaceCullMode::Back);
        ctx.clear();
        ctx.getVertexFunction().camPos.z = ctx.getVertexFunction().camPos.z - 0.005_fx;

        for(int16_t z = 6; z >=0; --z)
        {
            for(int16_t x = 0; x < ILevel::LEVEL_WIDTH; ++x)
            {
                auto collision {static_cast<size_t>(current_level_->getCell(x,z).collision)};
                auto const & cell {current_level_->getCell(x,z)};

                if(collision)
                {
                    if(cell.collision == Cell::Collision::TunnelLow || cell.collision == Cell::Collision::TunnelMid || cell.collision == Cell::Collision::TunnelHigh) {ctx.setFaceCulling(ffr::FaceCullMode::None);}
                    ctx.getVertexFunction().modelPos = {ffm::fixed32(static_cast<int16_t>(x-3)) - 0.5_fx,0.0_fx,ffm::fixed32(static_cast<int16_t>(z*2))};
                    auto const colptr{current_level_->getCellColorBufferPtr(x,z)};
                    ctx.setColorPointer(0, colptr);
                    ctx.setVertexPointer(3,sizeof(Vertex), Mesh::CELL_MESHES[ collision ].data());
                    ctx.drawArray(ffr::DrawType::Quads,0,Mesh::CELL_MESHES[ collision].size());
                    ctx.setFaceCulling(ffr::FaceCullMode::Back);
                }

            }
        }



        ctx.getVertexFunction().modelPos = current_player_->position;
        ctx.setColorPointer(sizeof(Vertex), &Mesh::SHIP_MESH.data()->color);
        ctx.setVertexPointer(3,sizeof(Vertex),Mesh::SHIP_MESH.data());
        ctx.drawArray(ffr::DrawType::Triangles,0,Mesh::SHIP_MESH.size());




        //ctx.quad(20,50,50,0,100,100,20,100, 65535);

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
