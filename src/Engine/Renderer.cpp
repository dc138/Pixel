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

#include "Engine/Texture.hpp"
#include "pch.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Shader.hpp"

namespace Gates {
  static struct {
    GLuint gl_tri_vertex_array  = 0;
    GLuint gl_line_vertex_array = 0;

    GLuint gl_tri_vertex_buffer = 0;
    GLuint gl_tri_index_buffer  = 0;

    GLuint gl_line_vertex_buffer = 0;
    GLuint gl_line_index_buffer  = 0;

    Vertex*   tri_vertex_buffer         = nullptr;
    Vertex*   tri_vertex_buffer_current = nullptr;
    uint32_t* tri_index_buffer          = nullptr;
    uint32_t  tri_vertex_count          = 0;
    uint32_t  tri_index_count           = 0;

    Vertex*   line_vertex_buffer         = nullptr;
    Vertex*   line_vertex_buffer_current = nullptr;
    uint32_t* line_index_buffer          = nullptr;
    uint32_t  line_vertex_count          = 0;
    uint32_t  line_index_count           = 0;

    uint32_t texture_slots[Renderer::pMaxTextures] = {};
    uint32_t texture_slot_index                    = 0;

    Renderer::Stats stats = {};

    std::unique_ptr<ShaderProgram> shader_program;

    glm::mat4 view_projection = glm::mat4(1.f);
    glm::mat4 transform       = glm::mat4(1.f);
  } data;

  Texture Renderer::white_texture = Texture();

  void Renderer::Init() {
    // Shaders
    data.shader_program = std::make_unique<ShaderProgram>("assets/shaders/shader.vert", "assets/shaders/shader.frag");
    data.shader_program->Use();

    int samplers[pMaxTextures];
    for (uint32_t i = 0; i < pMaxTextures; i++) {
      samplers[i] = i;
    }

    glUniform1iv(glGetUniformLocation(data.shader_program->getProgram(), "u_textures"), pMaxTextures, samplers);

    // Dynamic arrays
    data.tri_vertex_buffer = new Vertex[pMaxVertexCount];
    data.tri_index_buffer  = new uint32_t[pMaxIndexCount];

    data.line_vertex_buffer = new Vertex[pMaxVertexCount];
    data.line_index_buffer  = new uint32_t[pMaxIndexCount];

    // Tris
    glCreateVertexArrays(1, &data.gl_tri_vertex_array);
    glBindVertexArray(data.gl_tri_vertex_array);

    glCreateBuffers(1, &data.gl_tri_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, data.gl_tri_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, pMaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glCreateBuffers(1, &data.gl_tri_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.gl_tri_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMaxIndexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(data.gl_tri_vertex_array, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    glEnableVertexArrayAttrib(data.gl_tri_vertex_array, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

    glEnableVertexArrayAttrib(data.gl_tri_vertex_array, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));

    glEnableVertexArrayAttrib(data.gl_tri_vertex_array, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_id));

    // Lines
    glCreateVertexArrays(1, &data.gl_line_vertex_array);
    glBindVertexArray(data.gl_line_vertex_array);

    glCreateBuffers(1, &data.gl_line_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, data.gl_line_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, pMaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glCreateBuffers(1, &data.gl_line_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.gl_line_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMaxIndexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(data.gl_line_vertex_array, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    glEnableVertexArrayAttrib(data.gl_line_vertex_array, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

    glEnableVertexArrayAttrib(data.gl_line_vertex_array, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coord));

    glEnableVertexArrayAttrib(data.gl_line_vertex_array, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_id));

    white_texture.Load(glm::vec4 {1.f, 1.f, 1.f, 1.f});
    memset(data.texture_slots, 0, pMaxTextures);
  }

  void Renderer::Delete() {
    glDeleteVertexArrays(1, &data.gl_tri_vertex_array);
    glDeleteVertexArrays(1, &data.gl_line_vertex_array);

    glDeleteBuffers(1, &data.gl_tri_vertex_buffer);
    glDeleteBuffers(1, &data.gl_tri_index_buffer);

    glDeleteBuffers(1, &data.gl_line_vertex_buffer);
    glDeleteBuffers(1, &data.gl_line_index_buffer);

    white_texture.Release();

    delete[] data.tri_vertex_buffer;
    delete[] data.tri_index_buffer;

    delete[] data.line_vertex_buffer;
    delete[] data.line_index_buffer;
  }

  void BeginTriBatch() { data.tri_vertex_buffer_current = data.tri_vertex_buffer; }

  void BeginLineBatch() { data.line_vertex_buffer_current = data.line_vertex_buffer; }

  void EndTriBatch() {
    data.shader_program->Use();

    glUniformMatrix4fv(glGetUniformLocation(data.shader_program->getProgram(), "u_view_projection"),
                       1,
                       GL_FALSE,
                       &data.view_projection[0][0]);

    glUniformMatrix4fv(
        glGetUniformLocation(data.shader_program->getProgram(), "u_transform"), 1, GL_FALSE, &data.transform[0][0]);

    glBindVertexArray(data.gl_tri_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, data.gl_tri_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.tri_vertex_count * sizeof(Vertex), data.tri_vertex_buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.gl_tri_index_buffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.tri_index_count * sizeof(uint32_t), data.tri_index_buffer);
  }

  void EndLineBatch() {
    data.shader_program->Use();

    glUniformMatrix4fv(glGetUniformLocation(data.shader_program->getProgram(), "u_view_projection"),
                       1,
                       GL_FALSE,
                       &data.view_projection[0][0]);

    glUniformMatrix4fv(
        glGetUniformLocation(data.shader_program->getProgram(), "u_transform"), 1, GL_FALSE, &data.transform[0][0]);

    glBindVertexArray(data.gl_line_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, data.gl_line_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.line_vertex_count * sizeof(Vertex), data.line_vertex_buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.gl_line_index_buffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.line_index_count * sizeof(uint32_t), data.line_index_buffer);
  }

  void FlushTriBatch() {
    for (uint32_t i = 0; i < data.texture_slot_index; i++) {
      glBindTextureUnit(i, data.texture_slots[i]);
    }

    glBindVertexArray(data.gl_tri_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, data.gl_tri_vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.gl_tri_index_buffer);
    glDrawElements(GL_TRIANGLES, data.tri_index_count, GL_UNSIGNED_INT, nullptr);

    data.stats.tri_index_count += data.tri_index_count;
    data.stats.tri_vertex_count += data.tri_vertex_count;

    data.tri_index_count    = 0;
    data.tri_vertex_count   = 0;
    data.texture_slot_index = 0;

    data.stats.draw_calls++;
  }

  void FlushLineBatch() {
    for (uint32_t i = 0; i < data.texture_slot_index; i++) {
      glBindTextureUnit(i, data.texture_slots[i]);
    }

    glBindVertexArray(data.gl_line_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, data.gl_line_vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.gl_line_index_buffer);
    glDrawElements(GL_LINES, data.line_index_count, GL_UNSIGNED_INT, nullptr);

    data.stats.line_index_count += data.line_index_count;
    data.stats.line_vertex_count += data.line_vertex_count;

    data.line_index_count  = 0;
    data.line_vertex_count = 0;

    data.stats.draw_calls++;
  }

  void Renderer::BeginBatch() {
    data.texture_slots[0] = white_texture.getId();
    data.texture_slot_index++;

    BeginTriBatch();
    BeginLineBatch();
  }

  void Renderer::EndBatch() {
    EndTriBatch();
    EndLineBatch();
  }

  void Renderer::FlushBatch() {
    FlushTriBatch();
    FlushLineBatch();
  }

  void Renderer::DrawQuad(const glm::vec2& position,
                          const glm::vec2& size,
                          const glm::vec4& color,
                          const Texture&   texture) {
    if ((data.tri_index_count + 6) >= pMaxIndexCount || (data.tri_vertex_count + 4) >= pMaxVertexCount ||
        data.texture_slot_index > (pMaxTextures - 1)) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    float tex_index = -1.f;

    for (uint32_t i = 0; i < data.texture_slot_index; i++) {
      if (data.texture_slots[i] == texture.getId()) {
        tex_index = (float)i;
      }
    }

    if (tex_index == -1.f) {
      tex_index                                   = (float)data.texture_slot_index;
      data.texture_slots[data.texture_slot_index] = texture.getId();
      data.texture_slot_index++;
    }

    data.tri_vertex_buffer_current->position  = {position.x, position.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = tex_index;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {position.x + size.x, position.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {1.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = tex_index;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {position.x + size.x, position.y + size.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {1.f, 1.f};
    data.tri_vertex_buffer_current->tex_id    = tex_index;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {position.x, position.y + size.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 1.f};
    data.tri_vertex_buffer_current->tex_id    = tex_index;
    data.tri_vertex_buffer_current++;

    data.tri_index_buffer[data.tri_index_count + 0] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 1] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 2] = 2 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 3] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 4] = 3 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 5] = 0 + data.tri_vertex_count;

    data.tri_vertex_count += 4;
    data.tri_index_count += 6;

    data.stats.quads_drawn++;
  }

  void Renderer::DrawTri(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec4& color) {
    if ((data.tri_index_count + 3) >= pMaxIndexCount || (data.tri_vertex_count + 3) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    data.tri_vertex_buffer_current->position  = {v1.x, v1.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v2.x, v2.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v3.x, v3.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_index_buffer[data.tri_index_count + 0] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 1] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 2] = 2 + data.tri_vertex_count;

    data.tri_vertex_count += 3;
    data.tri_index_count += 3;

    data.stats.tris_drawn++;
  }

  void Renderer::DrawCircle(const glm::vec2& position, float radius, uint32_t segments, const glm::vec4& color) {
    if ((data.tri_index_count + segments * 3) >= pMaxIndexCount ||
        (data.tri_vertex_count + segments + 1) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    float inc = glm::two_pi<float>() / segments;

    data.tri_vertex_buffer_current->position  = {position.x, position.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    for (uint32_t current = 0; current < segments; current++) {
      data.tri_vertex_buffer_current->position = {
          glm::cos(current * inc) * radius + position.x, glm::sin(current * inc) * radius + position.y, 0.f};
      data.tri_vertex_buffer_current->color     = color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_index_buffer[data.tri_index_count + 0] = data.tri_vertex_count;
      data.tri_index_buffer[data.tri_index_count + 1] = data.tri_vertex_count + current + 1;
      data.tri_index_buffer[data.tri_index_count + 2] =
          data.tri_vertex_count + (current == segments - 1 ? 1 : current + 2);
      data.tri_index_count += 3;
    }

    data.tri_vertex_count += (segments + 1);
    data.stats.circles_drawn++;
  }

  void Renderer::DrawLine(const glm::vec2& pos1, const glm::vec2& pos2, const glm::vec4& color) {
    if ((data.line_index_count + 2) >= pMaxIndexCount || (data.line_vertex_count + 2) >= pMaxVertexCount) {
      EndLineBatch();
      FlushLineBatch();
      BeginLineBatch();
    }

    data.line_vertex_buffer_current->position = {pos1.x, pos1.y, 0.f};
    data.line_vertex_buffer_current->color    = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.line_vertex_buffer_current++;

    data.line_vertex_buffer_current->position = {pos2.x, pos2.y, 0.f};
    data.line_vertex_buffer_current->color    = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.line_vertex_buffer_current++;

    data.line_index_buffer[data.line_index_count + 0] = 0 + data.line_vertex_count;
    data.line_index_buffer[data.line_index_count + 1] = 1 + data.line_vertex_count;

    data.line_vertex_count += 2;
    data.line_index_count += 2;

    data.stats.lines_drawn++;
  }

  void Renderer::DrawLine(const glm::vec2& pos1, const glm::vec2& pos2, float width, const glm::vec4& color) {
    if ((data.tri_index_count + 6) >= pMaxIndexCount || (data.tri_vertex_count + 4) >= pMaxVertexCount ||
        data.texture_slot_index > (pMaxTextures - 1)) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    const glm::vec2 diff   = pos2 - pos1;
    const glm::vec2 offset = (width * diff) / (glm::length(diff) * 2);

    data.tri_vertex_buffer_current->position  = {pos1.x + offset.y, pos1.y - offset.x, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {pos1.x - offset.y, pos1.y + offset.x, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {1.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {pos2.x - offset.y, pos2.y + offset.x, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {1.f, 1.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {pos2.x + offset.y, pos2.y - offset.x, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 1.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_index_buffer[data.tri_index_count + 0] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 1] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 2] = 2 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 3] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 4] = 3 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 5] = 0 + data.tri_vertex_count;

    data.tri_vertex_count += 4;
    data.tri_index_count += 6;

    data.stats.wide_lines_drawn++;
  }

  void Renderer::OutlineQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    if ((data.line_index_count + 8) >= pMaxIndexCount || (data.line_vertex_count + 4) >= pMaxVertexCount) {
      EndLineBatch();
      FlushLineBatch();
      BeginLineBatch();
    }

    data.line_vertex_buffer_current->position = {position.x, position.y, 0.f};
    data.line_vertex_buffer_current->color    = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.line_vertex_buffer_current++;

    data.line_vertex_buffer_current->position = {position.x, position.y + size.y, 0.f};
    data.line_vertex_buffer_current->color    = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.line_vertex_buffer_current++;

    data.line_vertex_buffer_current->position = {position.x + size.x, position.y + size.y, 0.f};
    data.line_vertex_buffer_current->color    = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.line_vertex_buffer_current++;

    data.line_vertex_buffer_current->position = {position.x + size.x, position.y, 0.f};
    data.line_vertex_buffer_current->color    = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.line_vertex_buffer_current++;

    data.line_index_buffer[data.line_index_count + 0] = 0 + data.line_vertex_count;
    data.line_index_buffer[data.line_index_count + 1] = 1 + data.line_vertex_count;

    data.line_index_buffer[data.line_index_count + 2] = 1 + data.line_vertex_count;
    data.line_index_buffer[data.line_index_count + 3] = 2 + data.line_vertex_count;

    data.line_index_buffer[data.line_index_count + 4] = 2 + data.line_vertex_count;
    data.line_index_buffer[data.line_index_count + 5] = 3 + data.line_vertex_count;

    data.line_index_buffer[data.line_index_count + 6] = 3 + data.line_vertex_count;
    data.line_index_buffer[data.line_index_count + 7] = 0 + data.line_vertex_count;

    data.line_vertex_count += 4;
    data.line_index_count += 8;

    data.stats.quads_outlined++;
  }

  void Renderer::OutlineTri(const glm::vec2& v1,
                            const glm::vec2& v2,
                            const glm::vec2& v3,
                            float            width,
                            const glm::vec4& color) {
    const float a = glm::length(v1 - v2);
    const float b = glm::length(v2 - v3);
    const float c = glm::length(v3 - v1);

    const float s = (a + b + c) / 2;
    const float r = std::sqrt(((s - a) * (s - b) * (s - c)) / s);

    const glm::vec2 center = ((v1 * b) + (v2 * c) + (v3 * a)) / (a + b + c);

    const glm::vec2 v1_i = (width * (center - v1) / r) + v1;
    const glm::vec2 v2_i = (width * (center - v2) / r) + v2;
    const glm::vec2 v3_i = (width * (center - v3) / r) + v3;

    data.tri_vertex_buffer_current->position  = {v1.x, v1.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v2.x, v2.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v3.x, v3.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v1_i.x, v1_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v2_i.x, v2_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v3_i.x, v3_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_index_buffer[data.tri_index_count + 0] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 1] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 2] = 4 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 3] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 4] = 3 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 5] = 4 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 6] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 7] = 4 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 8] = 5 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 9]  = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 10] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 11] = 5 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 12] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 13] = 5 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 14] = 3 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 15] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 16] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 17] = 3 + data.tri_vertex_count;

    data.tri_vertex_count += 6;
    data.tri_index_count += 18;

    data.stats.tris_outlined++;
  }

  void Renderer::BorderTri(const glm::vec2& v1,
                           const glm::vec2& v2,
                           const glm::vec2& v3,
                           float            width,
                           const glm::vec4& inner_color,
                           const glm::vec4& outter_color) {
    const float a = glm::length(v1 - v2);
    const float b = glm::length(v2 - v3);
    const float c = glm::length(v3 - v1);

    const float s = (a + b + c) / 2;
    const float r = std::sqrt(((s - a) * (s - b) * (s - c)) / s);

    const glm::vec2 center = ((v1 * b) + (v2 * c) + (v3 * a)) / (a + b + c);

    const glm::vec2 v1_i = (width * (center - v1) / r) + v1;
    const glm::vec2 v2_i = (width * (center - v2) / r) + v2;
    const glm::vec2 v3_i = (width * (center - v3) / r) + v3;

    data.tri_vertex_buffer_current->position  = {v1.x, v1.y, 0.f};
    data.tri_vertex_buffer_current->color     = outter_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v2.x, v2.y, 0.f};
    data.tri_vertex_buffer_current->color     = outter_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v3.x, v3.y, 0.f};
    data.tri_vertex_buffer_current->color     = outter_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v1_i.x, v1_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = outter_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v2_i.x, v2_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = outter_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v3_i.x, v3_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = outter_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v1_i.x, v1_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v2_i.x, v2_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_buffer_current->position  = {v3_i.x, v3_i.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_index_buffer[data.tri_index_count + 0] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 1] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 2] = 4 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 3] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 4] = 3 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 5] = 4 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 6] = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 7] = 4 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 8] = 5 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 9]  = 1 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 10] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 11] = 5 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 12] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 13] = 5 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 14] = 3 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 15] = 2 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 16] = 0 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 17] = 3 + data.tri_vertex_count;

    data.tri_index_buffer[data.tri_index_count + 18] = 6 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 19] = 7 + data.tri_vertex_count;
    data.tri_index_buffer[data.tri_index_count + 20] = 8 + data.tri_vertex_count;

    data.tri_vertex_count += 9;
    data.tri_index_count += 21;

    data.stats.tris_bordered++;
  }

  void Renderer::OutlineCircle(const glm::vec2& position,
                               float            radius,
                               uint32_t         segments,
                               float            width,
                               const glm::vec4& color) {
    if ((data.tri_index_count + segments * 6) >= pMaxIndexCount ||
        (data.tri_vertex_count + segments * 2) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    const float inc = glm::two_pi<float>() / segments;
    const float w   = width / std::cos(inc / 2);

    for (uint32_t current = 0; current < segments; current++) {
      const glm::vec2 outter_pos = {glm::cos(current * inc) * radius + position.x,
                                    glm::sin(current * inc) * radius + position.y};

      const glm::vec2 inner_pos = {glm::cos(current * inc) * (radius - w) + position.x,
                                   glm::sin(current * inc) * (radius - w) + position.y};

      data.tri_vertex_buffer_current->position  = {outter_pos.x, outter_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_index_buffer[data.tri_index_count + 0] = data.tri_vertex_count + (current * 2);
      data.tri_index_buffer[data.tri_index_count + 1] = data.tri_vertex_count + ((current * 2) + 1);
      data.tri_index_buffer[data.tri_index_count + 2] = data.tri_vertex_count + ((2 * current) + 3) % (segments * 2);

      data.tri_index_buffer[data.tri_index_count + 3] = data.tri_vertex_count + (current * 2);
      data.tri_index_buffer[data.tri_index_count + 4] = data.tri_vertex_count + ((current * 2) + 2) % (segments * 2);
      data.tri_index_buffer[data.tri_index_count + 5] = data.tri_vertex_count + ((2 * current) + 3) % (segments * 2);

      data.tri_index_count += 6;
    }

    data.tri_vertex_count += (2 * segments);
    data.stats.circles_outlined++;
  }

  void Renderer::BorderCircle(const glm::vec2& position,
                              float            radius,
                              uint32_t         segments,
                              float            width,
                              const glm::vec4& inner_color,
                              const glm::vec4& outter_color) {
    if ((data.tri_index_count + segments * 9) >= pMaxIndexCount ||
        (data.tri_vertex_count + segments * 3 + 1) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    const float inc = glm::two_pi<float>() / segments;
    const float w   = width / std::cos(inc / 2);

    for (uint32_t current = 0; current < segments; current++) {
      const glm::vec2 outter_pos = {glm::cos(current * inc) * radius + position.x,
                                    glm::sin(current * inc) * radius + position.y};

      const glm::vec2 inner_pos = {glm::cos(current * inc) * (radius - w) + position.x,
                                   glm::sin(current * inc) * (radius - w) + position.y};

      data.tri_vertex_buffer_current->position  = {outter_pos.x, outter_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = inner_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_index_buffer[data.tri_index_count + 0] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 1] = data.tri_vertex_count + ((current * 3) + 1);
      data.tri_index_buffer[data.tri_index_count + 2] = data.tri_vertex_count + ((current * 3) + 4) % (segments * 3);

      data.tri_index_buffer[data.tri_index_count + 3] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 4] = data.tri_vertex_count + ((current * 3) + 3) % (segments * 3);
      data.tri_index_buffer[data.tri_index_count + 5] = data.tri_vertex_count + ((current * 3) + 4) % (segments * 3);

      data.tri_index_buffer[data.tri_index_count + 6] = data.tri_vertex_count + (current * 3) + 2;
      data.tri_index_buffer[data.tri_index_count + 7] = data.tri_vertex_count + ((current * 3) + 5) % (segments * 3);
      data.tri_index_buffer[data.tri_index_count + 8] = data.tri_vertex_count + 3 * segments;

      data.tri_index_count += 9;
    }

    data.tri_vertex_buffer_current->position  = {position.x, position.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_count += (3 * segments + 1);
    data.stats.circles_outlined++;
  }

  void Renderer::BorderSemicircle(const glm::vec2& position,
                                  float            radius,
                                  float            start_angle,
                                  float            end_angle,
                                  uint32_t         segments,
                                  float            width,
                                  const glm::vec4& inner_color,
                                  const glm::vec4& outter_color) {
    if ((data.tri_index_count + segments * 9) >= pMaxIndexCount ||
        (data.tri_vertex_count + segments * 3 + 4) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    const float inc          = (end_angle - start_angle) / segments;
    const float inner_radius = radius - (width / std::cos(inc / 2));

    for (uint32_t current = 0; current <= segments; current++) {
      const float current_angle = current * inc + start_angle;

      const glm::vec2 outter_pos = {glm::cos(current_angle) * radius + position.x,
                                    glm::sin(current_angle) * radius + position.y};

      const glm::vec2 inner_pos = {glm::cos(current_angle) * inner_radius + position.x,
                                   glm::sin(current_angle) * inner_radius + position.y};

      data.tri_vertex_buffer_current->position  = {outter_pos.x, outter_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = inner_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;
    }

    for (uint32_t current = 0; current < segments; current++) {
      data.tri_index_buffer[data.tri_index_count + 0] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 1] = data.tri_vertex_count + (current * 3) + 1;
      data.tri_index_buffer[data.tri_index_count + 2] = data.tri_vertex_count + (current * 3) + 4;

      data.tri_index_buffer[data.tri_index_count + 3] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 4] = data.tri_vertex_count + (current * 3) + 3;
      data.tri_index_buffer[data.tri_index_count + 5] = data.tri_vertex_count + (current * 3) + 4;

      data.tri_index_buffer[data.tri_index_count + 6] = data.tri_vertex_count + (current * 3) + 2;
      data.tri_index_buffer[data.tri_index_count + 7] = data.tri_vertex_count + (current * 3) + 5;
      data.tri_index_buffer[data.tri_index_count + 8] = data.tri_vertex_count + (segments * 3) + 3;

      data.tri_index_count += 9;
    }

    data.tri_vertex_buffer_current->position  = {position.x, position.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_count += (3 * segments + 4);
    data.stats.semicircles_bordered++;
  }

  void Renderer::BorderSemicircleCustomCenterInside(const glm::vec2& position,
                                                    const glm::vec2& center,
                                                    float            radius,
                                                    float            start_angle,
                                                    float            end_angle,
                                                    uint32_t         segments,
                                                    float            width,
                                                    const glm::vec4& inner_color,
                                                    const glm::vec4& outter_color) {
    if ((data.tri_index_count + segments * 9) >= pMaxIndexCount ||
        (data.tri_vertex_count + segments * 3 + 4) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    const float inc          = (end_angle - start_angle) / segments;
    const float inner_radius = radius - (width / std::cos(inc / 2));

    const glm::vec2 offset            = center - position;
    const bool      is_center_outside = ((offset.x * offset.x) + (offset.y * offset.y)) > (radius * radius);

    for (uint32_t current = 0; current <= segments; current++) {
      const float current_angle = current * inc + start_angle;

      const glm::vec2 outter_pos = {glm::cos(current_angle) * radius + position.x,
                                    glm::sin(current_angle) * radius + position.y};

      const glm::vec2 inner_pos = {glm::cos(current_angle) * inner_radius + position.x,
                                   glm::sin(current_angle) * inner_radius + position.y};

      data.tri_vertex_buffer_current->position  = {outter_pos.x, outter_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = inner_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;
    }

    for (uint32_t current = 0; current < segments; current++) {
      data.tri_index_buffer[data.tri_index_count + 0] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 1] = data.tri_vertex_count + (current * 3) + 1;
      data.tri_index_buffer[data.tri_index_count + 2] = data.tri_vertex_count + (current * 3) + 4;

      data.tri_index_buffer[data.tri_index_count + 3] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 4] = data.tri_vertex_count + (current * 3) + 3;
      data.tri_index_buffer[data.tri_index_count + 5] = data.tri_vertex_count + (current * 3) + 4;

      data.tri_index_buffer[data.tri_index_count + 6] = data.tri_vertex_count + (current * 3) + 2;
      data.tri_index_buffer[data.tri_index_count + 7] = data.tri_vertex_count + (current * 3) + 5;
      data.tri_index_buffer[data.tri_index_count + 8] = data.tri_vertex_count + (segments * 3) + 3;

      data.tri_index_count += 9;
    }

    data.tri_vertex_buffer_current->position  = {center.x, center.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_count += (3 * segments + 4);
    data.stats.semicircles_bordered++;
  }

  void Renderer::BorderSemicircleCustomCenterOutside(const glm::vec2& position,
                                                     const glm::vec2& center,
                                                     float            radius,
                                                     float            start_angle,
                                                     float            end_angle,
                                                     uint32_t         segments,
                                                     float            width,
                                                     const glm::vec4& inner_color,
                                                     const glm::vec4& outter_color) {
    if ((data.tri_index_count + segments * 9) >= pMaxIndexCount ||
        (data.tri_vertex_count + segments * 3 + 4) >= pMaxVertexCount) {
      EndTriBatch();
      FlushTriBatch();
      BeginTriBatch();
    }

    const float inc          = (end_angle - start_angle) / segments;
    const float inner_radius = radius - (width / std::cos(inc / 2));

    const glm::vec2 offset = center - position;

    for (uint32_t current = 0; current <= segments; current++) {
      const float current_angle = current * inc + start_angle;

      const glm::vec2 outter_pos = {glm::cos(current_angle) * radius + position.x,
                                    glm::sin(current_angle) * radius + position.y};

      const glm::vec2 inner_pos = {glm::cos(current_angle) * inner_radius + position.x,
                                   glm::sin(current_angle) * inner_radius + position.y};

      data.tri_vertex_buffer_current->position  = {outter_pos.x, outter_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {inner_pos.x, inner_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = outter_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;

      data.tri_vertex_buffer_current->position  = {outter_pos.x, outter_pos.y, 0.f};
      data.tri_vertex_buffer_current->color     = inner_color;
      data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
      data.tri_vertex_buffer_current->tex_id    = 0;
      data.tri_vertex_buffer_current++;
    }

    for (uint32_t current = 0; current < segments; current++) {
      data.tri_index_buffer[data.tri_index_count + 0] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 1] = data.tri_vertex_count + (current * 3) + 1;
      data.tri_index_buffer[data.tri_index_count + 2] = data.tri_vertex_count + (current * 3) + 4;

      data.tri_index_buffer[data.tri_index_count + 3] = data.tri_vertex_count + (current * 3);
      data.tri_index_buffer[data.tri_index_count + 4] = data.tri_vertex_count + (current * 3) + 3;
      data.tri_index_buffer[data.tri_index_count + 5] = data.tri_vertex_count + (current * 3) + 4;

      data.tri_index_buffer[data.tri_index_count + 6] = data.tri_vertex_count + (current * 3) + 2;
      data.tri_index_buffer[data.tri_index_count + 7] = data.tri_vertex_count + (current * 3) + 5;
      data.tri_index_buffer[data.tri_index_count + 8] = data.tri_vertex_count + (segments * 3) + 3;

      data.tri_index_count += 9;
    }

    data.tri_vertex_buffer_current->position  = {center.x, center.y, 0.f};
    data.tri_vertex_buffer_current->color     = inner_color;
    data.tri_vertex_buffer_current->tex_coord = {0.f, 0.f};
    data.tri_vertex_buffer_current->tex_id    = 0;
    data.tri_vertex_buffer_current++;

    data.tri_vertex_count += (3 * segments + 4);
    data.stats.semicircles_bordered++;
  }

  void Renderer::SetViewProjection(const glm::mat4& view_projection) { data.view_projection = view_projection; }

  void Renderer::SetTransform(const glm::vec3& transform) {
    data.transform = glm::translate(glm::mat4(1.f), transform);
  }

  void Renderer::UseCamera(const OrthographicCamera& camera) {
    data.view_projection = camera.getViewProjectionMatrix();
  }

  void                   Renderer::ResetStats() { memset(&data.stats, 0, sizeof(Stats)); }
  const Renderer::Stats& Renderer::GetStats() { return data.stats; }
}
