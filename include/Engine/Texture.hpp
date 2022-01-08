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

#ifndef GATES_TEXTURE_HPP
#define GATES_TEXTURE_HPP

#include "pch.hpp"

namespace Gates {
  class Texture {
   public:
    void Load(const std::string& filepath);
    void Load(const glm::vec4& color);

    void Release();

    bool   Loaded() const { return pId == 0; }
    GLuint getId() const;

   private:
    GLuint pId = 0;
  };
}

#endif
