#pragma once

#include "ffm.hpp"
#include "csv.hpp"
#include <vector>
#include <sstream>
#include <utility>

enum class MESHES : unsigned int
{
    BLOB_BLUE = 0,
    BLOB_RED,
    BALL
};



consteval auto createMesh(MESHES const m) -> std::pair<std::vector<ffm::fixed32>, std::vector<uint16_t>> const
{
    constexpr char blobbluecsv[] =
    {
        #embed "../data/m.csv" suffix(, 0)
    };

    constexpr char blobredcsv[] =
    {
        #embed "../data/m.csv" suffix(, 0)
    };

    constexpr char ballcsv[] =
    {
        #embed "../data/m.csv" suffix(, 0)
    };

    std::vector<double> dv =  parse_csv(blobbluecsv);

    std::vector<ffm::fixed32> fvec;
    std::vector<uint16_t> cvec;


    for(std::size_t i = 0; i < dv.size()/7; i = i + 7)
    {
        fvec.push_back(ffm::fixed32{dv[i+0]});
        fvec.push_back(ffm::fixed32{dv[i+1]});
        fvec.push_back(ffm::fixed32{dv[i+2]});

        uint8_t const r = static_cast<uint8_t>(dv[i+3] * 255.0);
        uint8_t const g = static_cast<uint8_t>(dv[i+4] * 255.0);
        uint8_t const b = static_cast<uint8_t>(dv[i+5] * 255.0);
        cvec.push_back((((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10) ));

    }


    return {fvec,cvec};
}

consteval auto createCols(MESHES const m) -> std::vector<ffm::fixed32> const
{
    constexpr char blobbluecsv[] =
        {
#embed "../data/m.csv" suffix(, 0)
        };

    constexpr char blobredcsv[] =
        {
#embed "../data/m.csv" suffix(, 0)
        };

    constexpr char ballcsv[] =
        {
#embed "../data/m.csv" suffix(, 0)
        };

    std::vector<double> dv =  parse_csv(blobbluecsv);

    std::vector<ffm::fixed32> fvec;
    std::vector<uint16_t> cvec;


    for(std::size_t i = 0; i < dv.size()/7; i = i + 7)
    {
        fvec.push_back(ffm::fixed32{dv[i+0]});
        fvec.push_back(ffm::fixed32{dv[i+1]});
        fvec.push_back(ffm::fixed32{dv[i+2]});

        uint8_t const r = static_cast<uint8_t>(dv[i+3] * 255.0);
        uint8_t const g = static_cast<uint8_t>(dv[i+4] * 255.0);
        uint8_t const b = static_cast<uint8_t>(dv[i+5] * 255.0);
        cvec.push_back((((r >> 3) & 31) | (((g >> 3) & 31) << 5) | (((b >> 3) & 31) << 10) ));

    }


    return fvec;
}



template<class T, std::size_t N>
consteval auto make_array(std::vector<T> vec) -> std::array<T, N>
{
    std::array<T, N> arr{};
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = vec[i];
    }
    return arr;
}

constexpr static auto BALL_VERTS = make_array<ffm::fixed32, createMesh(MESHES::BALL).first.size()>(
    createMesh(MESHES::BALL).first);


constexpr static




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
