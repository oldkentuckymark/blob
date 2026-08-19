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
        triangle(x0,y0,x1,y1,x2,y2,color);
        triangle(x2,y2,x3,y3,x0,y0,color);
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
        viewport_width_fx_ = static_cast<fixed32>(w);
        viewport_height_fx_ = static_cast<fixed32>(h);
        aspect_ratio_ = 1.0_fx / (viewport_width_fx_ / viewport_height_fx_);
        //frustrum_ = ComputeFrustumPlanesAsVec4();
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

                if(clip_point_near_(cvs))
                {
                    project_to_ndc(cvs);
                    if(clip_point_ndc_space_(cvs))
                    {
                        to_screen_space(cvs);
                        plot(cvs.x,cvs.y,ccs);
                    }
                }

                col = col + 1;
            }
            else if(current_draw_type_ == DrawType::Lines)
            {
                vec3& p0{working_vertex_buffer_[i]};
                vec3& p1{working_vertex_buffer_[i+1]};

                if(clip_line_near_(p0,p1))
                {
                    project_to_ndc(p0);project_to_ndc(p1);
                    if(clip_line_ndc_space_(p0,p1))
                    {
                        to_screen_space(p0);to_screen_space(p1);
                        line(p0.x,p0.y,p1.x,p1.y,ccs);
                    }
                }

                i = i + 1;;
                col = col + 2;
            }
            else if(current_draw_type_ == DrawType::Triangles)
            {
                vec3& v0{working_vertex_buffer_[i]};
                vec3& v1{working_vertex_buffer_[i+1]};
                vec3& v2{working_vertex_buffer_[i+2]};

                std::array<vec3,6> outVerts;
                auto numverts = clip_triangle_near_({v0,v1,v2},outVerts);
                for(auto j = 0ul; j < numverts; j = j + 3)
                {
                    project_to_ndc(outVerts[j+0]);project_to_ndc(outVerts[j+1]);project_to_ndc(outVerts[j+2]);
                    if(is_cull_passing(outVerts[j+0],outVerts[j+1],outVerts[j+2]))
                    {
                    std::array<vec3,12> outVerts2;
                    auto k = clip_triangle_ndc_space_({outVerts[j+0],outVerts[j+1],outVerts[j+2]}, outVerts2);
                    for(auto l = 0ul; l < k; l = l + 3)
                    {
                        to_screen_space(outVerts2[l+0]);to_screen_space(outVerts2[l+1]);to_screen_space(outVerts2[l+2]);
                        triangle( outVerts2[l+0].x,outVerts2[l+0].y, outVerts2[l+1].x,outVerts2[l+1].y, outVerts2[l+2].x,outVerts2[l+2].y, ccs );
                    }
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
            else if(current_draw_type_ == DrawType::Quads)
            {
                vec3& v0{working_vertex_buffer_[i]};
                vec3& v1{working_vertex_buffer_[i+1]};
                vec3& v2{working_vertex_buffer_[i+2]};
                vec3& v3{working_vertex_buffer_[i+3]};

                std::array<vec3,8> outVerts;
                auto numverts = clip_quad_near_({v0,v1,v2,v3},outVerts);
                for(auto j = 0ul; j < numverts; j = j + 4)
                {
                    project_to_ndc(outVerts[j]);project_to_ndc(outVerts[j+1]);project_to_ndc(outVerts[j+2]);project_to_ndc(outVerts[j+3]);
                    if(is_cull_passing(outVerts[j], outVerts[j+1], outVerts[j+2]))
                    {
                        std::array<vec3,12> outVerts2;
                        auto k = clip_quad_ndc_space_({outVerts[j+0],outVerts[j+1],outVerts[j+2],outVerts[j+3]}, outVerts2);
                        for(auto l = 0ul; l < k; l = l + 4)
                        {
                            to_screen_space(outVerts2[l+0]);to_screen_space(outVerts2[l+1]);to_screen_space(outVerts2[l+2]);to_screen_space(outVerts2[l+3]);
                            quad( outVerts2[l+0].x,outVerts2[l+0].y, outVerts2[l+1].x,outVerts2[l+1].y, outVerts2[l+2].x,outVerts2[l+2].y, outVerts2[l+3].x,outVerts2[l+3].y, ccs );
                        }
                    }
                }


                i = i + 3;
                col = col + 4;
            }
        }

    }

    [[nodiscard]] auto getVertexFunction() -> VERTEX_FUNCTION&
    {
        return vf_;
    }

    auto setFaceCulling(FaceCullMode const mode) -> void
    {
        cull_ = mode;
    }

    auto setNearZ(fixed32 const z) -> void
    {
        near_z_ = z;
    }


private:

    [[nodiscard]] auto clip_point_near_(vec3 const & p) const -> bool
    {
        return p.z > 0.0_fx;
    }

    [[nodiscard]] auto clip_line_near_(vec3& p0, vec3& p1) const -> bool
    {
        const auto z0 = p0.z;
        const auto z1 = p1.z;

        if (z0 <= near_z_ && z1 <= near_z_)
            return false;

        if (z0 >= near_z_ && z1 >= near_z_)
            return true;

        const auto t = (near_z_ - z0) / (z1 - z0);

        if (z0 < near_z_)
        {
            p0.x = p0.x + (p1.x - p0.x) * t;
            p0.y = p0.y + (p1.y - p0.y) * t;
            p0.z = near_z_;
        }
        else
        {
            p1.x = p1.x + (p1.x - p0.x) * t;
            p1.y = p1.y + (p1.y - p0.y) * t;
            p1.z = near_z_;
        }

        return true;
    }

    [[nodiscard]] auto clip_triangle_near_(std::array<vec3, 3> const & triangle, std::array<vec3, 6>& outVerts) const -> size_t
    {
        const auto inside = [this](const vec3& p) -> bool
        {
            return p.z >= near_z_;
        };

        const auto in0 = inside(triangle[0]);
        const auto in1 = inside(triangle[1]);
        const auto in2 = inside(triangle[2]);

        // Trivial reject: all vertices are behind the near plane.
        if (!in0 && !in1 && !in2)
            return 0;

        // Trivial accept: all vertices are in front of the near plane.
        if (in0 && in1 && in2)
        {
            outVerts[0] = triangle[0];
            outVerts[1] = triangle[1];
            outVerts[2] = triangle[2];

            return 3;
        }

        const auto intersect = [this](const vec3& a, const vec3& b) -> vec3
        {
            const auto t = (near_z_ - a.z) / (b.z - a.z);

            vec3 p = a + (b - a) * t;
            p.z = near_z_;

            return p;
        };

        std::array<vec3, 6> clipped{};
        size_t count = 0;

        auto previous = triangle[2];
        auto previous_inside = inside(previous);

        for (const auto& current : triangle)
        {
            const auto current_inside = inside(current);

            if (current_inside != previous_inside)
                clipped[count++] = intersect(previous, current);

            if (current_inside)
                clipped[count++] = current;

            previous = current;
            previous_inside = current_inside;
        }

        if (count == 3)
        {
            outVerts[0] = clipped[0];
            outVerts[1] = clipped[1];
            outVerts[2] = clipped[2];

            return 3;
        }

        // Clipped triangle is a quad -> two triangles.
        outVerts[0] = clipped[0];
        outVerts[1] = clipped[1];
        outVerts[2] = clipped[2];

        outVerts[3] = clipped[0];
        outVerts[4] = clipped[2];
        outVerts[5] = clipped[3];

        return 6;
    }

    [[nodiscard]] auto clip_quad_near_(std::array<vec3, 4> const & quad, std::array<vec3, 8>& outVerts) const -> size_t
    {
        const auto inside = [this](const vec3& p) -> bool
        {
            return p.z >= near_z_;
        };

        const auto in0 = inside(quad[0]);
        const auto in1 = inside(quad[1]);
        const auto in2 = inside(quad[2]);
        const auto in3 = inside(quad[3]);

        // Trivial reject.
        if (!in0 && !in1 && !in2 && !in3)
            return 0;

        // Trivial accept.
        if (in0 && in1 && in2 && in3)
        {
            outVerts[0] = quad[0];
            outVerts[1] = quad[1];
            outVerts[2] = quad[2];
            outVerts[3] = quad[3];

            return 4;
        }

        const auto intersect = [this](const vec3& a, const vec3& b) -> vec3
        {
            const auto t = (near_z_ - a.z) / (b.z - a.z);

            vec3 p = a + (b - a) * t;
            p.z = near_z_;

            return p;
        };

        std::array<vec3, 5> clipped{};

        size_t count = 0;

        auto previous = quad[3];
        auto previous_inside = inside(previous);

        for (const auto& current : quad)
        {
            const auto current_inside = inside(current);

            if (current_inside != previous_inside)
                clipped[count++] = intersect(previous, current);

            if (current_inside)
                clipped[count++] = current;

            previous = current;
            previous_inside = current_inside;
        }

        if (count < 3)
            return 0;

        // Triangle: use one degenerate quad.
        if (count == 3)
        {
            outVerts[0] = clipped[0];
            outVerts[1] = clipped[1];
            outVerts[2] = clipped[2];
            outVerts[3] = clipped[2];

            return 4;
        }

        // Quad: no degeneracy required.
        if (count == 4)
        {
            outVerts[0] = clipped[0];
            outVerts[1] = clipped[1];
            outVerts[2] = clipped[2];
            outVerts[3] = clipped[3];

            return 4;
        }

        // Pentagon: one real quad followed by one degenerate quad.
        outVerts[0] = clipped[0];
        outVerts[1] = clipped[1];
        outVerts[2] = clipped[2];
        outVerts[3] = clipped[3];

        outVerts[4] = clipped[0];
        outVerts[5] = clipped[3];
        outVerts[6] = clipped[4];
        outVerts[7] = clipped[4];

        return 8;
    }

    [[nodiscard]] auto clip_point_ndc_space_(vec3 const & p) -> bool
    {
        return !(p.x < -1.0_fx || p.x >= 1.0_fx ||
                 p.y < -1.0_fx || p.y >= 1.0_fx );
    }

    [[nodiscard]] auto clip_point_screen_space_(vec3 const & p) -> bool
    {
        return !(p.x < 0.0_fx || p.x >= viewport_width_fx_ ||
                 p.y < 0.0_fx || p.y >= viewport_height_fx_ );
    }

    [[nodiscard]] auto clip_line_ndc_space_(vec3& p0, vec3& p1) -> bool
    {
        constexpr auto left   = 1u << 0;
        constexpr auto right  = 1u << 1;
        constexpr auto bottom = 1u << 2;
        constexpr auto top    = 1u << 3;

        const auto out_code = [](const vec3& p) {
            auto code = 0u;

            if (p.x < -1.0_fx) code |= left;
            if (p.x >  1.0_fx) code |= right;
            if (p.y < -1.0_fx) code |= bottom;
            if (p.y >  1.0_fx) code |= top;

            return code;
        };

        auto c0 = out_code(p0);
        auto c1 = out_code(p1);

        for (;;)
        {
            if ((c0 | c1) == 0)
                return true;

            if ((c0 & c1) != 0)
                return false;

            const auto code = c0 != 0 ? c0 : c1;

            const auto dx = p1.x - p0.x;
            const auto dy = p1.y - p0.y;

            vec3 p{};

            if (code & top)
            {
                p.x = p0.x + dx * (1.0_fx - p0.y) / dy;
                p.y = 1.0_fx;
            }
            else if (code & bottom)
            {
                p.x = p0.x + dx * (-1.0_fx - p0.y) / dy;
                p.y = -1.0_fx;
            }
            else if (code & right)
            {
                p.y = p0.y + dy * (1.0_fx - p0.x) / dx;
                p.x = 1.0_fx;
            }
            else
            {
                p.y = p0.y + dy * (-1.0_fx - p0.x) / dx;
                p.x = -1.0_fx;
            }

            p.z = code == c0 ? p0.z : p1.z;

            if (code == c0)
            {
                p0 = p;
                c0 = out_code(p0);
            }
            else
            {
                p1 = p;
                c1 = out_code(p1);
            }
        }
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

    [[nodiscard]] auto clip_triangle_ndc_space_(std::array<vec3, 3> const& triangle, std::array<vec3, 12>& outVerts) -> size_t
    {
        enum class plane
        {
            left,
            right,
            bottom,
            top
        };

        constexpr std::array planes{
            plane::left,
            plane::right,
            plane::bottom,
            plane::top,
        };

        const auto inside = [](vec3 const& p, plane clip_plane) -> bool
        {
            switch (clip_plane)
            {
            case plane::left:
                return p.x >= -1.0_fx;

            case plane::right:
                return p.x <= 1.0_fx;

            case plane::bottom:
                return p.y >= -1.0_fx;

            case plane::top:
                return p.y <= 1.0_fx;
            }

            return false;
        };

        const auto intersect = [](vec3 const& a, vec3 const& b, plane clip_plane) -> vec3
        {
            vec3 result = a;

            switch (clip_plane)
            {
            case plane::left:
            {
                const auto t = (-1.0_fx - a.x) / (b.x - a.x);

                result.x = -1.0_fx;
                result.y = a.y + (b.y - a.y) * t;
                result.z = a.z + (b.z - a.z) * t;
                break;
            }

            case plane::right:
            {
                const auto t = (1.0_fx - a.x) / (b.x - a.x);

                result.x = 1.0_fx;
                result.y = a.y + (b.y - a.y) * t;
                result.z = a.z + (b.z - a.z) * t;
                break;
            }

            case plane::bottom:
            {
                const auto t = (-1.0_fx - a.y) / (b.y - a.y);

                result.x = a.x + (b.x - a.x) * t;
                result.y = -1.0_fx;
                result.z = a.z + (b.z - a.z) * t;
                break;
            }

            case plane::top:
            {
                const auto t = (1.0_fx - a.y) / (b.y - a.y);

                result.x = a.x + (b.x - a.x) * t;
                result.y = 1.0_fx;
                result.z = a.z + (b.z - a.z) * t;
                break;
            }
            }

            return result;
        };

        std::array<vec3, 6> polygon_a{
            triangle[0],
            triangle[1],
            triangle[2],
        };

        std::array<vec3, 6> polygon_b{};

        size_t count = 3;

        for (const auto clip_plane : planes)
        {
            size_t inside_count = 0;

            for (size_t i = 0; i < count; ++i)
                inside_count += inside(polygon_a[i], clip_plane);

            // Trivial reject.
            if (inside_count == 0)
                return 0;

            // Trivial accept.
            if (inside_count == count)
                continue;

            size_t out_count = 0;

            auto previous = polygon_a[count - 1];
            auto previous_inside = inside(previous, clip_plane);

            for (size_t i = 0; i < count; ++i)
            {
                const auto current = polygon_a[i];
                const auto current_inside = inside(current, clip_plane);

                if (current_inside != previous_inside)
                    polygon_b[out_count++] =
                        intersect(previous, current, clip_plane);

                if (current_inside)
                    polygon_b[out_count++] = current;

                previous = current;
                previous_inside = current_inside;
            }

            count = out_count;
            std::swap(polygon_a, polygon_b);
        }

        // Triangulate the resulting convex polygon as a triangle fan.
        for (size_t i = 1; i < count - 1; ++i)
        {
            const auto out = (i - 1) * 3;

            outVerts[out + 0] = polygon_a[0];
            outVerts[out + 1] = polygon_a[i];
            outVerts[out + 2] = polygon_a[i + 1];
        }

        return (count - 2) * 3;
    }

    [[nodiscard]] auto clip_quad_ndc_space_(std::array<vec3, 4> const& quad, std::array<vec3, 12>& outVerts) -> size_t
    {
        enum class plane
        {
            left,
            right,
            bottom,
            top
        };

        constexpr std::array planes{
            plane::left,
            plane::right,
            plane::bottom,
            plane::top,
        };

        const auto inside = [](vec3 const& p, plane clip_plane) -> bool
        {
            switch (clip_plane)
            {
            case plane::left:   return p.x >= -1.0_fx;
            case plane::right:  return p.x <=  1.0_fx;
            case plane::bottom: return p.y >= -1.0_fx;
            case plane::top:    return p.y <=  1.0_fx;
            }

            return false;
        };

        const auto intersect = [](vec3 const& a, vec3 const& b, plane clip_plane) -> vec3
        {
            switch (clip_plane)
            {
            case plane::left:
            {
                const auto t = (-1.0_fx - a.x) / (b.x - a.x);
                auto p = a + (b - a) * t;
                p.x = -1.0_fx;
                return p;
            }

            case plane::right:
            {
                const auto t = (1.0_fx - a.x) / (b.x - a.x);
                auto p = a + (b - a) * t;
                p.x = 1.0_fx;
                return p;
            }

            case plane::bottom:
            {
                const auto t = (-1.0_fx - a.y) / (b.y - a.y);
                auto p = a + (b - a) * t;
                p.y = -1.0_fx;
                return p;
            }

            case plane::top:
            {
                const auto t = (1.0_fx - a.y) / (b.y - a.y);
                auto p = a + (b - a) * t;
                p.y = 1.0_fx;
                return p;
            }
            }

            return a;
        };

        std::array<vec3, 8> polygon{
            quad[0],
            quad[1],
            quad[2],
            quad[3],
        };

        std::array<vec3, 8> scratch{};

        size_t count = 4;

        for (const auto clip_plane : planes)
        {
            size_t inside_count = 0;

            for (size_t i = 0; i < count; ++i)
                inside_count += inside(polygon[i], clip_plane);

            // Trivial reject.
            if (inside_count == 0)
                return 0;

            // Trivial accept for this plane.
            if (inside_count == count)
                continue;

            size_t output_count = 0;

            auto previous = polygon[count - 1];
            auto previous_inside = inside(previous, clip_plane);

            for (size_t i = 0; i < count; ++i)
            {
                const auto current = polygon[i];
                const auto current_inside = inside(current, clip_plane);

                if (current_inside != previous_inside)
                    scratch[output_count++] =
                        intersect(previous, current, clip_plane);

                if (current_inside)
                    scratch[output_count++] = current;

                previous = current;
                previous_inside = current_inside;
            }

            count = output_count;
            std::swap(polygon, scratch);
        }

        if (count < 3)
            return 0;

        // 3 vertices -> one degenerate quad.
        if (count == 3)
        {
            outVerts[0] = polygon[0];
            outVerts[1] = polygon[1];
            outVerts[2] = polygon[2];
            outVerts[3] = polygon[2];

            return 4;
        }

        // 4 vertices -> one real quad.
        if (count == 4)
        {
            for (size_t i = 0; i < 4; ++i)
                outVerts[i] = polygon[i];

            return 4;
        }

        // For 5-8 vertices, emit real quads first and use a degenerate
        // quad only for the final triangle when necessary.
        size_t out = 0;

        // A polygon fan produces triangles:
        //
        // (0,1,2), (0,2,3), (0,3,4), ...
        //
        // Pair adjacent triangles into quads where possible.
        size_t triangle_count = count - 2;
        size_t paired_triangles = triangle_count & ~size_t{1};

        for (size_t i = 0; i < paired_triangles; i += 2)
        {
            const auto a = i + 1;
            const auto b = i + 2;
            const auto c = i + 3;

            outVerts[out++] = polygon[0];
            outVerts[out++] = polygon[a];
            outVerts[out++] = polygon[b];
            outVerts[out++] = polygon[c];
        }

        // One final triangle remains for odd triangle counts.
        if (paired_triangles != triangle_count)
        {
            const auto i = paired_triangles + 1;

            outVerts[out++] = polygon[0];
            outVerts[out++] = polygon[i];
            outVerts[out++] = polygon[i + 1];
            outVerts[out++] = polygon[i + 1];
        }

        return out;
    }

    auto project_to_ndc(vec3& p) -> void
    {
        p.x = p.x * aspect_ratio_;
        p.x = p.x / p.z;
        p.y = p.y / p.z;
    }

    [[nodiscard]] auto is_cull_passing(vec3 const& v0, vec3 const& v1, vec3 const& v2) -> bool
    {
        const auto ax = v1.x - v0.x;
        const auto ay = v1.y - v0.y;
        const auto bx = v2.x - v0.x;
        const auto by = v2.y - v0.y;

        const auto cross = ax * by - ay * bx;

        if (cull_ == FaceCullMode::Back) [[likely]] { return cross > 0.0_fx; }
        else if (cull_ == FaceCullMode::None) { return true; }
        else if (cull_ == FaceCullMode::Front) { return cross < 0.0_fx; }
        else { return false; }
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
    fixed32 near_z_{0.0_fx};

    std::inplace_vector<vec3,MAX_VERTS> working_vertex_buffer_;
    std::inplace_vector<uint16_t,MAX_VERTS> working_color_buffer_;

    FaceCullMode cull_{FaceCullMode::All};


};



}
