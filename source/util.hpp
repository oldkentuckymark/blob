#pragma once

#include <array>

namespace util
{


template<class T, auto SIZE, auto FUNC>
constexpr auto makeTable() -> std::array<T, SIZE>
{
    std::array<T,SIZE> r{};
    for(std::size_t i = 0; i < SIZE; ++i)
    {
        r[i] = FUNC(i);
    }
    return r;
}


}

