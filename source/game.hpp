#ifndef GAME_HPP
#define GAME_HPP

#include "player.hpp"
#include "level.hpp"

class Game
{
public:
    Game()
    {

    }

    ~Game()
    {

    }


    auto setCurrentLevel(ILevel const * lvl) -> void
    {
        current_level_ = lvl;

    }


private:
    Player player_;
    ILevel const* current_level_{nullptr};
};



#endif // GAME_HPP
