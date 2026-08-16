#pragma once

#include <cassert>
#include <cstdint>
#include <inplace_vector>

#include "ffm.hpp"

namespace ffr
{

using namespace ffm;


enum class DrawType : uint32_t
{
    Points = 1,
    Lines,
    Triangles,
    TrianglesWireFrame,
    Quads,
    QuadsWireFra
};

enum class FaceCullMode : int32_t
{
    Back = -1,
    None = 0,
    Front = 1,
    All = 2
};


template<class VERTEX_FUNCTION>
class Context
{
    static constexpr size_t MAX_VERTS{128};

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

    virtual auto quad(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color) -> void
    {

        const int xs[4] = {x0, x1, x2, x3};
        const int ys[4] = {y0, y1, y2, y3};

        // 1. Find Top Vertex (min Y)
        int top = 0;
        for (int i = 1; i < 4; ++i) {
            if (ys[i] < ys[top]) top = i;
        }

        // 2. Setup Edge Walkers (Left=Backward, Right=Forward)
        // Left Chain Indices: top -> top-1 -> top-2
        int l_curr = top;
        int l_next = (top - 1 + 4) % 4;
        // Right Chain Indices: top -> top+1 -> top+2
        int r_curr = top;
        int r_next = (top + 1) % 4;

        // Edge State Variables (Left)
        int lx = xs[l_curr], ly = ys[l_curr];
        int ldx = xs[l_next] - lx, ldy = ys[l_next] - ly;
        int l_err = (ldy > 0) ? ldy / 2 : 0;
        int l_x_inc = (ldx >= 0) ? 1 : -1;
        int l_num = ffm::abs(ldx), l_den = ffm::abs(ldy);
        int l_y_max = ys[l_next];

        // Edge State Variables (Right)
        int rx = xs[r_curr], ry = ys[r_curr];
        int rdx = xs[r_next] - rx, rdy = ys[r_next] - ry;
        int r_err = (rdy > 0) ? rdy / 2 : 0;
        int r_x_inc = (rdx >= 0) ? 1 : -1;
        int r_num = ffm::abs(rdx), r_den = ffm::abs(rdy);
        int r_y_max = ys[r_next];

        // Skip horizontal start edges if any
        while (l_den == 0) {
            l_curr = l_next; l_next = (l_curr - 1 + 4) % 4;
            lx = xs[l_curr]; ly = ys[l_curr];
            ldx = xs[l_next] - lx; ldy = ys[l_next] - ly;
            l_err = (ldy > 0) ? ldy / 2 : 0; l_x_inc = (ldx >= 0) ? 1 : -1;
            l_num = ffm::abs(ldx); l_den = ffm::abs(ldy); l_y_max = ys[l_next];
        }
        while (r_den == 0) {
            r_curr = r_next; r_next = (r_curr + 1) % 4;
            rx = xs[r_curr]; ry = ys[r_curr];
            rdx = xs[r_next] - rx; rdy = ys[r_next] - ry;
            r_err = (rdy > 0) ? rdy / 2 : 0; r_x_inc = (rdx >= 0) ? 1 : -1;
            r_num = ffm::abs(rdx); r_den = ffm::abs(rdy); r_y_max = ys[r_next];
        }

        int scan_y = ffm::max(ly, ry);
        int end_y = ffm::max(ys[0], ffm::max(ys[1], ffm::max(ys[2], ys[3])));

        // 3. Scanline Loop
        while (scan_y <= end_y) {
            // Draw span
            if (lx <= rx) lineHorizontal(lx, scan_y, rx, color);
            else lineHorizontal(rx, scan_y, lx, color);

            scan_y++;

            // Step Left Edge
            if (ly < l_y_max) {
                l_err += l_num;
                if (l_err >= l_den) { lx += l_x_inc; l_err -= l_den; }
                ly++;
            } else {
                // Switch to next segment on Left
                l_curr = l_next; l_next = (l_curr - 1 + 4) % 4;
                lx = xs[l_curr]; ly = ys[l_curr];
                ldx = xs[l_next] - lx; ldy = ys[l_next] - ly;
                l_err = (ldy > 0) ? ldy / 2 : 0; l_x_inc = (ldx >= 0) ? 1 : -1;
                l_num = ffm::abs(ldx); l_den = ffm::abs(ldy); l_y_max = ys[l_next];
                // Fast forward if needed (shouldn't be for convex)
                while (ly < scan_y && l_den > 0) {
                    l_err += l_num; if (l_err >= l_den) { lx += l_x_inc; l_err -= l_den; }
                    ly++;
                }
            }

            // Step Right Edge
            if (ry < r_y_max) {
                r_err += r_num;
                if (r_err >= r_den) { rx += r_x_inc; r_err -= r_den; }
                ry++;
            } else {
                // Switch to next segment on Right
                r_curr = r_next; r_next = (r_curr + 1) % 4;
                rx = xs[r_curr]; ry = ys[r_curr];
                rdx = xs[r_next] - rx; rdy = ys[r_next] - ry;
                r_err = (rdy > 0) ? rdy / 2 : 0; r_x_inc = (rdx >= 0) ? 1 : -1;
                r_num = ffm::abs(rdx); r_den = ffm::abs(rdy); r_y_max = ys[r_next];
                while (ry < scan_y && r_den > 0) {
                    r_err += r_num; if (r_err >= r_den) { rx += r_x_inc; r_err -= r_den; }
                    ry++;
                }
            }

            // Termination check
            if ((l_den == 0 || ly >= l_y_max) && (r_den == 0 || ry >= r_y_max)) {
                // Check if both are at the very bottom vertex
                if (ly >= end_y && ry >= end_y) break;
            }
        }

    }

    virtual auto clear() -> void {}

    virtual auto present() -> void {}

    auto setVertexPointer(uint32_t const size, uint32_t const stride, void const* vp) -> void
    {
        vertex_size_ = size;
        vertex_stride_ = stride;
        vertex_pointer_= vp;
    }

    auto setColorPointer(uint16_t const stride, void const* cp)-> void
    {
        color_stride_ = stride;
        color_pointer_ = cp;
    }

    auto setViewPort(int16_t const w, int16_t const h) -> void
    {
        viewport_width_ = w;
        viewport_height_ = h;
        viewport_width_fx_ = static_cast<int16_t>(w);
        viewport_height_fx_ = static_cast<int16_t>(h);
        aspect_ratio_ = 1.0_fx / (viewport_width_fx_ / viewport_height_fx_);
        frustrum_ = ComputeFrustumPlanesAsVec4();
    }

    auto drawArray(DrawType const dt, uint32_t const first, uint32_t const count) -> void
    {
        current_draw_type_ = dt;
        working_vertex_buffer_.clear();
        working_color_buffer_.clear();

        std::byte const * vp = reinterpret_cast<std::byte const *>(vertex_pointer_);
        std::byte const * cp = reinterpret_cast<std::byte const *>(color_pointer_);
        auto vs = vertex_stride_;
        auto cs = color_stride_;

        if(vertex_size_ == 2)
        {
            if(vs == 0) {vs = sizeof(vec2);}
            for(auto const * p = vp + (first*vs); p < vp + ((first+count)*vs); p = p + vs)
            {
                working_vertex_buffer_.push_back( {reinterpret_cast<vec2 const*>(p)->x,reinterpret_cast<vec2 const*>(p)->y,0.0_fx} );
            }

        }
        else if(vertex_size_ == 3)
        {
            if(vs == 0) {vs = sizeof(vec3);}
            for(auto const * p = vp + (first*vs); p < vp + ((first+count)*vs); p = p + vs)
            {
                working_vertex_buffer_.push_back( {reinterpret_cast<vec3 const*>(p)[0]} );
            }

        }

        //gather colors into working buffer
        if(color_pointer_ == nullptr)
        {
            for(auto i = 0; i < working_vertex_buffer_.size(); ++i)
            {
                working_color_buffer_.push_back(color_stride_);
            }
        }
        else
        {
            if(color_stride_ == 0) {cs = sizeof(uint16_t);}
            for(auto const * p = cp + (first*cs); p < cp + ((first+count)*cs); p = p + cs)
            {
                working_color_buffer_.push_back(reinterpret_cast<uint16_t const*>(p)[0]);
            }

        }


        //vertex_pipeline_();

        //run vertex function
        for(uint32_t i = 0; i < working_vertex_buffer_.size(); ++i)
        {
            vf_(working_vertex_buffer_[i]);
        }


        size_t col = 0;
        for(uint32_t i = 0; i < working_vertex_buffer_.size(); ++i)
        {
            uint16_t const & ccs{working_color_buffer_[col]};

            if(current_draw_type_ == DrawType::Points)
            {
                vec3& cvs = working_vertex_buffer_[i];

                col = col + 1;
            }
            else if(current_draw_type_ == DrawType::Lines)
            {
                vec3& p0{working_vertex_buffer_[i]};
                vec3& p1{working_vertex_buffer_[i+1]};

                i = i + 1;;
                col = col + 2;
            }
            else if(current_draw_type_ == DrawType::Triangles)
            {
                vec3& v0{working_vertex_buffer_[i]};
                vec3& v1{working_vertex_buffer_[i+1]};
                vec3& v2{working_vertex_buffer_[i+2]};

                std::array<vec3, 3*(3+4)> vertArr;
                auto vertCount = ClipAndTriangulateConvexPolygon<3>({v0,v1,v2}, vertArr);
                for(auto j = 0; j < vertCount; j = j + 3)
                {
                    project_to_ndc(vertArr[j]);project_to_ndc(vertArr[j+1]);project_to_ndc(vertArr[j+2]);
                    if(is_cull_passing(vertArr[j],vertArr[j+1],vertArr[j+2]))
                    {
                        to_screen_space(vertArr[j]);to_screen_space(vertArr[j+1]);to_screen_space(vertArr[j+2]);
                        triangle( vertArr[j].x,vertArr[j].y, vertArr[j+1].x,vertArr[j+1].y, vertArr[j+2].x,vertArr[j+2].y, ccs );
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



                i = i + 2;
                col = col + 3;
            }
        }

    }

    [[nodiscard]] auto getVertexFunction() -> VERTEX_FUNCTION&
    {
        return vf_;
    }

    auto setFaceCulling(int32_t mode) -> void
    {
        cull_ = mode;
    }



private:

    template <std::size_t N>
    std::size_t ClipAndTriangulateConvexPolygon( const std::array<vec3, N>& polygon, std::array<vec3, 3 * (N + 4)>& outVerts) const
    {
        // A polygon can gain at most one vertex per clipping plane.
        std::array<vec3, N + 6> current{};
        std::array<vec3, N + 6> next{};

        std::size_t currentCount = N;

        for (std::size_t i = 0; i < N; ++i)
            current[i] = polygon[i];

        auto clipPlane = [&](const vec4& plane)
        {
            std::size_t nextCount = 0;

            for (std::size_t i = 0; i < currentCount; ++i)
            {
                const vec3& curr = current[i];
                const vec3& prev = current[(i + currentCount - 1) % currentCount];

                const fixed32 dCurr =
                    plane.x * curr.x +
                    plane.y * curr.y +
                    plane.z * curr.z +
                    plane.w;

                const fixed32 dPrev =
                    plane.x * prev.x +
                    plane.y * prev.y +
                    plane.z * prev.z +
                    plane.w;

                const bool currIn = dCurr >= 0.0_fx;
                const bool prevIn = dPrev >= 0.0_fx;

                if (currIn != prevIn)
                {
                    const fixed32 t = dPrev / (dPrev - dCurr);
                    next[nextCount++] = prev + (curr - prev) * t;
                }

                if (currIn)
                    next[nextCount++] = curr;
            }

            current = next;
            currentCount = nextCount;
        };

        // Clip against all six global frustum planes.
        for (const vec4& plane : frustrum_)
        {
            clipPlane(plane);

            if (currentCount == 0)
                return 0;
        }

        // --- Triangulate clipped polygon ---
        if (currentCount < 3)
            return 0;

        std::size_t outCount = 0;
        const vec3& anchor = current[0];

        for (std::size_t i = 1; i + 1 < currentCount; ++i)
        {
            outVerts[outCount++] = anchor;
            outVerts[outCount++] = current[i];
            outVerts[outCount++] = current[i + 1];
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

        if(cull_ == 1) [[likely]]  { return normal.z > 0.0_fx; }
        else if(cull_ == 0) { return true; }
        else { return normal.z <  0.0_fx; }

    }

    auto to_screen_space(vec3& p) -> void
    {
        // Map from [-1, +1] → [0, 1]
        fixed32 sx = (p.x + 1.0_fx).halved();
        fixed32 sy = (1.0_fx - p.y).halved();

        // Scale to viewport
        p.x = sx * viewport_width_fx_ - 1.0_fx;
        p.y = sy * viewport_height_fx_ - 1.0_fx;
    }


    constexpr std::array<vec4, 6> ComputeFrustumPlanesAsVec4(
        fixed32 nearZ = 0.001_fx,
        fixed32 farZ  = 1000.0_fx)
    {
        std::array<vec4, 6> planes{};

        // 90° vertical FOV:
        //
        // tan(FOV_Y / 2) = tan(45°) = 1
        //
        // Therefore:
        //
        //   x = +/- z * aspect_ratio_
        //   y = +/- z
        //
        // Plane normalization is unnecessary for clipping; only the
        // sign of the plane equation matters.

        // LEFT:   x >= -z * aspect
        //         aspect*x + z >= 0
        planes[0] = vec4{
            aspect_ratio_,
            0.0_fx,
            1.0_fx,
            0.0_fx
        };

        // RIGHT:  x <= z * aspect
        //         -aspect*x + z >= 0
        planes[1] = vec4{
            -aspect_ratio_,
            0.0_fx,
            1.0_fx,
            0.0_fx
        };

        // BOTTOM: y >= -z
        //         y + z >= 0
        planes[2] = vec4{
            0.0_fx,
            1.0_fx,
            1.0_fx,
            0.0_fx
        };

        // TOP:    y <= z
        //         -y + z >= 0
        planes[3] = vec4{
            0.0_fx,
            -1.0_fx,
            1.0_fx,
            0.0_fx
        };

        // NEAR:   z >= nearZ
        planes[4] = vec4{
            0.0_fx,
            0.0_fx,
            1.0_fx,
            -nearZ
        };

        // FAR:    z <= farZ
        planes[5] = vec4{
            0.0_fx,
            0.0_fx,
            -1.0_fx,
            farZ
        };

        return planes;
    }

    VERTEX_FUNCTION vf_;

    void const* vertex_pointer_{nullptr};
    uint32_t vertex_size_{0};
    uint32_t vertex_stride_{0};
    void const* color_pointer_{nullptr};
    uint16_t color_stride_{0};
    DrawType current_draw_type_{DrawType::Points};

    uint32_t viewport_width_{0};
    uint32_t viewport_height_{0};
    fixed32 viewport_width_fx_{0.0_fx};
    fixed32 viewport_height_fx_{0.0_fx};
    fixed32 aspect_ratio_{0.0_fx};

    std::inplace_vector<vec3,MAX_VERTS> working_vertex_buffer_;
    std::inplace_vector<uint16_t,MAX_VERTS> working_color_buffer_;

    int32_t cull_{1};

    std::array<vec4, 6> frustrum_;



};



}
