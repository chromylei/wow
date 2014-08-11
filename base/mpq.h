#pragma once

#include <string>
#include <memory>
#include "base/basictypes.h"
#include "base/files/file_path.h"

class MPQFile {
 public:
  MPQFile(const std::string& filepath)
      : path_(filepath) {
  }

  bool open();
 private:
  std::string path_;
  std::unique_ptr<char[]> buffer_;
  int64 size_;
  DISALLOW_COPY_AND_ASSIGN(MPQFile);
};

class MPQArchive {
 public:
  MPQArchive(const base::FilePath& filepath);
  bool open();
  bool IsPartical();

  bool ExistsFile(const std::string& filepath);
  bool ReadFile(const std::string& filepath, char* buffer, int size);
  int64 GetFileSize(const std::string& filepath);
 private:
  typedef int HANDLE;
  HANDLE handle_;
  ::base::FilePath filepath_;
  DISALLOW_COPY_AND_ASSIGN(MPQArchive);
};
