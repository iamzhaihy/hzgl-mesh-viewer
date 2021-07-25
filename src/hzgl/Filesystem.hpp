#pragma once

#include <string>

namespace hzgl
{
    bool Exists(const std::string& filename);
    std::string GetParentPath(const std::string& filepath);
    std::string GetAbsolutePath(const std::string& relpath);

    bool CopyFile(const std::string& src, const std::string& dest, bool failIfExists = true);
    bool MoveFile(const std::string& src, const std::string& dest, bool failIfExists = true);
    bool DeleteFile(const std::string& fileName);
} // namespace hzgl