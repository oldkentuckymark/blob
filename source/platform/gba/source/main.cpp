#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>


#include <cstdint>

#include "ffr.hpp"
#include "mesh.hpp"

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_SELECT   0x0004
#define KEY_START    0x0008
#define KEY_RIGHT    0x0010
#define KEY_LEFT     0x0020
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080
#define KEY_R        0x0100
#define KEY_L        0x0200

#define KEY_MASK     0xFC00

class FFT
{
public:
    auto operator()(ffm::vec3& in) -> void
    {
        using namespace ffm;
        // Precompute sines and cosines
        fixed32 const cx = cos(rotation.x);
        fixed32 const sx = sin(rotation.x);

        fixed32 const cy = cos(rotation.y);
        fixed32 const sy = sin(rotation.y);

        fixed32 const cz = cos(rotation.z);
        fixed32 const sz = sin(rotation.z);

        // --- Rotate around X ---
        vec3 rx;
        rx.x = in.x;
        rx.y = in.y * cx - in.z * sx;
        rx.z = in.y * sx + in.z * cx;

        // --- Rotate around Y ---
        vec3 ry;
        ry.x = rx.x * cy + rx.z * sy;
        ry.y = rx.y;
        ry.z = -rx.x * sy + rx.z * cy;

        // --- Rotate around Z ---
        vec3 rz;
        rz.x = ry.x * cz - ry.y * sz;
        rz.y = ry.x * sz + ry.y * cz;
        rz.z = ry.z;

        in = rz;

        in = in - camPos;

    }

    ffm::vec3 camPos{0.0_fx,0.0_fx,0_fx};
    ffm::vec3 rotation{0_fx,0_fx,0_fx};

};

class Context final : public ffr::Context<FFT>
{
public:
    Context()
    {
    }

    ~Context()
    {

    }

    inline void clear() override
    {
        for(volatile uint16_t* p = vram;p < vram+(160*128);++p)
        {
            *p = 0;
        }
    }

    inline void present() override
    {
        flipPage();
    }

    inline void plot(int16_t x, int16_t y, uint16_t c) override
    {
        vram[(y*160)+x] = c;
    }

    inline void lineHorizontal(int16_t x0, int16_t y0, int16_t x1, uint16_t color) override
    {
        if(x0 > x1)
        {
            auto tmp = x0;
            x0 = x1;
            x1 = tmp;
        }

        for(uint16_t* p = (uint16_t*)&vram[y0*160+x0]; p <= &vram[y0*160+x1]; ++p)
        {
            *p = color;
        }
    }

private:
    volatile uint16_t * vram = reinterpret_cast<uint16_t*>(0x06000000);;

    inline void flipPage()
    {
        //return;
        // Wait for VBlank to avoid tearing
        //while (*(volatile u16*)0x04000006 >= 160); // Wait until VDraw ends
        //while (*(volatile u16*)0x04000006 < 160);  // Wait until VBlank ends

        // Toggle display page
        if (REG_DISPCNT & 0x0010)
        {
            REG_DISPCNT &= ~(0x0010); // Show front buffer (0x06000000)
            vram = reinterpret_cast<uint16_t*>(0x0600A000);  // Now draw to back buffer
        }
        else
        {
            REG_DISPCNT |= 0x0010;  // Show back buffer (0x0600A000)
            vram = reinterpret_cast<uint16_t*>(0x06000000); // Now draw to front buffer
        }
    }

};


uint32_t getKeyState(uint16_t key_code)
{
    return !(key_code & (REG_KEYINPUT | KEY_MASK) );
}

int main(void)
{

    // Set up the interrupt handlers
    irqInit();

    //irqSet( IRQ_VBLANK, VblankInterrupt);

    // Enable Vblank Interrupt to allow VblankIntrWait
    irqEnable(IRQ_VBLANK);
    SetMode( MODE_5 | BG2_ON );		// screen mode & background to display


    Context ctx;
    ctx.setViewPort(160,128);

    while (true)
    {
        bool inputs[10];
        //inputs[static_cast<uint16_t>(Game::Input::A)] = getKeyState(KEY_A);

        VBlankIntrWait();


        ctx.clear();

        ctx.setVertexPointer(3,0,(void*)MESH_SHIP.data());
        ctx.setColorPointer(0,(uint16_t*)BALL_COLS.data());
        ctx.getVertexFunction().camPos = {3.0_fx,0.0_fx,-2.8_fx};
        ctx.getVertexFunction().rotation = ctx.getVertexFunction().rotation + ffm::vec3{0.1_fx,0.1_fx,0.1_fx};
        ctx.drawArray(ffr::DrawType::Triangles,0,MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::SHIP)]);


        ctx.setVertexPointer(3,0,(void*)MESH_TUNNELLOW.data());
        ctx.getVertexFunction().camPos = {0.0_fx,0.0_fx,-2.8_fx};
        ctx.drawArray(ffr::DrawType::TrianglesWireFrame,0,MESH_SIZES[static_cast<std::size_t>(MESH_TYPE::TUNNELLOW)]);


        ctx.present();



    }

    return 0;
}
