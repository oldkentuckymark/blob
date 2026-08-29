//TODO: parse CSV/OBJ files at compile time for #embed


#include "ffr.hpp"
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include "mesh.hpp"
#include "renderer.hpp"
#include "level.hpp"
#include "game.hpp"

#ifdef GBA
#define IWRAM_CODE	__attribute__((section(".iwram"), long_call))
#define EWRAM_CODE	__attribute__((section(".ewram"), long_call))

#define IWRAM_DATA	__attribute__((section(".iwram_data")))
#define EWRAM_DATA	__attribute__((section(".ewram_data")))

#define ARM_CODE	__attribute__((target("arm")))
#define THUMB_CODE	__attribute__((target("thumb")))

#define EWRAM_BSS	__attribute__((section(".sbss")))
#define ALIGN(m)	__attribute__((aligned (m)))
#else
#define IWRAM_CODE
#endif

class VertexFunction
{
public:

    auto operator()(ffm::vec3& in) -> void
    {

        using namespace ffm;
        // Precompute sines and cosines
        fixed32 const cx = cos(modelRotation.x);
        fixed32 const sx = sin(modelRotation.x);

        fixed32 const cy = cos(modelRotation.y);
        fixed32 const sy = sin(modelRotation.y);

        fixed32 const cz = cos(modelRotation.z);
        fixed32 const sz = sin(modelRotation.z);

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

        in = in + modelPos - camPos;

    }

    ffm::vec3 camPos{0.0_fx,0.0_fx,0_fx};
    ffm::vec3 modelPos{0.0_fx,0.0_fx,0.0_fx};
    ffm::vec3 modelRotation{0_fx,0_fx,0_fx};

};


class Context final : public ffr::Context<VertexFunction>
{
public:
    Context()
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer("blob",240*4,160*4,0,&win,&ren);
        SDL_SetRenderScale(ren,4.0f,4.0f);
    }

    ~Context() override
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
    }

    auto clear() -> void override
    {
        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);
    }

    auto present() -> void override
    {

        SDL_RenderPresent(ren);
    }

    auto lineHorizontal(int16_t x0, int16_t y0, int16_t x1, uint16_t color) -> void
    {
        if(x0 > x1) { auto t  = x0; x0 = x1; x1 = t; }
        if(x0 < 0) { x0 = 0; }
        if(x1 >= viewport_width_) { x1 = viewport_width_ - 1; }
        auto cc = util::Convert555to888(color);
        SDL_SetRenderDrawColor(ren,cc[0],cc[1],cc[2],255);
        SDL_RenderLine(ren, x1,y0,x0,y0);
    }

    auto plot(int16_t x, int16_t y, uint16_t c) -> void override
    {
        auto cc = util::Convert555to888(c);
        SDL_SetRenderDrawColor(ren,cc[0],cc[1],cc[2],255);
        SDL_RenderPoint(ren,x,y);
    }

private:
    SDL_Window* win{nullptr};
    SDL_Renderer* ren{nullptr};


};

auto main() -> int
{

    auto const * const lp = &level0;


    auto c1 = std::chrono::steady_clock::now();
    auto c2 = c1;

    Renderer<Context> renderer;

    Player player;

    renderer.setPlayer(&player);
    renderer.setPlayerMesh(Mesh::SHIP_MESH);
    renderer.setDrawDistance(10);
    renderer.setLevel(&level0);

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

            }
        }
        if(bool doinput = true)
        {
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
        }

        c2 = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>( c2.time_since_epoch()-c1.time_since_epoch()).count() >= 16)
        {
            renderer.draw();

            auto dt = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::milliseconds>(c2-c1).count());
            c1 = std::chrono::steady_clock::now();
        }




    }
//color buffer ptrs not being set right



    return 0;
}
