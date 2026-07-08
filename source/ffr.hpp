#pragma once

#include <cassert>
#include <cstdint>

#include "ffm.hpp"

namespace ffr
{

using namespace ffm;


enum class DrawType : uint32_t
{
    Points = 1,
    Lines = 2,
    Triangles = 3,
    TrianglesWireFrame = 4
};


template<class VERTEX_FUNCTION>
class Context
{
    static constexpr std::size_t MAX_VERTS{128};

public:
    Context() = default;
    virtual ~Context() = default;

    Context(Context&) = delete;
    auto operator = (Context&) = delete;
    Context(Context&&) = delete;
    auto operator = (Context&&) = delete;

    virtual auto plot(int16_t x, int16_t y, uint16_t color) -> void {}

    virtual auto line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) -> void
    {
        bool const steep = ffm::abs(y1 - y0) > ffm::abs(x1 - x0);

        if (steep)
        {
            int32_t tmp = x0;
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

    auto setVertexPointer(uint32_t size, uint32_t stride, void* vp) -> void
    {
        vertex_size_ = size;
        vertex_stride_ = stride;
        vertex_pointer_= vp;
    }

    //1 uint16_t per primitve
    auto setColorPointer(uint16_t stride, uint16_t* cp)-> void
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

        //fix color stride?
        current_draw_type_ = dt;
        working_vertex_buffer_size_ = 0;
        working_color_buffer_size_ = 0;

        if(vertex_size_ == 2)
        {
            for(uint32_t i = first; i < (first+count); ++i)
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
            for(uint32_t i = first; i < (first+count); ++i)
            {
                working_vertex_buffer_[working_vertex_buffer_size_] = reinterpret_cast<vec3*>(vertex_pointer_)[i];
                ++working_vertex_buffer_size_;
            }
        }

        //gather colors into working buffer
        if(color_pointer_ == nullptr)
        {
            for(uint32_t i = first; i < (first + count); ++i)
            {
                working_color_buffer_[working_color_buffer_size_] = color_stride_;
                ++working_color_buffer_size_;
            }
        }
        else
        {
            //TODO: convert all to pointer arithetic instead of indices for stride support

            for(uint32_t i = first; i < (first + count); ++i)
            {
                working_color_buffer_[working_color_buffer_size_] = color_pointer_[i];
                ++working_color_buffer_size_;
            }
        }


        //vertex_pipeline_();

        //run vertex function
        for(uint32_t i = 0; i < working_vertex_buffer_size_; ++i)
        {
            vf_(working_vertex_buffer_[i]);
        }


        uint32_t col = 0;
        for(uint32_t i = 0; i < working_vertex_buffer_size_; ++i)
        {
            uint16_t const & ccs{working_color_buffer_[col]};

            if(current_draw_type_ == DrawType::Points)
            {
                vec3& cvs = working_vertex_buffer_[i];
                project_to_ndc(cvs);
                to_screen_space(cvs);

                if(clip_point_screen_space(cvs))
                {
                    plot(static_cast<int16_t>(cvs.x), static_cast<int16_t>(cvs.y), ccs);
                }
                col = col + 1;
            }
            else if(current_draw_type_ == DrawType::Lines)
            {
                vec3& p0{working_vertex_buffer_[i]};
                vec3& p1{working_vertex_buffer_[i+1]};
                project_to_ndc(p0); project_to_ndc(p1);
                to_screen_space(p0); to_screen_space(p1);

                if(clip_line_screen_space(p0, p1))
                {
                    line(static_cast<int16_t>(p0.x), static_cast<int16_t>(p0.y),
                         static_cast<int16_t>(p1.x), static_cast<int16_t>(p1.y), ccs);
                }
                i = i + 1;;
                col = col + 2;
            }
            else if(current_draw_type_ == DrawType::Triangles)
            {
                vec3& v0{working_vertex_buffer_[i]};
                vec3& v1{working_vertex_buffer_[i+1]};
                vec3& v2{working_vertex_buffer_[i+2]};
                project_to_ndc(v0);project_to_ndc(v1);project_to_ndc(v2);

                if(is_cull_passing(v0, v1, v2))
                {
                    to_screen_space(v0);to_screen_space(v1);to_screen_space(v2);

                    std::array<vec3, 15> arr;
                    auto k = clipAndTriangulateTriangle(v0,v1,v2,arr);


                    for(auto i = 0ul; i < k; i = i + 3)
                    {
                        triangle(static_cast<int16_t>(arr[i].x), static_cast<int16_t>(arr[i].y),
                            static_cast<int16_t>(arr[i+1].x), static_cast<int16_t>(arr[i+1].y),
                            static_cast<int16_t>(arr[i+2].x), static_cast<int16_t>(arr[i+2].y), ccs);
                    }
                }




                i = i + 2;
                col = col + 3;
            }
            else if(current_draw_type_ == DrawType::TrianglesWireFrame)
            {
                vec3& v0{working_vertex_buffer_[i]};
                vec3& v1{working_vertex_buffer_[i+1]};
                vec3& v2{working_vertex_buffer_[i+2]};
                project_to_ndc(v0);project_to_ndc(v1);project_to_ndc(v2);

                if(is_cull_passing(v0, v1, v2))
                {
                    to_screen_space(v0);to_screen_space(v1);to_screen_space(v2);

                    std::array<vec3, 15> arr;
                    auto k = clipAndTriangulateTriangle(v0,v1,v2,arr);


                    for(auto i = 0ul; i < k; i = i + 3)
                    {
                        line( static_cast<int16_t>(arr[i].x), static_cast<int16_t>(arr[i].y),
                            static_cast<int16_t>(arr[i+1].x), static_cast<int16_t>(arr[i+1].y), ccs );

                        line( static_cast<int16_t>(arr[i+1].x), static_cast<int16_t>(arr[i+1].y),
                             static_cast<int16_t>(arr[i+2].x), static_cast<int16_t>(arr[i+2].y), ccs );

                        line( static_cast<int16_t>(arr[i+2].x), static_cast<int16_t>(arr[i+2].y),
                            static_cast<int16_t>(arr[i].x), static_cast<int16_t>(arr[i].y), ccs);
                    }
                }




                i = i + 2;
                col = col + 3;
            }
        }

    }

    auto getVertexFunction() -> VERTEX_FUNCTION&
    {
        return vf_;
    }

    auto setFaceCulling(int32_t mode) -> void
    {
        cull_ = mode;
    }



private:

    auto clipAndTriangulateTriangle(
        const vec3& A,
        const vec3& B,
        const vec3& C,
        std::array<vec3,15>& outVerts) -> int
    {
        const auto left   = 0.0_fx;
        const auto right  = viewport_width_fx_;
        const auto bottom = 0.0_fx;
        const auto top    = viewport_height_fx_;

        std::array<vec3,7> poly{};
        std::array<vec3,7> temp{};
        int polyCount = 3;

        // Input (no asserts)
        poly[0] = A;
        poly[1] = B;
        poly[2] = C;

        auto inside = [&](const vec2& p, int edge) {
            switch (edge) {
            case 0: return p.x >= left;   // LEFT
            case 1: return p.x <= right;  // RIGHT
            case 2: return p.y >= bottom; // BOTTOM
            case 3: return p.y <= top;    // TOP
            }
            return false;
        };

        auto intersect = [&](const vec3& P, const vec3& Q, int edge) {
            vec2 p(P.x, P.y);
            vec2 q(Q.x, Q.y);
            vec2 d = q - p;

            auto t = 0.0_fx;

            switch (edge) {
            case 0: t = (left   - p.x) / d.x; break;
            case 1: t = (right  - p.x) / d.x; break;
            case 2: t = (bottom - p.y) / d.y; break;
            case 3: t = (top    - p.y) / d.y; break;
            }

            vec2 xy = p + d * t;
            auto z = P.z + (Q.z - P.z) * t;

            return vec3{xy.x, xy.y, z};
        };

        // ------------------------------
        // Sutherland–Hodgman clipping
        // ------------------------------
        for (int edge = 0; edge < 4; ++edge) {
            if (polyCount == 0)
                return 0;

            int tempCount = 0;

            for (int i = 0; i < polyCount; ++i) {
                vec3 P = poly[i];
                vec3 Q = poly[(i + 1) % polyCount];

                vec2 p2(P.x, P.y);
                vec2 q2(Q.x, Q.y);

                bool Pin = inside(p2, edge);
                bool Qin = inside(q2, edge);

                if (Pin && Qin) {
                    temp[tempCount++] = Q;
                }
                else if (Pin && !Qin) {
                    temp[tempCount++] = intersect(P, Q, edge);
                }
                else if (!Pin && Qin) {
                    temp[tempCount++] = intersect(P, Q, edge);
                    temp[tempCount++] = Q;
                }
            }

            polyCount = tempCount;
            for (int i = 0; i < tempCount; ++i)
                poly[i] = temp[i];
        }

        if (polyCount < 3)
            return 0;

        // ------------------------------
        // Triangulation
        // ------------------------------
        int outCount = 0;

        if (polyCount == 3) {
            outVerts[0] = poly[0];
            outVerts[1] = poly[1];
            outVerts[2] = poly[2];
            outCount = 3;
        }
        else {
            for (int i = 1; i < polyCount - 1; ++i) {
                outVerts[outCount++] =
                    poly[0];
                outVerts[outCount++] = poly[i];
                outVerts[outCount++] = poly[i + 1];
            }
        }

        // ------------------------------
        // FINAL CLAMP (guarantees safety)
        // ------------------------------
        for (int i = 0; i < outCount; ++i) {
            auto &v = outVerts[i];

            if (v.x < left)   v.x = left;
            if (v.y < bottom) v.y = bottom;
            if (v.x > right)  v.x = right;
            if (v.y > top)    v.y = top;
        }

        // ------------------------------
        // FINAL ASSERTS (only here)
        // ------------------------------
        for (int i = 0; i < outCount; ++i) {
            assert(outVerts[i].x >= 0.0_fx);
            assert(outVerts[i].y >= 0.0_fx);
            assert(outVerts[i].x <= viewport_width_fx_);
            assert(outVerts[i].y <= viewport_height_fx_);
        }

        return outCount;
    }

    [[nodiscard]] auto clip_point_screen_space(vec3 const & p) -> bool
    {
        return !(p.x < 0.0_fx || p.x >= viewport_width_fx_ ||
                p.y < 0.0_fx || p.y >= viewport_height_fx_ );
    }

    [[nodiscard]] auto clip_line_screen_space(vec3 &p0, vec3 &p1) -> bool
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

    auto project_to_ndc(vec3& p) -> void
    {
        //p.z.data |= 0b00000000000000000000000000000001;
        p.x = p.x * aspect_ratio_;
        p.x = p.x / p.z;
        p.y = p.y / p.z;
    }

    [[nodiscard]] auto is_cull_passing(vec3 const & v0, vec3 const & v1, vec3 const & v2) -> bool
    {
        vec3 a = v1 - v0;
        vec3 b = v2 - v0;
        vec3 normal = vec3::cross(a, b);

        if(cull_ == 1) [[likely]]  { return normal.z >  0.0_fx; }
        else if(cull_ == 0) { return true; }
        else { return normal.z <  0.0_fx; }

    }

    auto to_screen_space(vec3& p) -> void
    {
        // Map from [-1, +1] → [0, 1]
        fixed32 sx = (p.x + 1.0_fx) * 0.5_fx;
        fixed32 sy = (1.0_fx - p.y) * 0.5_fx;

        // Scale to viewport
        p.x = sx * viewport_width_fx_;
        p.y = sy * viewport_height_fx_;
    }

    VERTEX_FUNCTION vf_;

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

    int32_t cull_ = 1;

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
