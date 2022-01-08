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

#include "Util/UUID.hpp"

namespace Gates {
  static std::random_device                      random_device;
  static std::mt19937_64                         engine(random_device());
  static std::uniform_int_distribution<uint64_t> uniform_ditribution;

  UUID::UUID() : pUUID(uniform_ditribution(engine)) {}

  UUID::UUID(uint64_t uuid) : pUUID(uuid) {}
}
