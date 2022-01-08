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

#ifndef GATES_ORTHOGRAPHICCAMERA_HPP
#define GATES_ORTHOGRAPHICCAMERA_HPP

#include "pch.hpp"

namespace Gates {
  class OrthographicCamera {
   public:
    OrthographicCamera(float left, float top, float right, float bottom);
    OrthographicCamera(float view_distance, float aspect_ratio);

    void setProjection(float left, float right, float bottom, float top);
    void setProjection(float view_distance, float aspect_ratio);

    const glm::vec3& getPosition() const;
    void             setPosition(const glm::vec3& position);
    void             offsetPosition(const glm::vec3& offset);

    float getRotation() const;
    void  setRotation(float rotation);

    const glm::mat4& getProjectionMatrix() const;
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getViewProjectionMatrix() const;

    glm::vec2 ScreenToWorld(const glm::vec2& pos, const float& view_distance, const float& aspect_ratio);

   private:
    void pRecalculateViewMatrix();

   private:
    glm::mat4 pProjectionMatrix;
    glm::mat4 pViewMatrix;
    glm::mat4 pViewProjectionMatrix;

    glm::vec3 pPosition = {0.0f, 0.0f, 0.0f};
    float     pRotation = 0.0f;
  };
}

#endif
