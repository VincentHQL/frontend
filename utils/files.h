#pragma once

#include <sys/types.h>

#include <chrono>
#include <string>

namespace redroid {
bool FileExists(const std::string& path);
bool FileHasContent(const std::string& path);
std::vector<std::string> DirectoryContents(const std::string& path);
bool DirectoryExists(const std::string& path);
bool IsDirectoryEmpty(const std::string& path);
off_t FileSize(const std::string& path);
bool RemoveFile(const std::string& file);
bool RenameFile(const std::string& old_name, const std::string& new_name);
std::string ReadFile(const std::string& file);
std::chrono::system_clock::time_point FileModificationTime(const std::string& path);
std::string cpp_dirname(const std::string& str);
std::string cpp_basename(const std::string& str);

// The returned value may contain .. or . if these are present in the path
// argument.
// path must not contain ~
std::string AbsolutePath(const std::string& path);

std::string CurrentDirectory();

struct FileSizes {
  off_t sparse_size;
  off_t disk_size;
};
FileSizes SparseFileSizes(const std::string& path);
}  // namespace redroid
