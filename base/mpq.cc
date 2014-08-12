#include "wow/base/mpq.h"
#include "wow/stormlib/StormLib/src/StormLib.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"


MPQArchive::MPQArchive(const ::base::FilePath& filepath)
    : archive_(filepath)
    , handle_(-1) {
}

bool MPQArchive::open() {
  DCHECK(handle_ == -1);
  std::string filepath = ::base::WideToUTF8(archive_.value());
  if (!SFileOpenArchive(archive_.value().c_str(), 0,
                        MPQ_OPEN_FORCE_MPQ_V1 | MPQ_OPEN_READ_ONLY,
                        (HANDLE*)&handle_)) {
    return false;
  }

  return true;
}

MPQFilePtr MPQArchive::OpenFile(const ::base::FilePath& filepath) {
  DCHECK(handle_ != -1);
  MPQFilePtr ptr(new MPQFile(filepath, archive_));

  HANDLE fh;
  std::string pathstr = ::base::WideToUTF8(filepath.value());
  if (!SFileOpenFileEx((HANDLE)handle_, pathstr.c_str(), SFILE_OPEN_FROM_MPQ, &fh)) {
    return MPQFilePtr();
  }

  DWORD dwRead;
  DWORD dwFileSize = SFileGetFileSize(fh, NULL);
  ptr->buffer_.reset(new uint8[dwFileSize]);
  ptr->size_ = dwFileSize;
  SFileReadFile(fh, ptr->buffer_.get(), dwFileSize, &dwRead, NULL);
  SFileCloseFile(fh);
  return ptr;
}

bool MPQArchive::ExistsFile(const ::base::FilePath& filepath) {
  DCHECK(handle_ != -1);
  HANDLE fh;
  std::string pathstr = ::base::WideToUTF8(filepath.value());
  if(!SFileOpenFileEx((HANDLE)handle_, pathstr.c_str(), SFILE_OPEN_FROM_MPQ, &fh)) {
    return false;
  }

  SFileCloseFile(fh);
  return true;
}

int64 MPQArchive::GetFileSize(const ::base::FilePath& filepath) {
  DCHECK(handle_ != -1);
  HANDLE fh;
  std::string pathstr = ::base::WideToUTF8(filepath.value());
  if(!SFileOpenFileEx((HANDLE)handle_, pathstr.c_str(), SFILE_OPEN_FROM_MPQ, &fh)) {
    return -1;
  }

  DWORD dwFileSize = SFileGetFileSize(fh, NULL);
  SFileCloseFile(fh);
  return (int64)dwFileSize;
}


bool MPQArchiveSet::AddArchive(const ::base::FilePath& filepath) {
  auto iter = archives_.find(filepath);
  if (iter != archives_.end()) {
    LOG(ERROR) << "MPQArchive \"" << filepath.value() << "\" has been exists.";
    return false;
  }

  MPQArchivePtr ptr(new MPQArchive(filepath));
  if (!ptr->open()) {
    return false;
  }

  archives_.insert(std::make_pair(filepath, ptr));
  return true;
}

MPQFilePtr MPQArchiveSet::OpenFile(const ::base::FilePath& filepath) {
  for (auto iter = archives_.begin(); iter != archives_.end(); ++iter) {
    MPQFilePtr ptr = iter->second->OpenFile(filepath);
    if (ptr.get() != NULL) {
      return ptr;
    }
  }

  return MPQFilePtr();
}
