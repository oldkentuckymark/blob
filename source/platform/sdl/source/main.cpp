#include "ffr.hpp"
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include "meshes.hpp"


class FFT
{
public:
    auto operator () (ffm::vec3& in) -> void
    {

    }
};

class FFT2
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


class SDL_Context final : public ffr::Context<FFT2>
{
public:
    SDL_Context()
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer("blob",240*6,160*6,0,&win,&ren);
    }

    ~SDL_Context()
    {

    }

    void clear() final
    {
        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);
    }

    void present()
    {

        SDL_RenderPresent(ren);
    }

    void plot(int16_t x, int16_t y, uint16_t c)
    {
        auto cc = ffr::Convert555to888(c);
        SDL_SetRenderDrawColor(ren,cc[0],cc[1],cc[2],255);
        SDL_RenderPoint(ren,x,y);
    }

private:
    SDL_Window* win;
    SDL_Renderer* ren;


};

auto main() -> int
{


    SDL_Context ctx;
    ctx.setViewPort(240,160);


    auto c1 = std::chrono::steady_clock::now();
    auto c2 = c1;

    bool running = true;
    while (running)
    {
        bool inputs[10] = {};
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if(e.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if(e.type == SDL_EVENT_MOUSE_MOTION)
            {
                //cam0ref.yaw_ -= (float)e.motion.xrel / 200.f;

            }
        }
        auto const * const keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_ESCAPE])
        {
            running = false;
        }
        if (keys[SDL_SCANCODE_X])
        {
            inputs[0] = true;
        }
        if (keys[SDL_SCANCODE_Z])
        {
            inputs[1] = true;
        }
        if (keys[SDL_SCANCODE_BACKSPACE])
        {
            inputs[2] = true;
        }
        if (keys[SDL_SCANCODE_RETURN])
        {
            inputs[3] = true;
        }
        if (keys[SDL_SCANCODE_RIGHT])
        {
            inputs[4] = true;
        }
        if (keys[SDL_SCANCODE_LEFT])
        {
            inputs[5] = true;
        }
        if (keys[SDL_SCANCODE_UP])
        {
            inputs[6] = true;
        }
        if (keys[SDL_SCANCODE_DOWN])
        {
            inputs[7] = true;
        }
        if (keys[SDL_SCANCODE_S])
        {
            inputs[8] = true;
        }
        if (keys[SDL_SCANCODE_A])
        {
            inputs[9] = true;
        }


        c2 = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>( c2.time_since_epoch()-c1.time_since_epoch()).count() >= 16)
        {
            auto dt = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::milliseconds>(c2-c1).count());
            c1 = std::chrono::steady_clock::now();
        }

        ctx.clear();

        ctx.setVertexPointer(3,0,(void*)ballpos);
        ctx.setColorPointer(0, (uint16_t*)ballcol);
        ctx.getVertexFunction().camPos = {0.0_fx,0.0_fx,-2.8_fx};
        ctx.drawArray(ffr::DrawType::Triangles, 0, sizeof(ballpos)/sizeof(ffm::vec3));



        ctx.setVertexPointer(3,0,(void*)blobbluepos);
        ctx.setColorPointer(0, (uint16_t*)blobbluecol);
        ctx.getVertexFunction().camPos = {-2.0_fx,0.0_fx,-2.0_fx};
        ctx.drawArray(ffr::DrawType::Triangles, 0, sizeof(blobbluepos)/sizeof(ffm::vec3));


        ctx.setVertexPointer(3,0,(void*)blobredpos);
        ctx.setColorPointer(0, (uint16_t*)blobredcol);
        ctx.getVertexFunction().camPos = {2.0_fx,0.0_fx,-2.0_fx};
        ctx.drawArray(ffr::DrawType::Triangles, 0, sizeof(blobredpos)/sizeof(ffm::vec3));

        ctx.present();

    }

    return 0;
}
