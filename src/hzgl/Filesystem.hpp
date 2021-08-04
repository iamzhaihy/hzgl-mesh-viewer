#pragma once

#include <string>

namespace hzgl
{
    bool Exists(const std::string& filename);
    std::string GetParentPath(const std::string& filepath);
    std::string GetAbsolutePath(const std::string& relpath);

    bool Copy(const std::string& src, const std::string& dest, bool failIfExists = true);
    bool Move(const std::string& src, const std::string& dest, bool failIfExists = true);
    bool Delete(const std::string& fileName);
} // namespace hzgl