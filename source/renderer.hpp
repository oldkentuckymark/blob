#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "mesh.hpp"
#include "cell.hpp"
#include "player.hpp"
#include "level.hpp"
#include "ffr.hpp"
#include <SDL3/SDL.h>

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

    auto draw() -> void
    {
        ctx.clear();




        ctx.present();
    }

private:
    Context ctx;

    uint16_t draw_distance_{10};

    Player* current_player_{nullptr};
    std::span<Vertex const> current_ship_mesh_{};

};


#endif // RENDERER_HPP
