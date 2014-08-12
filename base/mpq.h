#pragma once

#include <string>
#include <map>
#include <memory>
#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "base/logging.h"

class MPQFile {
 public:
  int64 size() const { return size_;}
  const uint8* data() const;
  uint8* data();
  void reset();
  uint8* current();
  const uint8* current() const;
  const ::base::FilePath& path() const { return path_;}
  const ::base::FilePath& archive_path() const { return archive_;}

  bool read(uint8* v, int bytes);
  bool seek(int32 offset);
 private:
  MPQFile(const ::base::FilePath& filepath, const ::base::FilePath& archive)
      : archive_(archive)
      , path_(filepath)
      , seek_(-1) {
  }
  
  ::base::FilePath archive_;
  ::base::FilePath path_;
  std::unique_ptr<uint8[]> buffer_;
  int64 size_;

  int64 seek_;
  friend class MPQArchive;
  DISALLOW_COPY_AND_ASSIGN(MPQFile);
};

typedef std::shared_ptr<MPQFile> MPQFilePtr;

class MPQArchive {
 public:
  MPQArchive(const base::FilePath& filepath);
  bool open();
  bool IsPartical();

  MPQFilePtr OpenFile(const ::base::FilePath& filepath);
  bool ExistsFile(const ::base::FilePath& filepath);
  bool ReadFile(const ::base::FilePath& filepath, char* buffer, int size);
  int64 GetFileSize(const ::base::FilePath& filepath);

  const ::base::FilePath& path() const { return archive_;}
 private:
  int handle_;
  ::base::FilePath archive_;
  DISALLOW_COPY_AND_ASSIGN(MPQArchive);
};

typedef std::shared_ptr<MPQArchive> MPQArchivePtr;

class MPQArchiveSet {
 public:
  MPQArchiveSet() {}

  bool AddArchive(const ::base::FilePath& filepath);
  MPQFilePtr OpenFile(const ::base::FilePath& filepath);
 private:
  std::map< ::base::FilePath, MPQArchivePtr> archives_;
  DISALLOW_COPY_AND_ASSIGN(MPQArchiveSet);
};

inline const uint8* MPQFile::data() const {
  DCHECK(buffer_.get() != NULL);
  return buffer_.get();
}

inline uint8* MPQFile::data() {
  DCHECK(buffer_.get() != NULL);
  return buffer_.get();
}

inline void MPQFile::reset() {
  seek_ = 0;
}

inline uint8* MPQFile::current() {
  DCHECK(buffer_.get() != NULL);
  return buffer_.get() + seek_;
}

inline const uint8* MPQFile::current() const {
  DCHECK(buffer_.get() != NULL);
  return buffer_.get() + seek_;
}

inline bool MPQFile::read(uint8* v, int bytes) {
  if (bytes + seek_ < size_) {
    memcpy(v, current(), bytes);
    seek_ += bytes;
    return true;
  } else {
    return false;
  }
}

inline bool MPQFile::seek(int32 offset) {
  if (seek_ + offset < size_) {
    seek_ += offset;
    return true;
  } else {
    return false;
  }
}
