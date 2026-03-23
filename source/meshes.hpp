#pragma once

#include <vector>

consteval auto createMesh(unsigned char data[])
{
    std::vector<int> f;
    f.push_back(4);
    f.push_back(6);

}


consteval auto createBall()
{

    constexpr unsigned char ballcsv[] =
    {
        #embed "../data/blobblue.pos"
    };


}





static constexpr unsigned char blobbluepos[] =
{
    #embed "../data/blobblue.pos"
};

static constexpr unsigned char blobbluecol[] =
{
    #embed "../data/blobblue.col"
};

static constexpr unsigned char blobredpos[] =
{
    #embed "../data/blobred.pos"
};

static constexpr unsigned char blobredcol[] =
{
    #embed "../data/blobred.col"
};

static constexpr unsigned char ballpos[] =
{
    #embed "../data/ball.pos"
};

static constexpr unsigned char ballcol[] =
{
    #embed "../data/ball.col"
};

static constexpr unsigned char cubepos[] =
{
    #embed "../data/m.pos"
};

static constexpr unsigned char cubecol[] =
{
    #embed "../data/m.col"
};
