// references:
//   - https://en.cppreference.com/w/cpp/filesystem
//   - https://en.cppreference.com/w/cpp/filesystem/path
//   - https://www.delftstack.com/howto/c/c-check-if-file-exists/
//   - https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
//   - https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c

#include "Filesystem.hpp"

#if defined(__cpp_lib_filesystem)
#include <filesystem>                  // prefer C++ standard library when possible
#elif defined(_WIN32)
#include <windows.h>
#define FS_MAX_PATH_LEN 32768          // max path length for unicode version
#define _FR_BOOL(x) (x != 0)           // convert from Win32 BOOL to C++ bool
#define _TO_BOOL(x) (x ? TRUE : FALSE) // convert from C++ bool to Win32 BOOL
#elif defined(__APPLE__)
    // something Apple
#elif defined(__linux__)
    // something Linux
#endif

bool hzgl::Exists(const std::string& filename)
{
#if defined(__cpp_lib_filesystem)
    namespace fs = std::filesystem;
    return fs::exists(filename);
#elif defined(_WIN32)
    // TODO
#elif defined(__APPLE__)
    // TODO
#elif defined(__linux__)
    // TODO
#endif

    return false;
}

std::string hzgl::GetParentPath(const std::string& filepath)
{
#if defined(__cpp_lib_filesystem)
    namespace fs = std::filesystem;
    return fs::path(filepath).parent_path().generic_string();
#elif defined(_WIN32)
    // TODO
#elif defined(__APPLE__)
    // TODO
#elif defined(__linux__)
    // TODO
#endif

    // default method: return everything to the last slash
    auto lastSlash = filepath.find_last_not_of("/\\");
    return filepath.substr(0, lastSlash);
}

std::string hzgl::GetAbsolutePath(const std::string& relpath)
{
#if defined(__cpp_lib_filesystem)
    namespace fs = std::filesystem;
    return fs::absolute(relpath).generic_string();
#elif defined(_WIN32)
    // TCHAR buffer[MAX_PATH];
    // DWORD result = GetFullPathName(TEXT(relpath.c_str()), MAX_PATH, buffer, NULL);
    // return std::string<TCHAR>(buffer);
#elif defined(__APPLE__)
    // TODO
#elif defined(__linux__)
    // TODO
#endif

    return relpath;
}

bool hzgl::CopyFile(const std::string& src, const std::string& dest, bool failIfExists)
{
#if defined(__cpp_lib_filesystem)
    namespace fs = std::filesystem;
    // do nothing if src or dest does not exists
    if (!fs::exists(src))
        return false;

    if (fs::exists(dest) && failIfExists)
        return false;

    return fs::copy_file(src, dest, fs::copy_options::overwrite_existing);
#elif defined(_WIN32)
    // BOOL result = CopyFile(TEXT(src), TEXT(dest), _TO_BOOL(failIfExists));
    // return _FR_BOOL(result);
#elif defined(__APPLE__)
    // TODO
#elif defined(__linux__)
    // TODO
#endif

    return false;
}

bool hzgl::MoveFile(const std::string& src, const std::string& dest, bool failIfExists)
{
#if defined(__cpp_lib_filesystem)
    // TODO
#elif defined(_WIN32)
    // BOOL result = MoveFile(TEXT(src), TEXT(dest));
    // return _FR_BOOL(result);
#elif defined(__APPLE__)
    // TODO
#elif defined(__linux__)
    // TODO
#endif

    return false;
}

bool hzgl::DeleteFile(const std::string& fileName)
{
#if defined(__cpp_lib_filesystem)
    // TODO
#elif defined(_WIN32)
    // BOOL result = DeleteFile(TEXT(fileName));
    // return _FR_BOOL(result);
#elif defined(__APPLE__)
    // TODO
#elif defined(__linux__)
    // TODO
#endif

    return false;
}