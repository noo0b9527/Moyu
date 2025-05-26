/*
MIT License

Copyright (c) 2025 noo0b9527

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "moyu_platform_defines.h"
#include "moyu_platform_export.h"

#include <string_view>

namespace moyu::platform
{
    class MOYU_PLATFORM_API Path
    {
    public:
        Path()  = delete;
        ~Path() = default;

        Path(const Path&)      = delete;
        Path& operator=(Path&) = delete;

        Path(Path&&)            = delete;
        Path& operator=(Path&&) = delete;

        // Get current application run path
        static const std::string_view GetRunPath();

        // Returns the file name and extension of the specified path string
        static const std::string_view
        GetDirectoryName(const std::string_view path);

        // Determines whether the specified file or directory exists.
        static bool Exists(std::string_view path);
    };

} // namespace moyu::platform
