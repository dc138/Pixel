/*
Gates, a simple logic circuit simulator written in C++
Copyright (C) 2022 DarthChungo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef GATES_RENDERER_HPP
#define GATES_RENDERER_HPP

#include "pch.hpp"
#include "Engine/OrthographicCamera.hpp"
#include "Engine/Texture.hpp"

namespace Gates {
  struct Vertex {
    glm::vec3 position {};
    glm::vec4 color {};
    glm::vec2 tex_coord {};
    float     tex_id = 0.f;
  };

  class Renderer {
   public:
    static void Init();
    static void Delete();

    static void BeginBatch();
    static void EndBatch();
    static void FlushBatch();

    static void DrawQuad(const glm::vec2& position,
                         const glm::vec2& size,
                         const glm::vec4& color   = {1.f, 1.f, 1.f, 1.f},
                         const Texture&   texture = white_texture);

    static void DrawTri(const glm::vec2& v1,
                        const glm::vec2& v2,
                        const glm::vec2& v3,
                        const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void DrawCircle(const glm::vec2& position,
                           float            radius,
                           uint32_t         segments,
                           const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void DrawLine(const glm::vec2& pos1, const glm::vec2& pos2, const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void DrawLine(const glm::vec2& pos1,
                         const glm::vec2& pos2,
                         float            width,
                         const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void OutlineQuad(const glm::vec2& position,
                            const glm::vec2& size,
                            const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void OutlineTri(const glm::vec2& v1,
                           const glm::vec2& v2,
                           const glm::vec2& v3,
                           float            width,
                           const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void BorderTri(const glm::vec2& v1,
                          const glm::vec2& v2,
                          const glm::vec2& v3,
                          float            width,
                          const glm::vec4& inner_color,
                          const glm::vec4& outter_color = {1.f, 1.f, 1.f, 1.f});

    static void OutlineCircle(const glm::vec2& position,
                              float            radius,
                              uint32_t         segments,
                              float            width,
                              const glm::vec4& color = {1.f, 1.f, 1.f, 1.f});

    static void BorderCircle(const glm::vec2& position,
                             float            radius,
                             uint32_t         segments,
                             float            width,
                             const glm::vec4& inner_color,
                             const glm::vec4& outter_color = {1.f, 1.f, 1.f, 1.f});

    static void BorderSemicircle(const glm::vec2& position,
                                 float            radius,
                                 float            start_angle,
                                 float            end_angle,
                                 uint32_t         segments,
                                 float            width,
                                 const glm::vec4& inner_color,
                                 const glm::vec4& outter_color = {1.f, 1.f, 1.f, 1.f});

    static void BorderSemicircleCustomCenterInside(const glm::vec2& position,
                                                   const glm::vec2& center,
                                                   float            radius,
                                                   float            start_angle,
                                                   float            end_angle,
                                                   uint32_t         segments,
                                                   float            width,
                                                   const glm::vec4& inner_color,
                                                   const glm::vec4& outter_color = {1.f, 1.f, 1.f, 1.f});

    static void BorderSemicircleCustomCenterOutside(const glm::vec2& position,
                                                    const glm::vec2& center,
                                                    float            radius,
                                                    float            start_angle,
                                                    float            end_angle,
                                                    uint32_t         segments,
                                                    float            width,
                                                    const glm::vec4& inner_color,
                                                    const glm::vec4& outter_color = {1.f, 1.f, 1.f, 1.f});

    static void SetViewProjection(const glm::mat4& view_projection);
    static void SetTransform(const glm::vec3& transform);
    static void UseCamera(const OrthographicCamera& camera);

    struct Stats {
      uint32_t quads_drawn    = 0;
      uint32_t quads_outlined = 0;

      uint32_t tris_drawn    = 0;
      uint32_t tris_outlined = 0;
      uint32_t tris_bordered = 0;

      uint32_t lines_drawn      = 0;
      uint32_t wide_lines_drawn = 0;

      uint32_t circles_drawn    = 0;
      uint32_t circles_outlined = 0;
      uint32_t circles_bordered = 0;

      uint32_t semicircles_bordered = 0;

      uint32_t draw_calls = 0;

      uint32_t tri_vertex_count = 0;
      uint32_t tri_index_count  = 0;

      uint32_t line_vertex_count = 0;
      uint32_t line_index_count  = 0;
    };

    static void         ResetStats();
    static const Stats& GetStats();

   public:
    static constexpr uint32_t pMaxVertexCount = 20000;
    static constexpr uint32_t pMaxIndexCount  = 30000;
    static constexpr uint32_t pMaxTextures    = 8;  // TODO: query current device being used

   public:
    static Texture white_texture;
  };
}

#endif
