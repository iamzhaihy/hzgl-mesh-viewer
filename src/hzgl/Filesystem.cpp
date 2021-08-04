// references:
//   - https://en.cppreference.com/w/cpp/filesystem
//   - https://en.cppreference.com/w/cpp/filesystem/path
//   - https://www.delftstack.com/howto/c/c-check-if-file-exists/
//   - https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
//   - https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c

#include "Filesystem.hpp"

#include <cstdio>

#define HZGL_LOG_ERROR(msg) \
    fprintf(stderr, "[ERROR] %s (line %d): %s\n", __FILE__, __LINE__, msg);

#if   _MSVC_LANG >= 201703L || __cplusplus >= 201703L
    #define HZGL_CXX17
#elif _MSVC_LANG >= 201403L || __cplusplus >= 201403L
    #define HZGL_CXX14
#endif

#if defined(HZGL_CXX17)
    #include <filesystem>
    namespace _hzfs = std::filesystem;
#elif defined(HZGL_CXX14)
    #include <experimental/filesystem>
    namespace _hzfs = std::experimental::filesystem::v1;
#elif defined(_WIN32)
    #include <windows.h>
    #define FS_MAX_PATH_LEN 32768          // max path length for unicode version
    #define _FR_BOOL(x) (x != 0)           // convert from Win32 BOOL to C++ bool
    #define _TO_BOOL(x) (x ? TRUE : FALSE) // convert from C++ bool to Win32 BOOL    
#elif defined(__APPLE__)
    // something Apple
    HZGL_LOG_ERROR("Not implemented yet on macOS")
#elif defined(__linux__)
    // something Linux
    HZGL_LOG_ERROR("Not implemented yet on Linux")
#endif

bool hzgl::Exists(const std::string& filename)
{
#if defined(HZGL_CXX17) || defined(HZGL_CXX14)
    return _hzfs::exists(filename);
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
#if defined(HZGL_CXX17) || defined(HZGL_CXX14)
    return _hzfs::path(filepath).parent_path().generic_string();
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
#if defined(HZGL_CXX17) || defined(HZGL_CXX14)
    return _hzfs::absolute(relpath).generic_string();
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

bool hzgl::Copy(const std::string& src, const std::string& dest, bool failIfExists)
{
#if defined(HZGL_CXX17) || defined(HZGL_CXX14)
    // do nothing if src or dest does not exists
    if (!_hzfs::exists(src))
        return false;

    if (_hzfs::exists(dest) && failIfExists)
        return false;

    return _hzfs::copy_file(src, dest, _hzfs::copy_options::overwrite_existing);
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

bool hzgl::Move(const std::string& src, const std::string& dest, bool failIfExists)
{
    HZGL_LOG_ERROR("Function not implemented yet")

#if defined(HZGL_CXX17) || defined(HZGL_CXX14)
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

bool hzgl::Delete(const std::string& fileName)
{
    HZGL_LOG_ERROR("Function not implemented yet")

#if defined(HZGL_CXX17) || defined(HZGL_CXX14)
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

#undef HZGL_LOG_ERROR