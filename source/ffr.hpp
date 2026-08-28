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
    static constexpr size_t MAX_VERTS{256};

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

            auto draw = clip_horizontal_line_screen(min_x, v_top_y, max_x);
            if(draw == 0) {return;}
            if(draw == 1)
            {
                lineHorizontal(min_x, v_top_y, max_x, color);
            }
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


            auto draw = clip_horizontal_line_screen(x_a,y,x_b);
            if(draw == 0) {return;}
            if(draw == 1)
            {
                lineHorizontal(x_a, y, x_b, color);
            }

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
            auto draw = clip_horizontal_line_screen(x_a,y,x_b);
            if(draw == 0) { return; }
            if(draw == 1)
            {
                lineHorizontal(x_a, y, x_b, color);
            }

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


                    auto outVerts = clip_triangle_near(v0,v1,v2);
                    for(auto k = 0ul; k < outVerts.size(); k = k + 3)
                    {
                        if(is_cull_passing(outVerts[k+0],outVerts[k+1],outVerts[k+2]))
                        {
                        project_to_ndc(outVerts[k+0]);project_to_ndc(outVerts[k+1]);project_to_ndc(outVerts[k+2]);
                        //if(is_cull_passing(outVerts[k+0],outVerts[k+1],outVerts[k+2]))
                        {
                        to_screen_space(outVerts[k+0]);to_screen_space(outVerts[k+1]);to_screen_space(outVerts[k+2]);
                        triangle(outVerts[k+0].x,outVerts[k+0].y,outVerts[k+1].x,outVerts[k+1].y,outVerts[k+2].x,outVerts[k+2].y,ccs);
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


                    auto outVerts = clip_quad_near(v0,v1,v2,v3);
                    for(auto k = 0ul; k < outVerts.size(); k = k + 4)
                    {
                        if(is_cull_passing(outVerts[k+0],outVerts[k+1],outVerts[k+2]))
                        {
                        project_to_ndc(outVerts[k+0]);project_to_ndc(outVerts[k+1]);project_to_ndc(outVerts[k+2]);project_to_ndc(outVerts[k+3]);
                        //if(is_cull_passing(outVerts[k+0],outVerts[k+1],outVerts[k+2]))
                        {
                            to_screen_space(outVerts[k+0]);to_screen_space(outVerts[k+1]);to_screen_space(outVerts[k+2]);to_screen_space(outVerts[k+3]);
                            quad(outVerts[k+0].x,outVerts[k+0].y,outVerts[k+1].x,outVerts[k+1].y,outVerts[k+2].x,outVerts[k+2].y,outVerts[k+3].x,outVerts[k+3].y,ccs);
                        }
                        }


                    }


                i = i + 3;
                col = col + 4;
            }
        }

    }




protected:

    auto is_point_inside_near(vec3 const & p) -> bool
    {
        return p.z > near_z_;
    }


    auto clip_line_near(vec3& v0, vec3& v1) -> bool
    {
        //trivial pass
        if(is_point_inside_near(v0) && is_point_inside_near(v1))
        {
            return true;
        }

        //trivial fail
        else if((!is_point_inside_near(v0)) && (!is_point_inside_near(v1)))
        {
            return false;
        }

        //clamp if partial
        return false;
    }

    auto clip_triangle_near(vec3 const & v0, vec3 const & v1, vec3 const & v2) -> std::inplace_vector<vec3, 8>
    {
        //trivial pass
        if(is_point_inside_near(v0) && is_point_inside_near(v1) && is_point_inside_near(v2))
        {
            return {v0,v1,v2};
        }

        //trivial fail
        else if((!is_point_inside_near(v0)) && (!is_point_inside_near(v1)) && (!is_point_inside_near(v2)))
        {
            return {};
        }

        //clamp if partial
        return {};

    }

    auto clip_quad_near(vec3 const & v0, vec3 const & v1, vec3 const & v2, vec3 const & v3) -> std::inplace_vector<vec3, 8>
    {
        //trivial pass
        if(is_point_inside_near(v0) && is_point_inside_near(v1) && is_point_inside_near(v2) && (is_point_inside_near(v3)))
        {
            return {v0,v1,v2,v3};
        }

        //trivial fail
        else if((!is_point_inside_near(v0)) && (!is_point_inside_near(v1)) && (!is_point_inside_near(v2)) && (!is_point_inside_near(v3)))
        {
            return {};
        }

        //clamp if partial
        return {};
    }

    auto clip_horizontal_line_screen(int16_t const & x0, int16_t const & y0, int16_t const & x1) -> int32_t
    {

        if(y0 >= viewport_height_) { return 0; }

        if(y0 < 0) { return -1; }


        //this one
        if(x0 < 0 && x1 < 0)
        {
            return -1;
        }
        if (x0 >= viewport_width_ && x1 >= viewport_width_)
        {
            return -1;
        }

        return 1;
    }

    auto project_to_ndc(vec3& p) -> void
    {
        p.x = p.x * aspect_ratio_;
        p.x = p.x / p.z;
        p.y = p.y / p.z;
    }

    [[nodiscard]] auto is_cull_passing(vec3 const& v0, vec3 const& v1, vec3 const& v2) -> bool
    {
        if (cull_ == FaceCullMode::None) { return true; }
        else if (cull_ == FaceCullMode::All) { return false; }
        else
        {
            const auto ab_x = v1.x - v0.x;
            const auto ab_y = v1.y - v0.y;
            const auto ac_x = v2.x - v1.x;
            const auto ac_y = v2.y - v1.y;
            const auto nz = ab_x * ac_y - ab_y * ac_x;

            if (cull_ == FaceCullMode::Back) [[likely]] { return nz >= 0.0_fx; }
            else if (cull_ == FaceCullMode::Front) { return nz < 0.0_fx; }
        }
        return false;
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

    int16_t viewport_width_{0};
    int16_t viewport_height_{0};
    fixed32 viewport_width_fx_{0.0_fx};
    fixed32 viewport_height_fx_{0.0_fx};
    fixed32 aspect_ratio_{0.0_fx};
    fixed32 near_z_{0.0_fx};

    std::inplace_vector<vec3,MAX_VERTS> working_vertex_buffer_;
    std::inplace_vector<uint16_t,MAX_VERTS> working_color_buffer_;
    std::inplace_vector<vec3, 16> post_clip_verts1_;
    std::inplace_vector<vec3, 16> post_clip_verts2_;

    FaceCullMode cull_{FaceCullMode::All};


};



}
