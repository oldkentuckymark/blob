#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>


#include <cstdint>

#include "ffr.hpp"
#include "mesh.hpp"
#include "level.hpp"
#include "renderer.hpp"

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


        in = in + modelPos - camPos;

    }

    ffm::vec3 camPos{0.0_fx,0.0_fx,0_fx};
    ffm::vec3 modelPos{0.0_fx,0.0_fx,0.0_fx};

};
\


class Context final : public ffr::Context<FFT>
{
public:

    Context()
    {
        // Enable Vblank Interrupt to allow VblankIntrWait
        irqEnable(IRQ_VBLANK);
        SetMode( MODE_5 | BG2_ON );
    }


    inline void clear() override
    {
        for(volatile uint16_t* p = vram;p < vram+(width*height);++p)
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
        vram[(y*width)+x] = c;
    }

    inline void lineHorizontal(int16_t x0, int16_t y0, int16_t x1, uint16_t color) override
    {
        if(x0 > x1) { auto t  = x0; x0 = x1; x1 = t; }
        if(x0 < 0) { x0 = 0; }
        if(x1 >= viewport_width_) { x1 = viewport_width_ - 1; }

        for(uint16_t* p = (uint16_t*)&vram[y0*width+x0]; p <= &vram[y0*width+x1]; ++p)
        {
            *p = color;
        }
    }

private:
    constexpr static uint16_t width = 160;
    constexpr static uint16_t height = 128;

    volatile uint16_t * FB = (uint16_t*)0x6000000;
    volatile uint16_t * BB = (uint16_t*)0x600A000;


    volatile uint16_t * vram = BB;

    inline void flipPage()
    {

        //while(REG_VCOUNT >= 160); // Wait for vertical blank

        if(REG_DISPCNT & 0x10)
        {
            REG_DISPCNT &= ~0x10; // Show Frame 0
            vram = BB;
        }
        else
        {
            REG_DISPCNT |= 0x10;  // Show Frame 1
            vram = FB;
        }
    }

};



/*
class Context final : public ffr::Context<FFT>
{
public:

    Context()
    {
        // Enable Vblank Interrupt to allow VblankIntrWait
        irqEnable(IRQ_VBLANK);
        SetMode( MODE_3 | BG2_ON );
    }

    inline void clear() override
    {
        for(volatile uint16_t* p = vram;p < vram+(width*height);++p)
        {
            *p = 0;
        }
    }

    inline void present() override
    {

    }

    inline void plot(int16_t x, int16_t y, uint16_t c) override
    {
        vram[(y*width)+x] = c;
    }

    inline void lineHorizontal(int16_t x0, int16_t y0, int16_t x1, uint16_t color) override
    {
        if(x0 > x1)
        {
            auto tmp = x0;
            x0 = x1;
            x1 = tmp;
        }

        for(uint16_t* p = (uint16_t*)&vram[(y0*width)+x0]; p <= &vram[(y0*width)+x1]; ++p)
        {
            *p = color;
        }
    }

private:
    constexpr static uint16_t width = 240;
    constexpr static uint16_t height = 160;

    volatile uint16_t * vram = (uint16_t*)0x6000000;

};
*/


uint32_t getKeyState(uint16_t key_code)
{
    return !(key_code & (REG_KEYINPUT | KEY_MASK) );
}

int main(void)
{
    // Set up the interrupt handlers
    irqInit();
    //irqSet( IRQ_VBLANK, VblankInterrupt);

    Renderer<Context> renderer;

    Player player;

    renderer.setPlayer(&player);
    renderer.setPlayerMesh(Mesh::SHIP_MESH);
    renderer.setDrawDistance(10);
    renderer.setLevel(&level0);




    while (true)
    {
        bool inputs[10] = {};
        //inputs[static_cast<uint16_t>(Game::Input::A)] = getKeyState(KEY_A);



        renderer.draw();
    }

    return 0;
}

