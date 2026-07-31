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

};


#endif // RENDERER_HPP
