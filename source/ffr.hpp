#pragma once

#include <cstdint>

#include "ffm.hpp"

namespace ffr
{

using namespace ffm;

constexpr auto Convert888to555(uint8_t const r, uint8_t const g, uint8_t const b) -> uint32_t
{
    return (((r >> 3) & 31) |
            (((g >> 3) & 31) << 5) |
            (((b >> 3) & 31) << 10) );

}

constexpr auto Convert555to888(uint32_t color) -> std::array<uint8_t, 4>
{
    uint8_t const red = (color & 31) << 3;
    uint8_t const green = ((color >> 5) & 31) << 3;
    uint8_t const blue = ((color >> 10) & 31) << 3;
    uint8_t const alpha = 255;
    return {red,green,blue,alpha};
}

enum class DrawType : uint32_t
{
    Points = 1,
    Lines = 2,
    Triangles = 3
};

class VertexFunction
{
public:
    virtual auto operator()(ffm::vec3&) -> void = 0;
};

class Context
{
public:
    Context() = default;
    virtual ~Context() = default;

    Context(Context&) = delete;
    auto operator = (Context&) = delete;
    Context(Context&&) = delete;
    auto operator = (Context&&) = delete;


    virtual auto plot(int16_t x, int16_t y, uint16_t color) -> void = 0;

    virtual auto line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) -> void
    {
        bool const steep = ffm::abs(y1 - y0) > ffm::abs(x1 - x0);

        if (steep)
        {
            int16_t tmp = x0;
            x0 = y0;
            y0 = tmp;

            tmp = x1;
            x1 = y1;
            y1 = tmp;
        }

        if (x0 > x1)
        {
            int32_t tmp = x0;
            x0 = x1;
            x1 = tmp;

            tmp = y0;
            y0 = y1;
            y1 = tmp;
        }

        int32_t const dx = x1 - x0;
        int32_t const dy = ffm::abs(y1 - y0);
        int32_t error = dx / 2;
        int32_t const ystep = (y0 < y1) ? 1 : -1;
        int32_t y = y0;

        for (int32_t x = x0; x <= x1; ++x)
        {
            if (steep)
            {
                plot(y, x, color);
            }
            else
            {
                plot(x, y, color);
            }
            error -= dy;
            if (error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }

    virtual auto lineHorizontal(int16_t x0, int16_t y0, int16_t x1, uint16_t color) -> void
    {
        line(x0, y0, x1, y0, color);
    }
    virtual auto lineVertical(int16_t x0, int16_t y0, int16_t y1, uint16_t color) -> void
    {
        line(x0, y0, x0, y1, color);
    }

    virtual auto triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) -> void
    {
        //std::cout << x0 << ", " << x1 << ", " << y0 << ", " << y1 << ", " << x2 << ", " << y2 << "\n";

        // This implementation uses only 16-bit integer math (Bresenham-style)
        // and avoids all C++ standard library functions.
        int32_t v_top_x = x0, v_top_y = y0;
        int32_t v_mid_x = x1, v_mid_y = y1;
        int32_t v_bot_x = x2, v_bot_y = y2;
        int32_t temp_x, temp_y;

        // --- 1. Manual Sort ---
        // Sort points so v_top_y <= v_mid_y <= v_bot_y
        if (v_top_y > v_mid_y)
        {
            temp_x = v_top_x;
            v_top_x = v_mid_x;
            v_mid_x = temp_x;
            temp_y = v_top_y;
            v_top_y = v_mid_y;
            v_mid_y = temp_y;
        }
        if (v_mid_y > v_bot_y)
        {
            temp_x = v_mid_x;
            v_mid_x = v_bot_x;
            v_bot_x = temp_x;
            temp_y = v_mid_y;
            v_mid_y = v_bot_y;
            v_bot_y = temp_y;
        }
        if (v_top_y > v_mid_y)
        {
            temp_x = v_top_x;
            v_top_x = v_mid_x;
            v_mid_x = temp_x;
            temp_y = v_top_y;
            v_top_y = v_mid_y;
            v_mid_y = temp_y;
        }

        // --- 2. Trivial Case: Horizontal line ---
        if (v_top_y == v_bot_y)
        {
            int32_t min_x = v_top_x;
            int32_t max_x = v_top_x;
            if (v_mid_x < min_x)
                min_x = v_mid_x;
            if (v_mid_x > max_x)
                max_x = v_mid_x;
            if (v_bot_x < min_x)
                min_x = v_bot_x;
            if (v_bot_x > max_x)
                max_x = v_bot_x;
            lineHorizontal(min_x, v_top_y, max_x, color);
            return;
        }

        // --- 3. Setup Bresenham Edge Steppers ---
        // Stepper A traces the long edge (top -> bottom)
        int32_t dx_a = v_bot_x - v_top_x;
        int32_t dy_a = v_bot_y - v_top_y;
        int32_t x_step_a = 1;
        if (dx_a < 0)
        {
            dx_a = -dx_a;
            x_step_a = -1;
        }
        int32_t error_a = dy_a >> 1;
        int32_t x_a = v_top_x;

        // Stepper B will trace the upper int32_t edge (top -> middle) first
        int32_t dx_b = v_mid_x - v_top_x;
        int32_t dy_b = v_mid_y - v_top_y;
        int32_t x_step_b = 1;
        if (dx_b < 0)
        {
            dx_b = -dx_b;
            x_step_b = -1;
        }
        int32_t error_b = dy_b >> 1;
        int32_t x_b = v_top_x;

        // --- 4. Top half of triangle ---
        // This part is skipped if the triangle is flat-top (top_y == mid_y)
        for (int32_t y = v_top_y; y < v_mid_y; y++)
        {
            lineHorizontal(x_a, y, x_b, color);

            // Advance stepper A along the long edge
            error_a -= dx_a;
            while (error_a < 0)
            {
                x_a += x_step_a;
                error_a += dy_a;
            }

            // Advance stepper B along the upper int32_t edge
            if (dy_b > 0)
            {
                // Avoid division by zero on a horizontal top edge
                error_b -= dx_b;
                while (error_b < 0)
                {
                    x_b += x_step_b;
                    error_b += dy_b;
                }
            }
        }

        // --- 5. Bottom half of triangle ---
        // Re-setup stepper B for the lower int32_t edge (middle -> bottom)
        dx_b = v_bot_x - v_mid_x;
        dy_b = v_bot_y - v_mid_y;
        x_step_b = 1;
        if (dx_b < 0)
        {
            dx_b = -dx_b;
            x_step_b = -1;
        }
        error_b = dy_b >> 1;
        x_b = v_mid_x;

        for (int32_t y = v_mid_y; y <= v_bot_y; y++)
        {
            lineHorizontal(x_a, y, x_b, color);

            // Advance stepper A along the long edge
            error_a -= dx_a;
            while (error_a < 0)
            {
                x_a += x_step_a;
                error_a += dy_a;
            }

            // Advance stepper B along the lower int32_t edge
            if (dy_b > 0)
            {
                // Avoid division by zero on a horizontal bottom edge
                error_b -= dx_b;
                while (error_b < 0)
                {
                    x_b += x_step_b;
                    error_b += dy_b;
                }
            }
        }
    }

    virtual auto clear() -> void {}
    virtual auto present() -> void {}

    auto setVertexFunction(VertexFunction* vf) -> void
    {
        vf_ = vf;
    }

    auto setVertexPointer(uint32_t size, uint32_t stride, void* vp) -> void
    {
        vertex_size_ = size;
        vertex_stride_ = stride;
        vertex_pointer_= vp;
    }

    //1 uint16_t per primitve
    auto setColorPointer(uint32_t stride, uint16_t* cp)-> void
    {
        color_stride_ = stride;
        color_pointer_ = cp;
    }
    auto setViewPort(int32_t w, int32_t h) -> void
    {
        viewport_width_ = w;
        viewport_height_ = h;
        viewport_width_fx_ = static_cast<int16_t>(w);
        viewport_height_fx_ = static_cast<int16_t>(h);
        aspect_ratio_ = 1.0_fx / (viewport_width_fx_ / viewport_height_fx_);
    }

    auto drawArray(DrawType dt, uint32_t first, uint32_t count) -> void
    {
        current_draw_type_ = dt;
        working_vertex_buffer_size_ = 0;
        working_color_buffer_size_ = 0;

        if(vertex_size_ == 2)
        {
            for(uint32_t i = first; i < first+count; ++i)
            {
                working_vertex_buffer_[working_vertex_buffer_size_] =
                {
                    reinterpret_cast<ffm::vec2*>(vertex_pointer_)[i].x,
                    reinterpret_cast<ffm::vec2*>(vertex_pointer_)[i].y,
                    0.0_fx
                };
                ++working_vertex_buffer_size_;
            }
        }
        else if(vertex_size_ == 3)
        {
            for(uint32_t i = first; i < first+count; ++i)
            {
                working_vertex_buffer_[working_vertex_buffer_size_] = reinterpret_cast<vec3*>(vertex_pointer_)[i];
                ++working_vertex_buffer_size_;
            }
        }

        for(uint16_t i = first; i < (first + count); ++i)
        {
            working_color_buffer_[working_color_buffer_size_] = color_pointer_[i];
            ++working_color_buffer_size_;
        }


        //vertex_pipeline_();

        //run vertex function
        for(uint32_t i = 0; i < working_vertex_buffer_size_; ++i)
        {
            vf_[0](working_vertex_buffer_[i]);
        }


        uint32_t col = 0;
        for(uint32_t i = 0; i < working_vertex_buffer_size_; ++i)
        {
            uint16_t const & ccs{working_color_buffer_[col]};

            if(current_draw_type_ == DrawType::Points)
            {
                vec3& cvs = working_vertex_buffer_[i];
                project_to_ndc_(cvs);
                to_screen_space_(cvs);

                if(clip_point_screen_space_(cvs))
                {
                    plot(static_cast<int16_t>(cvs.x), static_cast<int16_t>(cvs.y), ccs);
                }
                ++col;
            }
            else if(current_draw_type_ == DrawType::Lines)
            {
                vec3& p0{working_vertex_buffer_[i]};
                vec3& p1{working_vertex_buffer_[i+1]};
                project_to_ndc_(p0); project_to_ndc_(p1);
                to_screen_space_(p0); to_screen_space_(p1);

                if(clip_line_screen_space_(p0, p1))
                {
                    line(static_cast<int16_t>(p0.x), static_cast<int16_t>(p0.y),
                         static_cast<int16_t>(p1.x), static_cast<int16_t>(p1.y), ccs);
                }
                ++i;
                ++col;
            }
            else if(current_draw_type_ == DrawType::Triangles)
            {
                vec3& v0{working_vertex_buffer_[i]};
                vec3& v1{working_vertex_buffer_[i+1]};
                vec3& v2{working_vertex_buffer_[i+2]};
                project_to_ndc_(v0);project_to_ndc_(v1);project_to_ndc_(v2);

                std::array<vec3,15> arr;
                auto n = clip_and_triangulate_ndc(v0,v1,v2,arr);
                for(uint32_t k = 0; k < n; k = k + 3)
                {
                    if(is_front_facing(arr[k], arr[k+1], arr[k+2]))
                    {

                        to_screen_space_(arr[k]);to_screen_space_(arr[k+1]);to_screen_space_(arr[k+2]);
                        triangle(static_cast<int16_t>(arr[k].x), static_cast<int16_t>(arr[k].y),
                                 static_cast<int16_t>(arr[k+1].x), static_cast<int16_t>(arr[k+1].y),
                                 static_cast<int16_t>(arr[k+2].x), static_cast<int16_t>(arr[k+2].y), ccs);
                    }
                }



                i = i + 2;
                ++col;
            }
        }

    }

private:

    static constexpr uint16_t MAX_VERTS = 128;

    [[nodiscard]] auto clip_point_screen_space_(vec3 const & p) -> bool
    {
        return !(p.x < 0.0_fx || p.x >= viewport_width_fx_ ||
                p.y < 0.0_fx || p.y >= viewport_height_fx_ );
    }

    [[nodiscard]] auto clip_line_screen_space_(vec3 &p0, vec3 &p1) -> bool
    {
        const int32_t xmax = viewport_width_  - 1;
        const int32_t ymax = viewport_height_ - 1;

        constexpr int32_t INSIDE = 0;
        constexpr int32_t LEFT   = 1;
        constexpr int32_t RIGHT  = 2;
        constexpr int32_t BOTTOM = 4;
        constexpr int32_t TOP    = 8;

        auto compute_code = [&](int32_t x, int32_t y) -> int32_t
        {
            int32_t code = INSIDE;

            if (x < 0)      code |= LEFT;
            else if (x > xmax) code |= RIGHT;

            if (y < 0)      code |= BOTTOM;
            else if (y > ymax) code |= TOP;

            return code;
        };

        int32_t x0 = (int16_t)p0.x;
        int32_t y0 = (int16_t)p0.y;
        int32_t x1 = (int16_t)p1.x;
        int32_t y1 = (int16_t)p1.y;

        int32_t code0 = compute_code(x0, y0);
        int32_t code1 = compute_code(x1, y1);

        bool accept = false;

        while (true)
        {
            if ((code0 | code1) == 0)
            {
                accept = true;
                break;
            }
            else if (code0 & code1)
            {
                break;
            }
            else
            {
                int32_t outcode = code0 ? code0 : code1;

                int32_t x, y;

                if (outcode & TOP)
                {
                    y = ymax;
                    x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                }
                else if (outcode & BOTTOM)
                {
                    y = 0;
                    x = x0 + (x1 - x0) * (0 - y0) / (y1 - y0);
                }
                else if (outcode & RIGHT)
                {
                    x = xmax;
                    y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                }
                else // LEFT
                {
                    x = 0;
                    y = y0 + (y1 - y0) * (0 - x0) / (x1 - x0);
                }

                if (outcode == code0)
                {
                    x0 = x;
                    y0 = y;
                    code0 = compute_code(x0, y0);
                }
                else
                {
                    x1 = x;
                    y1 = y;
                    code1 = compute_code(x1, y1);
                }
            }
        }

        if (!accept)
        {
            return false;
        }

        // Write back
        p0.x = (int16_t)x0;
        p0.y = (int16_t)y0;
        p1.x = (int16_t)x1;
        p1.y = (int16_t)y1;

        return true;
    }

    auto project_to_ndc_(vec3& p) -> void
    {
        //p.z.data |= 0b00000000000000000000000000000001;
        p.x = p.x * aspect_ratio_;
        p.x = p.x / p.z;
        p.y = p.y / p.z;
    }

    [[nodiscard]] auto clip_and_triangulate_ndc(
        const vec3& v0, const vec3& v1, const vec3& v2,
        std::array<vec3, 15>& out_vertices) noexcept -> int32_t
    {
        // ---- Trivial reject ------------------------------------------------
        if (v0.x < -1.0_fx && v1.x < -1.0_fx && v2.x < -1.0_fx) return 0;
        if (v0.x >  1.0_fx && v1.x >  1.0_fx && v2.x >  1.0_fx) return 0;
        if (v0.y < -1.0_fx && v1.y < -1.0_fx && v2.y < -1.0_fx) return 0;
        if (v0.y >  1.0_fx && v1.y >  1.0_fx && v2.y >  1.0_fx) return 0;

        // ---- Trivial accept ------------------------------------------------
        auto inside_all = [](const vec3& v) noexcept {
            return v.x >= -1.0_fx && v.x <= 1.0_fx
                   && v.y >= -1.0_fx && v.y <= 1.0_fx;
        };

        if (inside_all(v0) && inside_all(v1) && inside_all(v2)) {
            out_vertices[0] = v0;
            out_vertices[1] = v1;
            out_vertices[2] = v2;
            return 3;
        }

        // ---- Helpers -------------------------------------------------------
        auto lerp = [](const vec3& a, const vec3& b, fixed32 t) noexcept -> vec3 {
            return {
                a.x + t * (b.x - a.x),
                a.y + t * (b.y - a.y),
                a.z + t * (b.z - a.z)
            };
        };

        // Clips edge a->b against one half-plane, appending 0-2 vertices into
        // buf at offset n. Returns updated n.
        auto clip_edge = [&](
                             std::array<vec3, 7>& buf, std::size_t n,
                             const vec3& a, const vec3& b,
                             auto&& inside_fn, auto&& t_fn) noexcept -> std::size_t
        {
            const bool a_in = inside_fn(a);
            const bool b_in = inside_fn(b);
            if (a_in) {
                buf[n++] = a;
                if (!b_in) buf[n++] = lerp(a, b, t_fn(a, b));
            } else if (b_in) {
                buf[n++] = lerp(a, b, t_fn(a, b));
            }
            return n;
        };

        // ---- Sutherland-Hodgman: 4 planes, ping-pong two 7-element buffers --

        std::array<vec3, 7> a{}, b{};
        std::size_t na = 0, nb = 0;

        // Pass 1 — Left (x >= -1): unrolled, always exactly 3 input edges
        auto lx_in = [](const vec3& v)                { return v.x >= -1.0_fx; };
        auto lx_t  = [](const vec3& p, const vec3& q) { return (-1.0_fx - p.x) / (q.x - p.x); };
        na = clip_edge(a, 0,  v0, v1, lx_in, lx_t);
        na = clip_edge(a, na, v1, v2, lx_in, lx_t);
        na = clip_edge(a, na, v2, v0, lx_in, lx_t);
        if (na < 3) return 0;

        // Pass 2 — Right (x <= 1): up to 4 input edges
        auto rx_in = [](const vec3& v)                { return v.x <=  1.0_fx; };
        auto rx_t  = [](const vec3& p, const vec3& q) { return ( 1.0_fx - p.x) / (q.x - p.x); };
        nb = 0;
        for (std::size_t i = 0; i < na; ++i)
            nb = clip_edge(b, nb, a[i], a[i + 1 < na ? i + 1 : 0], rx_in, rx_t);
        if (nb < 3) return 0;

        // Pass 3 — Bottom (y >= -1): up to 5 input edges
        auto by_in = [](const vec3& v)                { return v.y >= -1.0_fx; };
        auto by_t  = [](const vec3& p, const vec3& q) { return (-1.0_fx - p.y) / (q.y - p.y); };
        na = 0;
        for (std::size_t i = 0; i < nb; ++i)
            na = clip_edge(a, na, b[i], b[i + 1 < nb ? i + 1 : 0], by_in, by_t);
        if (na < 3) return 0;

        // Pass 4 — Top (y <= 1): up to 6 input edges
        auto ty_in = [](const vec3& v)                { return v.y <=  1.0_fx; };
        auto ty_t  = [](const vec3& p, const vec3& q) { return ( 1.0_fx - p.y) / (q.y - p.y); };
        nb = 0;
        for (std::size_t i = 0; i < na; ++i)
            nb = clip_edge(b, nb, a[i], a[i + 1 < na ? i + 1 : 0], ty_in, ty_t);
        if (nb < 3) return 0;

        // ---- Fan triangulation ---------------------------------------------
        int out_count = 0;
        for (std::size_t i = 1; i + 1 < nb; ++i) {
            out_vertices[out_count++] = b[0];
            out_vertices[out_count++] = b[i];
            out_vertices[out_count++] = b[i + 1];
        }
        return out_count;
    }

    [[nodiscard]] auto clip_triangle_screen_space_dummy_(vec3 const & v0, vec3 const & v1, vec3 const & v2, std::array<vec3, 8>& outVerts) -> uint32_t
    {
        outVerts[0] = v0;
        outVerts[1] = v1;
        outVerts[2] = v2;
        return 3;
    }

    [[nodiscard]] auto is_front_facing(vec3 const & v0, vec3 const & v1, vec3 const & v2) -> bool
    {
        vec3 a = v1 - v0;
        vec3 b = v2 - v0;
        vec3 normal = vec3::cross(a, b);
        return normal.z >  0.0_fx;
    }

    auto to_screen_space_(vec3& p) -> void
    {
        // Map from [-1, +1] → [0, 1]
        fixed32 sx = (p.x + 1.0_fx) * 0.5_fx;
        fixed32 sy = (1.0_fx - p.y) * 0.5_fx;

        // Scale to viewport
        p.x = sx * viewport_width_fx_;
        p.y = sy * viewport_height_fx_;
    }

    VertexFunction* vf_;

    void* vertex_pointer_{nullptr};
    uint32_t vertex_size_{0};
    uint32_t vertex_stride_{0};
    uint16_t* color_pointer_{nullptr};
    uint32_t color_stride_{0};
    DrawType current_draw_type_{DrawType::Points};

    uint32_t viewport_width_{0};
    uint32_t viewport_height_{0};
    fixed32 viewport_width_fx_{0.0_fx};
    fixed32 viewport_height_fx_{0.0_fx};
    fixed32 aspect_ratio_{0.0_fx};

    std::array<vec3, MAX_VERTS> working_vertex_buffer_;
    uint32_t working_vertex_buffer_size_{0};

    std::array<uint16_t, MAX_VERTS> working_color_buffer_;
    uint32_t working_color_buffer_size_{0};

    static constexpr vec4 frustrum_[6] =
    {
        {0.0_fx,0.0_fx,1.0_fx, -0.01_fx},
        {0.707106781187_fx,0.0_fx,0.707106781187_fx, 0.0_fx},
        {-0.707106781187_fx,0.0_fx,0.707106781187_fx,0.0_fx},
        {0.0_fx,0.707106781187_fx,0.707106781187_fx,0.0_fx},
        {0.0_fx,-0.707106781187_fx,0.707106781187_fx,0.0_fx},
        {0.0_fx,0.0_fx,-1.0_fx, -1000.0_fx}
    };


};



}
