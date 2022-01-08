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

#ifndef GATES_MISC_HPP
#define GATES_MISC_HPP

#include "pch.hpp"

namespace Gates {
  template <typename Base, typename T>
  inline bool instanceof (const T *ptr) {
    return dynamic_cast<const Base *>(ptr) != nullptr;
  }
}

#define GATES_APPLICATION_LICENSE                                          \
  "Gates, a simple logic circuit simulator written in C++\n"               \
  "Copyright (C) 2022 DarthChungo\n"                                       \
  "\n"                                                                     \
  "This program is free software: you can redistribute it and/or modify\n" \
  "it under the terms of the GNU General Public License as published by\n" \
  "the Free Software Foundation, either version 3 of the License, or\n"    \
  "(at your option) any later version.\n"                                  \
  "\n"                                                                     \
  "This program is distributed in the hope that it will be useful,\n"      \
  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"       \
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"        \
  "GNU General Public License for more details.\n"                         \
  "\n"                                                                     \
  "You should have received a copy of the GNU General Public License\n"    \
  "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"

#endif
