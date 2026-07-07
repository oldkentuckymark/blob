#ifndef CELL_HPP
#define CELL_HPP

#include <cstdint>

class Cell
{
public:

    enum class Collision : uint8_t
    {
        Empty,
        PlaneLow,
        PlaneMid,
        PlaneHigh,
        BlockLow,
        BlockMid,
        BlockHigh,
        TunnelLow,
        TunnelMid,
        TunnelHigh,
        TunnelPlaneLow,
        TunnelPlaneMid,
        TunnelPlaneHigh,
        TunnelBlockLow,
        TunnelBlockMid,
        TunnelBlockHigh
    };

    enum class Type : uint8_t
    {
        Normal,
        Oxygen,
        Boost,
        Sticky,
        Slippery,
        Kill,
        End
    };



    constexpr Cell() = default;

    constexpr Cell(Collision const c, Type const t, uint16_t const tc, uint16_t const sc) :
        collision(c), type(t), topColor(tc), sideColor(sc)
    {

    }

    ~Cell() = default;

    Collision collision{Collision::Empty};
    Type type{Type::Normal};
    uint16_t topColor{0};
    uint16_t sideColor{0};


private:



};

#endif // CELL_HPP
