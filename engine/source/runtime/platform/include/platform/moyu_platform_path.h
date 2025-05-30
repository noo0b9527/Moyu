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

#include <string>
#include <string_view>
#include <vector>


namespace moyu
{
    class Path
    {
    public:
        Path()  = delete;
        ~Path() = default;

        Path(const Path&)      = delete;
        Path& operator=(Path&) = delete;

        Path(Path&&)            = delete;
        Path& operator=(Path&&) = delete;

        // Get current application run directory
        static const std::string GetCurrentDirectory();

        // Returns the directory information for the specified path.
        static const std::string GetDirectoryName(const std::string_view path);

        // Returns a relative path from one path to another.
        static const std::string
        GetRelativePath(const std::string_view relativeTo,
                        const std::string_view path);

        // Returns the file name and extension of the specified path string.
        static const std::string GetFileName(const std::string_view path);

        // Returns the file name of the specified path string without the
        // extension.
        static const std::string
        GetFileNameWithoutExtension(const std::string_view path);

        // Returns the extension (including the period ".") of the specified
        // path string.
        static const std::string GetExtension(const std::string_view path);

        // Decompose the path into a string array consisting of each level of
        // directory/file name
        static const std::vector<std::string>
        GetPathSegments(const std::string_view path);

        // Determines whether the specified file or directory exists.
        static bool Exists(const std::string_view path);
    }; // class Path

} // namespace moyu
