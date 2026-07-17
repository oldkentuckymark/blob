#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "mesh.hpp"
#include "cell.hpp"
#include "player.hpp"
#include "level.hpp"

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


};


#endif // RENDERER_HPP
