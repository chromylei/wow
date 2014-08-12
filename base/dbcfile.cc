#include "wow/base/dbcfile.h"

#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"

DBCFile::DBCFile(const ::base::FilePath& path)
    : raw_path_(path)
    , record_size_((uint32)-1)
    , record_num_((uint32)-1)
    , field_num_((uint32)-1)
    , string_size_((uint32)-1) {
  if (EndsWith(path.value(), FILE_PATH_LITERAL("item.dbc"), false)) {
    ::base::FilePath::StringType str = path.DirName().value();
    str.append(FILE_PATH_LITERAL("/"));
    str.append(path.BaseName().value());
    str.append(FILE_PATH_LITERAL(".db"));
    act_path_ = ::base::FilePath(str);
  } else {
    act_path_ = path;
  }
}


bool DBCFile::open(MPQArchiveSet* set) {
  MPQFilePtr ptr(set->OpenFile(act_path_));
  if (ptr.get() == NULL) {
    return false;
  }

  if (!ParseHead(ptr.get())) {
    return false;
  }

  CHECK(field_num_ * 4 >= record_size_);
  const uint8* table = ptr->current();
  return true;
}

bool DBCFile::ParseHead(MPQFile* ptr) {
  int dbctype = 0;
  char header[5] = {0};
  CHECK(ptr->read((uint8*)header, 4));
  if (strcmp(header, "WDBC") == 0) {
    dbctype = 1;
  } else if (strcmp(header, "WDB2") == 0) {
    dbctype = 2;
  } else {
    NOTREACHED();
  }

  CHECK(ptr->read((uint8*)&record_num_, sizeof(record_num_)));  // Number of recorder
  CHECK(ptr->read((uint8*)&field_num_, sizeof(field_num_)));  // Number of fields
  CHECK(ptr->read((uint8*)&record_size_, sizeof(record_size_)));  // size of a record
  CHECK(ptr->read((uint8*)&string_size_, sizeof(string_size_)));  // string size
  if (dbctype == 2) {
    CHECK(ptr->seek(28));
    uint32 check;
    ptr->read((uint8*)&check, sizeof(check));
    if (check == 6) {
      ptr->seek(-20);
    } else {
      ptr->seek(-4);
    }
  }

  string_table_ = table_ + record_size_ * record_num_;
  return true;
}

// 
DBCFile::Record::Record(const Record& record) {
  offset_ = record.offset_;
}

DBCFile::Record DBCFile::GetRecord(int num) {
  CHECK_LT(num, record_num_);
  return Record(table_ + record_size_ * num, this);
}

bool DBCFile::Record::GetUint32(int field, uint32* val) {
  CHECK_LT(field, dbcfile_->field_num_);
  *val = *(uint32*)(offset_ + field * 4);
  return true;
}

bool DBCFile::Record::GetInt32(int field, int32* val) {
  CHECK_LT(field, dbcfile_->field_num_);
  *val = *(int32*)(offset_ + field * 4);
  return true;
}

bool DBCFile::Record::GetFloat(int field, float* val) {
  CHECK_LT(field, dbcfile_->field_num_);
  *val = *(float*)(offset_ + field * 4);
  return true;
}

bool DBCFile::Record::GetString(int field, char** val) {
  CHECK_LT(field, dbcfile_->field_num_);
  uint32 str_offset;
  if (!GetUint32(field, &str_offset)) {
    return false;
  }

  if (str_offset > dbcfile_->string_size_) {
    str_offset = 0;
    return false;
  }

  *val = (char*)(dbcfile_->string_table_ + str_offset);
  return true;
}
