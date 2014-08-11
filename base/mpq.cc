#include "wow/base/mpq.h"
#include "Stormlib.h"
#include "base/strings/utf_string_conversions.h"

bool MPQFile::open() {
  return true;
}


MPQArchive::MPQArchive(const ::base::FilePath& filepath)
    : filepath_(filepath)
    , handle_(-1) {
}

bool MPQArchive::open() {
  std::string filepath = ::base::WideToUTF8(filepath_);
  if (!SFileOpenArchive(filepath, 0,
                        MPQ_OPEN_FORCE_MPQ_V1 | MPQ_OPEN_READ_ONLY,
                        &handle_)) {
    return false;
  }
}
