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

#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in float tex_index;

out vec4 our_color;
out vec2 our_tex_coord;
out float our_tex_index;

uniform mat4 u_view_projection;
uniform mat4 u_transform;

void main() {
    gl_Position = u_view_projection * u_transform * vec4(position, 1.0f);
    our_color = color;
    our_tex_coord = tex_coord;
    our_tex_index = tex_index;
}
